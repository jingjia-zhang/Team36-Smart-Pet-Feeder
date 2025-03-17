#ifndef _QMQTT_CLIENT_H  // Check if the header file _QMQTT_CLIENT_H is not already defined to prevent multiple inclusions
#define _QMQTT_CLIENT_H  // Define the _QMQTT_CLIENT_H macro to indicate that the header file has been included, preventing multiple inclusions
#include "hv/mqtt_client.h"  // Include the MQTT client header file from the "hv" library for MQTT communication functionality
#include <thread>  // Include the thread library to enable thread management and multithreading capabilities
#include <iostream>  // Include the iostream library for input and output stream operations
using namespace hv;  // Use the hv namespace to avoid prefixing hv:: for each reference to entities within the hv library
typedef void (*CallbackFunction)(hv::MqttClient *cli, mqtt_message_t *msg);  // Define a function pointer type named CallbackFunction for a callback function that takes an MqttClient pointer and a mqtt_message_t pointer as arguments
class qmqtt_client  // Define a class named qmqtt_client for implementing MQTT client functionality, handling communication with MQTT brokers
{
private:
    hv::MqttClient mMqttClient;  // Declare an instance of the MqttClient class from the hv namespace to manage MQTT communication
    int mHostPort;  // Declare an integer variable to store the port number for the MQTT broker connection
    const std::string hostUrl = " ";  // Declare a constant string to store the URL of the MQTT broker
    const int hostPort =  ;  // Declare a constant integer to store the default port number for the MQTT broker connection
    const std::string userName = " ";  // Declare a constant string to store the default username for MQTT broker authentication
    const std::string passWord = " ";  // Declare a constant string to store the default password for MQTT broker authentication
    std::thread mRunThread;  // Declare a thread object (mRunThread) to handle running MQTT client operations concurrently
    static qmqtt_client *QMqttClient;  // Declare a static pointer to a qmqtt_client instance, used for implementing the Singleton pattern

public:
    qmqtt_client(/* args */);  // Constructor for the qmqtt_client class, used to initialize the MQTT client instance
    ~qmqtt_client();  // Destructor for the qmqtt_client class, used to clean up resources and stop the MQTT client operations

    static qmqtt_client &getInstance();  // Static method that returns a reference to the singleton instance of the qmqtt_client class
    void setOnMsg(CallbackFunction callback);  // Set a callback function to handle incoming MQTT messages
    void run();  // Start the MQTT client operations in a separate thread or main loop
    bool getConnStatus();  // Return the current connection status of the MQTT client (true if connected, false otherwise)
    static void onConnect(hv::MqttClient *cli);  // Static method called when the MQTT client successfully connects to the broker
    static void onClose(hv::MqttClient *cli);  // Static method called when the MQTT client disconnects from the broker
    // static void onMessage(hv::MqttClient *cli, mqtt_message_t *msg);
    static void pulishMsg(const std::string &msg);
};

#endif