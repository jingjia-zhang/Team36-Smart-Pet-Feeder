#include <stdio.h>   // Standard input-output library
#include <iostream>  // C++ standard input-output library
#include <wiringPi.h> // WiringPi library for Raspberry Pi GPIO
#include "qmqtt_client.h" // MQTT client library
#include "hx711.hpp" // HX711 sensor library
#include <sstream> // String stream handling

// Define GPIO pins for hardware connections
#define INFRA_RED_PIN 3    // Infrared sensor pin
#define WATER_PUMP_PIN 25  // Water pump control pin
#define SERVO_PIN 1        // Servo motor control pin
#define HX711_DT 21        // HX711 data pin
#define HX711_SCK 22       // HX711 clock pin

// Define global variables
int infrared_status = 0; // Infrared sensor status
int led_status = LOW, water_pump_status = LOW; // LED and water pump status
int mode = 1; // Mode selection
const char* pulish_msg = "FD %.2f %.2f %d %d %d %d %d %d FF"; // MQTT message format

// GPIO initialization function
void gpioInit(void)
{
    if (wiringPiSetup() < 0) // Initialize wiringPi library
        return;
    pinMode(INFRA_RED_PIN, INPUT); // Set infrared sensor pin as input
    pinMode(WATER_PUMP_PIN, OUTPUT); // Set water pump pin as output
    pinMode(SERVO_PIN, PWM_OUTPUT); // Set servo pin as PWM output
    pwmSetMode(PWM_MODE_MS); // Set PWM mode to mark-space mode
    pwmSetRange(2000); // Set PWM range
    pwmSetClock(192); // Set PWM clock
    pullUpDnControl(INFRA_RED_PIN, PUD_DOWN); // Enable pull-down resistor for infrared sensor pin
    pullUpDnControl(WATER_PUMP_PIN, PUD_DOWN); // Enable pull-down resistor for water pump pin
}

// Function to set servo motor angle
void setServoAngle(int angle)
{
    // Calculate servo PWM pulse width (angle * 2000μs / 180°) + 500μs
    int pulse_width = static_cast<int>((angle * 2000.0 / 180.0) + 500);
    pwmWrite(SERVO_PIN, pulse_width); // Send PWM signal
}

// Turn on the water pump
void openWaterPump(void)
{
    water_pump_status = HIGH;
    digitalWrite(WATER_PUMP_PIN, HIGH);
}

// Turn off the water pump
void closeWaterPump(void)
{
    water_pump_status = LOW;
    digitalWrite(WATER_PUMP_PIN, LOW);
}

// Function to split a string
std::vector<std::string> splitString(const std::string& s)
{
    std::vector<std::string> tokens;
    std::stringstream stream(s);
    std::string token;
    while (std::getline(stream, token, ' ')) // Split the string using space as a delimiter
    {
        tokens.push_back(token);
    }
    return tokens;
}

// MQTT message handling callback function
void mqtt_msg(hv::MqttClient* cli, mqtt_message_t* msg)
{
    std::cout << "mqtttopic:" << msg->topic << ",mqttpayload:" << msg->payload << std::endl;
    std::cout << "topic_len:" << msg->topic_len << ",payload_len:" << msg->payload_len << std::endl;
    
    std::string strMsg(msg->payload, msg->payload_len); // Extract message content
    std::vector<std::string> splits = splitString(strMsg); // Parse the message
    
    for (size_t i = 0; i < splits.size(); i++)
    {
        std::cout << splits[i] << " ";
    }
    std::cout << std::endl;
    
    if (splits[0].compare("FD") == 0 && splits[3].compare("FF") == 0)
    {
        // Successful parsing, execute corresponding operations here
    }
}

// Main function
int main(void)
{
    char value;
    qmqtt_client& client = qmqtt_client::getInstance(); // Get MQTT client instance
    client.setOnMsg(mqtt_msg); // Bind MQTT message handler function
    client.run(); // Start MQTT client
    
    gpioInit(); // Initialize GPIO
    printf("gpio init success!\r\n");
    
    setServoAngle(0); // Set servo angle to 0°
    delay(2000); // Wait for 2 seconds
    setServoAngle(90); // Set servo angle to 90°
    delay(2000);
    setServoAngle(0); // Set servo angle to 0°
    
    while (1)
    {
        infrared_status = digitalRead(INFRA_RED_PIN); // Read infrared sensor status
        if (infrared_status == HIGH)
        {
            printf("infrared Open\n");
        }
        else
        {
            printf("infrared Close\n");
        }
        
        pwmWrite(SERVO_PIN, 150); // Adjust servo motor angle
        delay(1000);
        pwmWrite(SERVO_PIN, 200);
    }
}
