#include <wiringPi.h>  // wiringPi.h: For controlling GPIO pins on Raspberry Pi.
#include <iostream>  // iostream: For input/output stream operations.
#include <fstream>  // fstream: For file handling (reading/writing calibration data).
#include <chrono>  // chrono: For time-related functions, such as sleep
#include <thread>  // thread: For thread management (used with std::this_thread::sleep_for)
#include <mutex>  // mutex: For thread synchronization to ensure safe access to shared resources
#include <queue>  // queue: For using priority queues to filter sensor data
#include <cmath>  // cmath: For mathematical operations (e.g., to handle scaling)

class HX711
{
public:
    // Pin configuration (using WiringPi pin numbers)
    HX711(int data_pin, int clock_pin, int gain = 128)  // Initialize HX711 module with specified data pin, clock pin, and gain setting
        : DATA_PIN(data_pin), CLOCK_PIN(clock_pin), GAIN(gain)  // Set class member variables for data pin, clock pin, and gain during object initialization
    {
        wiringPiSetup();  // Initialize WiringPi library.
        pinMode(DATA_PIN, INPUT);  // Set the DATA_PIN as an input to read data from the HX711 sensor.
        pinMode(CLOCK_PIN, OUTPUT);  // Set the CLOCK_PIN as an output to send clock signals to the HX711 sensor

        // Load calibration parameter
        load_calibration();
    }

    // Basic reading method
    long read_raw()  // Read raw data from the HX711 sensor
    {
        std::lock_guard<std::mutex> lock(io_mutex);  // Lock the mutex to ensure thread-safe access to the I/O operations

        while (!is_ready())  // Wait in a loop until the sensor is ready to send data
            ;

        unsigned long data = 0;  // Initialize the data variable to zero to store the raw sensor readings
        digitalWrite(CLOCK_PIN, LOW);  // Set the clock pin to LOW to prepare for triggering a new data read from the sensor

        // Read 24-bit data
        for (int i = 0; i < 24; ++i)
        {
            digitalWrite(CLOCK_PIN, HIGH);  // Set the clock pin to HIGH to signal the HX711 sensor to send the next bit of data
            delayMicroseconds(1);  // Pause for 1 microsecond to allow the clock signal to be processed by the HX711 sensor
            data = (data << 1) | digitalRead(DATA_PIN);  // Shift the previously read data left by one bit and add the current bit read from the DATA_PIN
            digitalWrite(CLOCK_PIN, LOW);  // Set the clock pin back to LOW to prepare for the next data bit read
            delayMicroseconds(1);  // Pause for 1 microsecond to stabilize the data line before the next clock pulse
        }

        // Set the gain and next channel
        for (int i = 0; i < GAIN / 64; ++i)
        {
            digitalWrite(CLOCK_PIN, HIGH);  // Set the clock pin to HIGH to issue a clock pulse for gain setting
            delayMicroseconds(1);  // Wait for 1 microsecond to ensure the clock pulse is registered by the HX711 sensor
            digitalWrite(CLOCK_PIN, LOW);  // Set the clock pin back to LOW after the clock pulse to complete the gain setting cycle
            delayMicroseconds(1);  // Pause for 1 microsecond to stabilize the clock line after setting it to LOW.
        }

        // Symbolic extension
        if (data & 0x800000)
        {
            data |= 0xFF000000;  // Perform sign extension by setting the upper 8 bits if the data is negative to maintain the correct value in a 32-bit signed integer
        }

        return static_cast<long>(data);  // Convert the data from unsigned long to long and return it as the function result
    }

    // Read with filter (median + mean filter)
    double read_filtered(int samples = 5)
    {
        std::priority_queue<long> sample_queue;  // Declare a priority queue to store sensor readings for filtering, with the largest value at the top
        long sum = 0;  // Initialize a variable to store the sum of filtered sensor readings

        for (int i = 0; i < samples + 2; ++i)  // Loop to collect sensor readings, including two extra samples to later remove the highest and lowest values
        {
            sample_queue.push(read_raw());  // Read raw data from the HX711 sensor and store it in the priority queue for filtering
        }

        // Remove the maximum and minimum values
        sample_queue.pop();  // Remove the top element from the priority queue, which is the highest value, to reduce noise
        std::priority_queue<long> temp;  // Declare a temporary priority queue to store the remaining filtered sensor readings
        while (sample_queue.size() > 1)  // Loop through the priority queue, keeping all but the last remaining element to remove the lowest value
        {
            temp.push(sample_queue.top());  // Move the current top element from the original priority queue to the temporary queue for further processing
            sample_queue.pop();  // Remove the top element from the original priority queue after transferring it to the temporary queue
        }
        sample_queue.pop();  // Remove the last remaining element from the priority queue, which is the lowest value, to reduce noise

        // Calculate the average of the median values
        sum = 0;  // Reset the sum variable to accumulate the filtered sensor readings
        int count = 0;  // Initialize a counter to track the number of filtered sensor readings
        while (!temp.empty())  // Loop through the temporary priority queue until all elements are processed
        {
            sum += temp.top();  // Add the top element of the temporary priority queue to the sum for calculating the average
            temp.pop();  // Remove the top element from the temporary priority queue after adding it to the sum
            count++;  // Increment the counter to keep track of the number of filtered sensor readings
        }

        double result = (sum / static_cast<double>(count) - offset) * scale;  // Compute the final filtered weight by averaging the sum, applying the offset correction, and scaling the value
        return result;  // Return the final computed weight value after filtering and calibration
    }

