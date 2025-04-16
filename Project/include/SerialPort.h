#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <string>
#include <wiringSerial.h>
#include <termios.h>   // 添加termios结构体定义
#include <fcntl.h>     // 添加文件控制选项
#include <sys/ioctl.h> // 添加IO控制命令
#include <ctime>
class SerialPort
{
public:
    // 串口配置参数结构体
    struct Config
    {
        Config() : device("/dev/ttyAMA4"),
            baudrate(115200),
            timeout_ms(1000),
            max_buffer_size(512) {
        }

        std::string device;
        int baudrate;
        int timeout_ms;
        size_t max_buffer_size;
    };
    static SerialPort& getInstance();
    // 构造函数/析构函数
    explicit SerialPort(const Config& config = Config());
    ~SerialPort();

    // 基础功能
    bool open();
    void close();
    bool isOpen() const;

    // 数据收发
    ssize_t send(const std::string& data);
    ssize_t send(const uint8_t* data, size_t length);
    std::string receive();
    ssize_t receive(uint8_t* buffer, size_t max_length);

    // 高级功能
    void flushInput();
    void flushOutput();
    size_t available() const;

    // 配置管理
    void reconfigure(const Config& new_config);
    Config getConfiguration() const;

private:
    Config config_;
    int fd_ = -1;
    bool is_open_ = false;

    // 禁用复制构造和赋值
    SerialPort(const SerialPort&) = delete;
    SerialPort& operator=(const SerialPort&) = delete;
};

#endif // SERIAL_PORT_H
