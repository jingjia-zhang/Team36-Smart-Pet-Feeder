#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <string>
#include <wiringSerial.h>
#include <termios.h>   // Add termios structure definition
#include <fcntl.h>     // Add file control options
#include <sys/ioctl.h> // Adding IO Control Commands
#include <ctime>
class SerialPort
{
public:
    // Serial Port Configuration Parameters Structure
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
    // Constructor/Destructor
    explicit SerialPort(const Config& config = Config());
    ~SerialPort();

    // Basic Functions
    bool open();
    void close();
    bool isOpen() const;

    // data transmission and reception
    ssize_t send(const std::string& data);
    ssize_t send(const uint8_t* data, size_t length);
    std::string receive();
    ssize_t receive(uint8_t* buffer, size_t max_length);

    // Advanced Features
    void flushInput();
    void flushOutput();
    size_t available() const;

    // configuration management
    void reconfigure(const Config& new_config);
    Config getConfiguration() const;

private:
    Config config_;
    int fd_ = -1;
    bool is_open_ = false;

    // Disable copy constructs and assignments
    SerialPort(const SerialPort&) = delete;
    SerialPort& operator=(const SerialPort&) = delete;
};

#endif // SERIAL_PORT_H
