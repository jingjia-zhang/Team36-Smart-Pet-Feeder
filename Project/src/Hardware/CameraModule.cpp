#include "Hardware/CameraModule.hpp"
#include "Communication/WebSocketClient.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdexcept>
#include <cstring>

CameraModule::CameraModule(std::shared_ptr<WebSocketClient> webSocketClient)
    : m_deviceId("/dev/video0")
    , m_width(640)
    , m_height(480)
    , m_fps(30)
    , m_bitrateKbps(1000)
    , m_initialized(false)
    , m_streaming(false)
    , m_webSocketClient(webSocketClient) {
}

CameraModule::~CameraModule() {
    stopStreaming();
}

bool CameraModule::initialize(const std::string& deviceId, int width, int height) {
    if (m_initialized) {
        return true;
    }
    
    m_deviceId = deviceId;
    m_width = width;
    m_height = height;
    
    int fd = -1;

    try {
        // Open the video device
        fd = open(m_deviceId.c_str(), O_RDWR | O_NONBLOCK, 0);
        if (fd == -1) {
            throw std::runtime_error("Failed to open camera device: " + std::string(strerror(errno)));
        }

        // Query device capability
        v4l2_capability cap;
        if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
            throw std::runtime_error("Failed to query device capabilities: " + std::string(strerror(errno)));
        }

        // Check if the device supports video capture
        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
            throw std::runtime_error("Device does not support video capture");
        }

        // Check if the device supports streaming I/O
        if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
            throw std::runtime_error("Device does not support streaming I/O");
        }

        // Set video format
        v4l2_format fmt;
        memset(&fmt, 0, sizeof(fmt));
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = m_width;
        fmt.fmt.pix.height = m_height;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; // YUYV format
        fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

        if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
            throw std::runtime_error("Failed to set video format: " + std::string(strerror(errno)));
        }

        // Verify the format of the actual settings
        if (ioctl(fd, VIDIOC_G_FMT, &fmt) == -1) {
            throw std::runtime_error("Failed to get video format: " + std::string(strerror(errno)));
        }

        if (fmt.fmt.pix.width != m_width || fmt.fmt.pix.height != m_height) {
            std::cerr << "Warning: Camera does not support requested resolution "
                      << m_width << "x" << m_height << std::endl;
            std::cerr << "Using supported resolution "
                      << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height << std::endl;

            m_width = fmt.fmt.pix.width;
            m_height = fmt.fmt.pix.height;
        }

        // Set frame rate
        v4l2_streamparm parm;
        memset(&parm, 0, sizeof(parm));
        parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        parm.parm.capture.timeperframe.numerator = 1;
        parm.parm.capture.timeperframe.denominator = m_fps;

        if (ioctl(fd, VIDIOC_S_PARM, &parm) == -1) {
            std::cerr << "Warning: Failed to set frame rate: " << strerror(errno) << std::endl;
        } else {
            // Obtain the actual frame rate
            if (ioctl(fd, VIDIOC_G_PARM, &parm) != -1) {
                m_fps = parm.parm.capture.timeperframe.denominator /
                        parm.parm.capture.timeperframe.numerator;
                std::cout << "Actual frame rate: " << m_fps << " FPS" << std::endl;
            }
        }

        // Initialize memory mapping buffer
        v4l2_requestbuffers req;
        memset(&req, 0, sizeof(req));
        req.count = 4; // Request 4 buffers
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
            throw std::runtime_error("Failed to request buffer: " + std::string(strerror(errno)));
        }

        // Store buffer information
        m_buffers.resize(req.count);
        for (unsigned int i = 0; i < req.count; ++i) {
            v4l2_buffer buf;
            memset(&buf, 0, sizeof(buf));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;

            if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
                throw std::runtime_error("Failed to query buffer: " + std::string(strerror(errno)));
            }

            m_buffers[i].length = buf.length;
            m_buffers[i].start = mmap(NULL, buf.length,
                                      PROT_READ | PROT_WRITE,
                                      MAP_SHARED,
                                      fd, buf.m.offset);

            if (m_buffers[i].start == MAP_FAILED) {
                throw std::runtime_error("Failed to map buffer: " + std::string(strerror(errno)));
            }

            // Put the buffer into the queue
            if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
                throw std::runtime_error("Failed to queue buffer: " + std::string(strerror(errno)));
            }
        }

        // Turn off the device (it will reopen in streamLoop)
        close(fd);
        fd = -1;

        m_fd = -1; // Mark as unopened
        m_initialized = true;

        std::cout << "Camera initialized: " << m_deviceId
                  << " (" << m_width << "x" << m_height << "@" << m_fps << "fps)" << std::endl;

        return true;
    } catch (const std::exception& e) {
        if (fd != -1) {
            close(fd);
        }

        // Release mapped buffer
        for (auto& buffer : m_buffers) {
            if (buffer.start != MAP_FAILED) {
                munmap(buffer.start, buffer.length);
            }
        }

        std::cerr << "Camera initialization failed: " << e.what() << std::endl;
        return false;
    }
}

bool CameraModule::startStreaming() {
    if (!m_initialized) {
        std::cerr << "Camera not initialized" << std::endl;
        return false;
    }
    
    if (m_streaming) {
        return true;
    }
    
    try {
        m_streaming = true;
        m_streamThread = std::make_unique<std::thread>(&CameraModule::streamLoop, this);
        std::cout << "Camera streaming started" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to start streaming: " << e.what() << std::endl;
        m_streaming = false;
        return false;
    }
}