    // Calibration process (required known weight)
    void calibrate(double known_weight)
    {
        std::cout << "开始校准，请移除所有重量..." << std::endl;  // Display a message prompting the user to remove all weight before calibration
        std::this_thread::sleep_for(std::chrono::seconds(2));  // Pause execution for 2 seconds to allow the user to remove all weight before calibration
        tare(50);  // Perform the tare operation by taking 50 samples to determine the zero offset

        std::cout << "请放置已知重量：" << known_weight << "g" << std::endl;  // Prompt the user to place a known weight on the scale for calibration
        std::this_thread::sleep_for(std::chrono::seconds(3));  // Pause execution for 3 seconds to allow the user to place the known weight on the scale

        long raw = read_raw();  // Read the raw sensor value from the HX711 for calibration
        scale = known_weight / (raw - offset);  // Calculate the scale factor by dividing the known weight by the measured raw value adjusted for offset

        save_calibration();  // Save the calibration data (offset and scale factor) to a file for future use
        std::cout << "校准完成！比例系数：" << scale << std::endl;  // Display a message indicating that calibration is complete and show the calculated scale factor
    }

    // Peeling operation
    void tare(int samples = 10)
    {
        double sum = 0;  // Initialize a variable to store the sum of sensor readings for averaging
        for (int i = 0; i < samples; ++i)  // Loop to collect multiple sensor readings for averaging
        {
            sum += read_raw();  // Read raw data from the HX711 sensor and add it to the sum for averaging
            std::this_thread::sleep_for(std::chrono::milliseconds(10));  // Pause execution for 10 milliseconds between sensor readings to ensure stability
        }
        offset = sum / samples;  // Calculate the offset by averaging the collected sensor readings
    }

private:
    const int DATA_PIN;  // Private constant variables to store the data pin for the HX711 sensor
    const int CLOCK_PIN;  // Private constant variables to store the clock pin for the HX711 sensor
    const int GAIN;  // Private constant variables to store the gain setting for the HX711 sensor

    long offset = 0;  // Initialize the offset variable to store the tare (zero) value for calibration
    double scale = 1.0; // // Initialize the scale factor to 1.0 for converting raw sensor data into weight measurements

    std::mutex io_mutex;  // Define a mutex to ensure thread-safe access to the HX711 sensor

    bool is_ready()  // Check if the HX711 sensor is ready to send data
    {
        return digitalRead(DATA_PIN) == LOW;  // Return true if the HX711 sensor is ready, indicated by a LOW signal on the data pin
    }

    void save_calibration()  // Save the calibration data (offset and scale) to a file for future use
    {
        std::ofstream file("hx711.cal");  // Open a file named "hx711.cal" for writing calibration data
        if (file)  // If the file is successfully opened
        {
            file << offset << " " << scale;  // Write the offset and scale values to the file, separated by a space
        }
    }

    void load_calibration()  // Load the calibration data (offset and scale) from a file to apply previously saved calibration settings
    {
        std::ifstream file("hx711.cal");  // Open the file "hx711.cal" for reading calibration data
        if (file)  // If the file is successfully opened
        {
            file >> offset >> scale;// Read the offset and scale values from the file and store them in the respective variables
        }
    }
};

// Use example
// int main() {
//     // Use the WiringPi pin number
//     HX711 load_cell(21, 22);  // DT=21(BCM5), SCK=22(BCM6)

//     // First use requires calibration
//     // load_cell.calibrate(1000.0);  // Calibration using 1kg weights

//     while (true) {
//         double weight = load_cell.read_filtered();
//         std::cout << "当前重量: " << weight << " g" << std::endl;
//         std::this_thread::sleep_for(std::chrono::milliseconds(500));
//     }

//     return 0;
// }
