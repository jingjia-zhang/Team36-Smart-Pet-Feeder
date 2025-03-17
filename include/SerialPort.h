#ifndef SERIAL_PORT_H  // Check if SERIAL_PORT_H is not defined to prevent multiple inclusions of this header file
#define SERIAL_PORT_H  // Define SERIAL_PORT_H to ensure the contents of the header are included only once

#include <string>  // Include the string library for using std::string type to handle text data
#include <wiringSerial.h>  // Include the wiringSerial.h library for serial communication functionality on Raspberry Pi
#include <termios.h>  // Add the termios struct definition
#include <fcntl.h>  // Add file control options
#include <sys/ioctl.h>  // Adding IO control commands
#include <ctime>  // Include the ctime library for time-related functions, such as getting current time and manipulating time values
class SerialPort
{
public:
    // Serial port configuration parameter structure
    struct Config
    {
        Config() : device("/dev/ttyAMA0"),  // Constructor initializer list to set the default serial port device to "/dev/ttyAMA0"
                   baudrate(9600),  // Initialize the baud rate to 9600 for serial communication
                   timeout_ms(1000),  // Set the timeout for serial communication to 1000 milliseconds (1 second)
                   max_buffer_size(512) {}  // Set the maximum buffer size for serial communication to 512 bytes

        std::string device;  // Declare a string variable to store the name of the serial port device
        int baudrate;  // Declare an integer variable to store the baud rate for serial communication
        int timeout_ms;  // Declare an integer variable to store the timeout value in milliseconds for serial communication operations
        size_t max_buffer_size;  // Declare a variable to store the maximum buffer size (in bytes) for serial communication
    };

    // Constructor/destructor
    explicit SerialPort(const Config &config = Config());
    ~SerialPort();  // Destructor for the SerialPort class, used to clean up and close the serial port when the object is destroyed

    // Basic function
    bool open();  // Open the serial port for communication
    void close();  // Close the serial port and release any resources associated with it
    bool isOpen() const;  // Check if the serial port is currently open and return true if it is, false otherwise

    // Data sending and receiving
    ssize_t send(const std::string &data);  // Send a string of data through the serial port and return the number of bytes sent
    ssize_t send(const uint8_t *data, size_t length);  // Send a specified number of bytes (length) from the provided data buffer through the serial port and return the number of bytes sent
    std::string receive();  // Receive data from the serial port and return it as a string
    ssize_t receive(uint8_t *buffer, size_t max_length);  // Receive data from the serial port, storing it in the provided buffer, and return the number of bytes received

    // Advanced function
    void flushInput();  // Clear any unread data from the input buffer.
    void flushOutput();  // Clear any data waiting to be sent in the output buffer.
    size_t available() const;  // Return the number of bytes available for reading in the input buffer

    // Configuration management
    void reconfigure(const Config &new_config);  // Update the serial port configuration with new settings from the provided Config object
    Config getConfiguration() const;  // Return the current serial port configuration

private:
    Config config_;  // Store the current serial port configuration.
    int fd_ = -1;  // File descriptor for the serial port (initialized to -1, meaning the port is not open).
    bool is_open_ = false;  // Boolean flag indicating whether the serial port is currently open.

    // Disable copy constructs and assignments
    SerialPort(const SerialPort &) = delete;  // Delete the copy constructor to prevent creating a copy of the SerialPort object
    SerialPort &operator=(const SerialPort &) = delete;  // Delete the copy assignment operator to prevent assigning one SerialPort object to another
};

#endif // SERIAL_PORT_H
