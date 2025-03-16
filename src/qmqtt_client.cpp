#include "qmqtt_client.h"  // Include the header file for MQTT client functionality

// static std::string mSubscribeToptic1 = "/esp32cam/posture/camera"; // (Commented) A topic for subscribing, not used in the code
static std::string mSubscribeToptic2 = "/Pi4BSmartHome/updateInfo"; // MQTT topic to subscribe to for updates from Pi4BSmartHome
static std::string mPublishTopic = "/Pi4BSmartHome/postSetting"; // MQTT topic for publishing messages
static bool isConn = false; // Flag to track the MQTT connection status
qmqtt_client *qmqtt_client::QMqttClient = nullptr; // Static pointer to the instance of the MQTT client

// Constructor for the mqtt_client class
qmqtt_client::qmqtt_client(/* args */)
{
    // Set callbacks for connection events
    mMqttClient.onConnect = onConnect; // Callback function when connection is established
    mMqttClient.onClose = onClose; // Callback function when connection is closed
    
    // mMqttClient.onMessage = onMessage; // (Commented) Callback function for when a message is received, not used

    // Set client ID for the MQTT connection
    mMqttClient.setID("Pi4BSmartHome");
    
    // Set MQTT authentication credentials
    mMqttClient.setAuth(userName.c_str(), passWorld.c_str()); 

    // Set up the MQTT reconnect settings
    reconn_setting_t reconn; // Declare reconnect settings structure
    reconn_setting_init(&reconn); // Initialize reconnect settings with default values
    reconn.min_delay = 1000; // Minimum reconnect delay (in ms)
    reconn.max_delay = 10000; // Maximum reconnect delay (in ms)
    reconn.delay_policy = 2; // Reconnect delay policy
    mMqttClient.setReconnect(&reconn); // Apply the reconnect settings to the MQTT client

    // Set the interval for sending ping messages
    mMqttClient.setPingInterval(10); // Interval for sending ping messages to keep connection alive (in seconds)

    // Assign the instance to the static pointer
    QMqttClient = this;
}

// Destructor for the mqtt_client class
qmqtt_client::~qmqtt_client()
{
    mMqttClient.disconnect(); // Disconnect the MQTT client
    mMqttClient.stop(); // Stop the MQTT client
    // mRunThread.join(); // (Commented) Wait for the running thread to finish, not used
}

// Method to get the singleton instance of the MQTT client
qmqtt_client &qmqtt_client::getInstance()
{
    static qmqtt_client mqttClient; // Create a static instance of the MQTT client
    return mqttClient; // Return the singleton instance
}

// Set the callback function for handling received messages
void qmqtt_client::setOnMsg(CallbackFunction callback)
{
    mMqttClient.onMessage = callback; // Set the provided callback for message reception
}

// Method to start the MQTT client and connect to the broker
void qmqtt_client::run()
{
    // Run the MQTT client connection and loop in a separate thread
    mRunThread = std::thread([&]
                             {
        mMqttClient.connect(hostUrl.c_str(), hostPort, 0); // Connect to the MQTT broker using the provided host and port
        mMqttClient.run(); // Start the client loop to handle MQTT messages
    });
    // mMqttClient.connect(hostUrl.c_str(), hostPort, 0); // (Commented) Alternative method to connect, not using a separate thread
    // mMqttClient.run(); // (Commented) Alternative to start the MQTT client loop, not using a separate thread
}

// Method to check the connection status
bool qmqtt_client::getConnStatus()
{
    return isConn; // Return the connection status flag
}

// Callback function for when the MQTT client successfully connects to the broker
void qmqtt_client::onConnect(hv::MqttClient *cli)
{
    std::cout << "mqttclient onConnect" << std::endl; // Log the connection event

    // cli->subscribe(mSubscribeToptic1.c_str()); // (Commented) Subscribe to a topic, but not used in the code
    cli->subscribe(mSubscribeToptic2.c_str()); // Subscribe to the mSubscribeToptic2 topic

    isConn = true; // Set the connection status to true
}

// Callback function for when the MQTT client disconnects or closes the connection
void qmqtt_client::onClose(hv::MqttClient *cli)
{
    isConn = false; // Set the connection status to false
    std::cout << "mqttclient onClose" << std::endl; // Log the disconnection event
}

// (Commented) Callback function for handling incoming messages from the broker
// void qmqtt_client::onMessage(hv::MqttClient *cli, mqtt_message_t *msg)
// {
// }

// Method to publish a message to the specified MQTT topic
void qmqtt_client::pulishMsg(const std::string &msg)
{
    QMqttClient->mMqttClient.publish(mPublishTopic, msg, 0, 0); // Publish the message to the mPublishTopic with QoS 0 and retain flag 0
}
