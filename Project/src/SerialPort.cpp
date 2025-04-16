#include "SerialPort.h"
#include <stdexcept>
#include <unistd.h>
#include <wiringPi.h>

// Constructor: setup wiringPi and store config
SerialPort::SerialPort(const Config& config) : config_(config)
{
    if (wiringPiSetup() == -1)
    {
        throw std::runtime_error("WiringPi initialization failed");
    }
}

SerialPort::~SerialPort()
{
    close();
}

// Destructor: close port if open
SerialPort& SerialPort::getInstance()
{
    static SerialPort port;
    return port;
}

// Open the serial port
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

    // Set timeout attributes
    struct termios options;
    tcgetattr(fd_, &options);
    options.c_cc[VTIME] = config_.timeout_ms / 100; // timeout in 100ms units
    options.c_cc[VMIN] = 0;
    tcsetattr(fd_, TCSANOW, &options);

    is_open_ = true;
    return true;
}

// Close serial port
void SerialPort::close()
{
    if (is_open_)
    {
        serialClose(fd_);
        fd_ = -1;
        is_open_ = false;
    }
}

// Check if port is open
bool SerialPort::isOpen() const
{
    return is_open_;
}

// Send text data
ssize_t SerialPort::send(const std::string& data)
{
    if (!is_open_)
        return -1;
    return write(fd_, data.c_str(), data.size());
}

// Send binary data
ssize_t SerialPort::send(const uint8_t* data, size_t length)
{
    if (!is_open_)
        return -1;
    return write(fd_, data, length);
}

// Receive data as string with timeout
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
        usleep(10000); // 10ms delay
    }
    return buffer;
}

// Receive binary data
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
            usleep(10000); // wait 10ms
        }
    }
    return static_cast<ssize_t>(received);
}

// Clear input buffer
void SerialPort::flushInput()
{
    if (is_open_)
        serialFlush(fd_);
}

// Clear output buffer
void SerialPort::flushOutput()
{
    if (is_open_)
        serialFlush(fd_);
}

// Get number of available bytes
size_t SerialPort::available() const
{
    return is_open_ ? serialDataAvail(fd_) : 0;
}

// Reconfigure port settings
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

// Return current config
SerialPort::Config SerialPort::getConfiguration() const
{
    return config_;
}

// #include "SerialPort.h"
// #include <iostream>
// #include <thread>

// int main() {
//     try {
//         // Step 1: Define the serial port configuration
//         SerialPort::Config config;
//         config.device = "/dev/ttyAMA0";// Serial port device (adjust for your hardware)
//         config.baudrate = 115200;// Communication speed
//         config.timeout_ms = 2000;// Timeout in milliseconds

//         // Step 2: Create a SerialPort object with the config
//         SerialPort uart(config);

//         // Step 3: Open the serial port
//         if (!uart.open()) {
//             std::cerr << "Failed to open serial port!" << std::endl;
//             return 1;
//         }

//         // Step 4: Send a plain text message
//         std::string message = "Hello from class!\n";
//         uart.send(message);

//         // Step 5: Wait and read response from the serial port
//         std::string response = uart.receive();
//         if (!response.empty()) {
//             std::cout << "Received response: " << response << std::endl;
//         } else {
//             std::cout << "Receive timeout or no data" << std::endl;
//         }

//         // Step 6: Send a binary message (example payload)
//         uint8_t binary_data[] = {0xAA, 0x55, 0x01};
//         uart.send(binary_data, sizeof(binary_data));

//     } catch (const std::exception& e) {
//         std::cerr << "An error occurred: " << e.what() << std::endl;
//         return 1;
//     }

//     return 0;
// }