bool CameraModule::stopStreaming() {
    if (!m_streaming) {
        return true;
    }
    
    m_streaming = false;
    if (m_streamThread && m_streamThread->joinable()) {
        m_streamThread->join();
    }
    
    std::cout << "Camera streaming stopped" << std::endl;
    return true;
}

bool CameraModule::isStreaming() const {
    return m_streaming;
}

bool CameraModule::isInitialized() const {
    return m_initialized;
}

void CameraModule::setResolution(int width, int height) {
    m_width = width;
    m_height = height;
    
    // If already initialized, the camera needs to be reinitialized
    if (m_initialized) {
        m_initialized = false;
        initialize(m_deviceId, m_width, m_height);
    }
}

void CameraModule::setFramerate(int fps) {
    m_fps = fps;
}

void CameraModule::setBitrate(int bitrateKbps) {
    m_bitrateKbps = bitrateKbps;
}

void CameraModule::streamLoop() {
    // Restart the device
    m_fd = open(m_deviceId.c_str(), O_RDWR | O_NONBLOCK, 0);
    if (m_fd == -1) {
        std::cerr << "Failed to open camera device in streaming thread" << std::endl;
        m_streaming = false;
        return;
    }

    try {
        // Start data flow
        v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(m_fd, VIDIOC_STREAMON, &type) == -1) {
            throw std::runtime_error("Failed to start streaming: " + std::string(strerror(errno)));
        }

        std::vector<uint8_t> rawFrame;
        std::vector<uint8_t> encodedFrame;

        // Frame Rate Control
        auto lastFrameTime = std::chrono::high_resolution_clock::now();
        const auto frameDuration = std::chrono::milliseconds(1000 / m_fps);

        while (m_streaming) {
            // Capture a frame
            if (!captureFrame(rawFrame)) {
                std::cerr << "Failed to capture frame" << std::endl;
                continue;
            }

            // Encoding frame
            if (!encodeFrame(rawFrame, encodedFrame)) {
                std::cerr << "Failed to encode frame" << std::endl;
                continue;
            }

            // Send frames to WebSocket
            if (m_webSocketClient && m_webSocketClient->isConnected()) {
                m_webSocketClient->sendBinary(encodedFrame);
            }

            // Control frame rate
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = now - lastFrameTime;

            if (elapsed < frameDuration) {
                std::this_thread::sleep_for(frameDuration - elapsed);
            }

            lastFrameTime = now;
        }
    } catch (const std::exception& e) {
        std::cerr << "Streaming error: " << e.what() << std::endl;
    }

    // Stop data flow and shut down the device
    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(m_fd, VIDIOC_STREAMOFF, &type) == -1) {
        std::cerr << "Failed to stop streaming: " << strerror(errno) << std::endl;
    }

    close(m_fd);
    m_fd = -1;
    m_streaming = false;
}

bool CameraModule::captureFrame(std::vector<uint8_t>& frameData) {
    if (!m_initialized || m_fd == -1) {
        std::cerr << "Camera not initialized or not opened" << std::endl;
        return false;
    }

    fd_set fds;
    struct timeval tv;
    int r;

    // Waiting for data to be ready
    FD_ZERO(&fds);
    FD_SET(m_fd, &fds);

    // Set timeout period
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    r = select(m_fd + 1, &fds, NULL, NULL, &tv);
    if (r == -1) {
        if (errno != EINTR) {
            std::cerr << "Select error: " << strerror(errno) << std::endl;
            return false;
        }
        return false;
    }

    if (r == 0) {
        std::cerr << "Select timeout" << std::endl;
        return false;
    }

    // Retrieve buffer from queue
    v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (ioctl(m_fd, VIDIOC_DQBUF, &buf) == -1) {
        if (errno != EAGAIN) {
            std::cerr << "Failed to dequeue buffer: " << strerror(errno) << std::endl;
            return false;
        }
        return false;
    }

    // Copy frame data
    frameData.resize(buf.bytesused);
    memcpy(frameData.data(), m_buffers[buf.index].start, buf.bytesused);

    // Put the buffer back into the queue
    if (ioctl(m_fd, VIDIOC_QBUF, &buf) == -1) {
        std::cerr << "Failed to requeue buffer: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool CameraModule::encodeFrame(const std::vector<uint8_t>& rawFrame, std::vector<uint8_t>& encodedFrame) {
    try {
        // Create OpenCV matrix to represent raw frames
        cv::Mat frame(m_height, m_width, CV_8UC2, const_cast<uint8_t*>(rawFrame.data()));

        // Convert to BGR format
        cv::Mat bgrFrame;
        cv::cvtColor(frame, bgrFrame, cv::COLOR_YUV2BGR_YUYV);

        // Set JPEG encoding parameters
        std::vector<int> encodeParams;
        encodeParams.push_back(cv::IMWRITE_JPEG_QUALITY);
        encodeParams.push_back(80); // The quality range is 0-100, with 80 being a good balance point

        // Execute Code
        return cv::imencode(".jpg", bgrFrame, encodedFrame, encodeParams);
    } catch (const std::exception& e) {
        std::cerr << "Encoding error: " << e.what() << std::endl;
        return false;
    }
}


bool CameraModule::sendFrame(const std::vector<uint8_t>& encodedFrame) {
    if (!m_webSocketClient || !m_webSocketClient->isConnected()) {
        return false;
    }
    
    // Sending binary data
    return m_webSocketClient->sendBinary(encodedFrame);
}
