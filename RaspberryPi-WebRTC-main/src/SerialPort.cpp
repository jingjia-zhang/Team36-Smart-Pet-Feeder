#include "SerialPort.h"  // Include the header file for the SerialPort class
#include <stdexcept>      // Include the standard exception library for throwing errors
#include <unistd.h>       // Include the Unix standard library for sleep functions
#include <wiringPi.h>     // Include the WiringPi library for Raspberry Pi GPIO access

// SerialPort constructor: Initializes with a given configuration
SerialPort::SerialPort(const Config &config) : config_(config)
{
    if (wiringPiSetup() == -1)  // Initialize WiringPi, returns -1 if initialization fails
    {
        throw std::runtime_error("WiringPi初始化失败");  // Throw an error if WiringPi initialization fails
    }
}

// SerialPort destructor: Closes the serial port when the object is destroyed
SerialPort::~SerialPort()
{
    close();  // Calls the close function to release the serial port
}

// Opens the serial port with the configuration provided in the constructor
bool SerialPort::open()
{
    if (is_open_)  // If the serial port is already open, return true
        return true;

    fd_ = serialOpen(config_.device.c_str(), config_.baudrate);  // Open the serial port with the given device and baud rate
    if (fd_ < 0)  // If opening the port failed, set is_open_ to false and return false
    {
        is_open_ = false;
        return false;
    }

    // Set timeout parameters
    struct termios options;  // Declare a struct to store serial port settings
    tcgetattr(fd_, &options);  // Get the current serial port settings
    options.c_cc[VTIME] = config_.timeout_ms / 100;  // Set timeout value in 100ms units
    options.c_cc[VMIN] = 0;  // Set the minimum number of characters to read
    tcsetattr(fd_, TCSANOW, &options);  // Apply the new settings to the serial port

    is_open_ = true;  // Set is_open_ to true since the port is successfully opened
    return true;
}

// Closes the serial port if it is open
void SerialPort::close()
{
    if (is_open_)  // If the serial port is open
    {
        serialClose(fd_);  // Close the serial port
        fd_ = -1;  // Reset file descriptor to an invalid value
        is_open_ = false;  // Mark the port as closed
    }
}

// Checks if the serial port is open
bool SerialPort::isOpen() const
{
    return is_open_;  // Return the current status of the serial port (open or closed)
}

// Sends data as a string to the serial port
ssize_t SerialPort::send(const std::string &data)
{
    if (!is_open_)  // If the port is not open, return -1 to indicate failure
        return -1;
    return write(fd_, data.c_str(), data.size());  // Write the string data to the serial port
}

// Sends binary data (uint8_t array) to the serial port
ssize_t SerialPort::send(const uint8_t *data, size_t length)
{
    if (!is_open_)  // If the port is not open, return -1 to indicate failure
        return -1;
    return write(fd_, data, length);  // Write the binary data to the serial port
}

// Receives data from the serial port as a string
std::string SerialPort::receive()
{
    std::string buffer;  // String to store received data
    if (!is_open_)  // If the port is not open, return an empty buffer
        return buffer;

    time_t start = time(nullptr);  // Get the current time to track timeout
    while (time(nullptr) - start < config_.timeout_ms / 1000)  // Loop until the timeout is reached
    {
        int avail = serialDataAvail(fd_);  // Check if data is available to read
        if (avail > 0)  // If data is available
        {
            char c = serialGetchar(fd_);  // Read a character from the serial port
            buffer += c;  // Append the character to the buffer
            if (c == '\n' || buffer.size() >= config_.max_buffer_size)  // Stop if newline is encountered or buffer size is reached
            {
                break;
            }
        }
        usleep(10000);  // Sleep for 10ms before checking again
    }
    return buffer;  // Return the received data
}

// Receives binary data (uint8_t array) from the serial port
ssize_t SerialPort::receive(uint8_t *buffer, size_t max_length)
{
    if (!is_open_)  // If the port is not open, return -1 to indicate failure
        return -1;

    size_t received = 0;  // Number of bytes received
    time_t start = time(nullptr);  // Get the current time to track timeout

    while (received < max_length &&
           (time(nullptr) - start < config_.timeout_ms / 1000))  // Loop until the timeout or buffer is full
    {
        int avail = serialDataAvail(fd_);  // Check if data is available to read
        if (avail > 0)  // If data is available
        {
            buffer[received++] = static_cast<uint8_t>(serialGetchar(fd_));  // Read a byte and store it in the buffer
        }
        else
        {
            usleep(10000);  // Sleep for 10ms before checking again
        }
    }
    return static_cast<ssize_t>(received);  // Return the number of bytes received
}

// Clears the input buffer of the serial port
void SerialPort::flushInput()
{
    if (is_open_)  // If the port is open
        serialFlush(fd_);  // Flush the input buffer
}

// Clears the output buffer of the serial port
void SerialPort::flushOutput()
{
    if (is_open_)  // If the port is open
        serialFlush(fd_);  // Flush the output buffer
}

// Returns the number of bytes available to read from the serial port
size_t SerialPort::available() const
{
    return is_open_ ? serialDataAvail(fd_) : 0;  // If the port is open, return the available data count, otherwise return 0
}

// Reconfigures the serial port with a new configuration
void SerialPort::reconfigure(const Config &new_config)
{
    if (is_open_)  // If the port is open
    {
        close();  // Close the port
        config_ = new_config;  // Update the configuration
        open();  // Reopen the port with the new configuration
    }
    else
    {
        config_ = new_config;  // Update the configuration if the port is not open
    }
}

// Returns the current configuration of the serial port
SerialPort::Config SerialPort::getConfiguration() const
{
    return config_;  // Return the current configuration
}

// Example code for using the SerialPort class (commented out in the provided code)
// #include "SerialPort.h"
// #include <iostream>
// #include <thread>

// int main() {
//     try {
//         // Configure serial port settings
//         SerialPort::Config config;
//         config.device = "/dev/ttyAMA0";  // Device file for the serial port
//         config.baudrate = 115200;  // Baud rate for the serial port
//         config.timeout_ms = 2000;  // Timeout for receiving data in milliseconds

//         // Create a SerialPort object
//         SerialPort uart(config);

//         // Open the serial port
//         if (!uart.open()) {
//             std::cerr << "Unable to open serial port!" << std::endl;
//             return 1;
//         }

//         // Send a message
//         std::string message = "Hello from class!\n";
//         uart.send(message);

//         // Receive a response
//         std::string response = uart.receive();
//         if (!response.empty()) {
//             std::cout << "Received response: " << response << std::endl;
//         } else {
//             std::cout << "Receive timeout" << std::endl;
//         }

//         // Send binary data as an example
//         uint8_t binary_data[] = {0xAA, 0x55, 0x01};
//         uart.send(binary_data, sizeof(binary_data));

//     } catch (const std::exception& e) {
//         std::cerr << "Error occurred: " << e.what() << std::endl;
//         return 1;
//     }

//     return 0;
// }
