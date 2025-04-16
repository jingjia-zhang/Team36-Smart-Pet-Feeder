#include "SerialPort.h"
#include <stdexcept>
#include <unistd.h>
#include <wiringPi.h>

SerialPort::SerialPort(const Config& config) : config_(config)
{
    if (wiringPiSetup() == -1)
    {
        throw std::runtime_error("WiringPi初始化失败");
    }
}

SerialPort::~SerialPort()
{
    close();
}

SerialPort& SerialPort::getInstance()
{
    static SerialPort port;
    return port;
}

bool SerialPort::open()
{
    if (is_open_)
        return true;

    fd_ = serialOpen(config_.device.c_str(), config_.baudrate);
    if (fd_ < 0)
    {
        is_open_ = false;
        return false;
    }

    // 设置超时参数
    struct termios options;
    tcgetattr(fd_, &options);
    options.c_cc[VTIME] = config_.timeout_ms / 100; // 单位：100ms
    options.c_cc[VMIN] = 0;
    tcsetattr(fd_, TCSANOW, &options);

    is_open_ = true;
    return true;
}

void SerialPort::close()
{
    if (is_open_)
    {
        serialClose(fd_);
        fd_ = -1;
        is_open_ = false;
    }
}

bool SerialPort::isOpen() const
{
    return is_open_;
}

ssize_t SerialPort::send(const std::string& data)
{
    if (!is_open_)
        return -1;
    return write(fd_, data.c_str(), data.size());
}

ssize_t SerialPort::send(const uint8_t* data, size_t length)
{
    if (!is_open_)
        return -1;
    return write(fd_, data, length);
}

std::string SerialPort::receive()
{
    std::string buffer;
    if (!is_open_)
        return buffer;

    time_t start = time(nullptr);
    while (time(nullptr) - start < config_.timeout_ms / 1000)
    {
        int avail = serialDataAvail(fd_);
        if (avail > 0)
        {
            char c = serialGetchar(fd_);
            buffer += c;
            if (c == '\n' || buffer.size() >= config_.max_buffer_size)
            {
                break;
            }
        }
        usleep(10000); // 10ms间隔
    }
    return buffer;
}

ssize_t SerialPort::receive(uint8_t* buffer, size_t max_length)
{
    if (!is_open_)
        return -1;

    size_t received = 0;
    time_t start = time(nullptr);

    while (received < max_length &&
        (time(nullptr) - start < config_.timeout_ms / 1000))
    {
        int avail = serialDataAvail(fd_);
        if (avail > 0)
        {
            buffer[received++] = static_cast<uint8_t>(serialGetchar(fd_));
        }
        else
        {
            usleep(10000); // 10ms间隔
        }
    }
    return static_cast<ssize_t>(received);
}

void SerialPort::flushInput()
{
    if (is_open_)
        serialFlush(fd_);
}

void SerialPort::flushOutput()
{
    if (is_open_)
        serialFlush(fd_);
}

size_t SerialPort::available() const
{
    return is_open_ ? serialDataAvail(fd_) : 0;
}

void SerialPort::reconfigure(const Config& new_config)
{
    if (is_open_)
    {
        close();
        config_ = new_config;
        open();
    }
    else
    {
        config_ = new_config;
    }
}

SerialPort::Config SerialPort::getConfiguration() const
{
    return config_;
}

// #include "SerialPort.h"
// #include <iostream>
// #include <thread>

// int main() {
//     try {
//         // 配置串口参数
//         SerialPort::Config config;
//         config.device = "/dev/ttyAMA0";
//         config.baudrate = 115200;
//         config.timeout_ms = 2000;

//         // 创建串口对象
//         SerialPort uart(config);

//         // 打开端口
//         if (!uart.open()) {
//             std::cerr << "无法打开串口！" << std::endl;
//             return 1;
//         }

//         // 发送数据
//         std::string message = "Hello from class!\n";
//         uart.send(message);

//         // 接收数据
//         std::string response = uart.receive();
//         if (!response.empty()) {
//             std::cout << "收到响应: " << response << std::endl;
//         } else {
//             std::cout << "接收超时" << std::endl;
//         }

//         // 二进制数据传输示例
//         uint8_t binary_data[] = {0xAA, 0x55, 0x01};
//         uart.send(binary_data, sizeof(binary_data));

//     } catch (const std::exception& e) {
//         std::cerr << "错误发生: " << e.what() << std::endl;
//         return 1;
//     }

//     return 0;
// }
