#ifndef CAMERA_MODULE_HPP
#define CAMERA_MODULE_HPP

#include <string>
#include <atomic>
#include <thread>
#include <memory>
#include <functional>
#include <vector>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

struct Buffer {
    void* start;
    size_t length;
};


class WebSocketClient;

class CameraModule {
public:
    CameraModule(std::shared_ptr<WebSocketClient> webSocketClient);
    ~CameraModule();

    // Initialize camera
    bool initialize(const std::string& deviceId = "/dev/video0", int width = 640, int height = 480);
    
    // Start/Stop Video Streaming
    bool startStreaming();
    bool stopStreaming();
    
    // Check the camera status
    bool isStreaming() const;
    bool isInitialized() const;
    
    // Set video quality parameters
    void setResolution(int width, int height);
    void setFramerate(int fps);
    void setBitrate(int bitrateKbps);
    
private:
    // Camera parameters
    std::string m_deviceId;
    int m_width;
    int m_height;
    int m_fps;
    int m_bitrateKbps;

    int m_fd; // Device file descriptor
    std::vector<Buffer> m_buffers; // Video Buffer
    
    // Status Flag
    std::atomic<bool> m_initialized;
    std::atomic<bool> m_streaming;
    
    // Video streaming thread
    std::unique_ptr<std::thread> m_streamThread;
    
    // WebSocket client
    std::shared_ptr<WebSocketClient> m_webSocketClient;
    
    // Video stream processing function
    void streamLoop();
    
    // Read a frame of data
    bool captureFrame(std::vector<uint8_t>& frameData);
    
    // Encoding frame
    bool encodeFrame(const std::vector<uint8_t>& rawFrame, std::vector<uint8_t>& encodedFrame);
    
    // Send frames to WebSocket
    bool sendFrame(const std::vector<uint8_t>& encodedFrame);
};

#endif // CAMERA_MODULE_HPP
