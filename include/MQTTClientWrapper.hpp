#include <iostream>
#include <mutex>
#include <stdexcept>
#include <cstring>
#include <functional>
#include "MQTTClient.h"

class MQTTClientWrapper {
public:
    // Removing Copy Constructors and Assignment Operators
    MQTTClientWrapper(const MQTTClientWrapper&) = delete;
    MQTTClientWrapper& operator=(const MQTTClientWrapper&) = delete;

    // Getting a Singleton Instance
    static MQTTClientWrapper& getInstance() {
        static MQTTClientWrapper instance;
        return instance;
    }

    // Initializing the MQTT Client
    void initialize(const std::string& address, const std::string& clientId) {
        std::lock_guard<std::mutex> lock(mutex_);

        if (initialized_) {
            throw std::runtime_error("MQTT client already initialized");
        }

        int rc = MQTTClient_create(&client_, address.c_str(), clientId.c_str(),
            MQTTCLIENT_PERSISTENCE_NONE, nullptr);
        if (rc != MQTTCLIENT_SUCCESS) {
            throw std::runtime_error("Failed to create MQTT client: " + std::to_string(rc));
        }

        // Setting Callback Functions
        MQTTClient_setCallbacks(client_, this, connectionLostCallback, messageArrivedCallback, deliveryCompleteCallback);

        initialized_ = true;
    }

    // Connecting to the MQTT Agent
    void connect(const std::string& username = "", const std::string& password = "") {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!initialized_) {
            throw std::runtime_error("MQTT client not initialized");
        }

        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
        MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
        conn_opts.keepAliveInterval = 60;
        conn_opts.cleansession = 1;
        conn_opts.ssl = &ssl_opts;  // Pass legal left address

        if (!username.empty()) {
            conn_opts.username = username.c_str();
            conn_opts.password = password.c_str();
        }

        int rc = MQTTClient_connect(client_, &conn_opts);
        if (rc != MQTTCLIENT_SUCCESS) {
            throw std::runtime_error("Failed to connect: " + std::to_string(rc));
        }
    }

    // Post the Message
    void publish(const std::string& topic, const std::string& payload, int qos = 0) {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!MQTTClient_isConnected(client_)) {
            throw std::runtime_error("Not connected to broker");
        }

        MQTTClient_message pubmsg = MQTTClient_message_initializer;
        pubmsg.payload = const_cast<char*>(payload.c_str());
        pubmsg.payloadlen = static_cast<int>(payload.length());
        pubmsg.qos = qos;
        pubmsg.retained = 0;

        MQTTClient_deliveryToken token;
        int rc = MQTTClient_publishMessage(client_, topic.c_str(), &pubmsg, &token);
        if (rc != MQTTCLIENT_SUCCESS) {
            throw std::runtime_error("Publish failed: " + std::to_string(rc));
        }

        // Waiting for Confirmation.
        rc = MQTTClient_waitForCompletion(client_, token, 1000L);
        if (rc != MQTTCLIENT_SUCCESS) {
            throw std::runtime_error("Message not acknowledged: " + std::to_string(rc));
        }
    }

    // Subscribe to Threads
    void subscribe(const std::string& topic, int qos = 0) {
        std::lock_guard<std::mutex> lock(mutex_);

        int rc = MQTTClient_subscribe(client_, topic.c_str(), qos);
        if (rc != MQTTCLIENT_SUCCESS) {
            throw std::runtime_error("Subscribe failed: " + std::to_string(rc));
        }
    }

    // Disconnect
    void disconnect() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (MQTTClient_isConnected(client_)) {
            MQTTClient_disconnect(client_, 1000);
        }
    }

    // Setting Up Message Arrival Callbacks
    void setMessageHandler(std::function<void(const std::string& topic, const std::string& message)> handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        messageHandler_ = std::move(handler);
    }

private:
    MQTTClient client_;
    bool initialized_ = false;
    std::mutex mutex_;
    std::function<void(const std::string&, const std::string&)> messageHandler_;

    // Liquidation of Resources
    ~MQTTClientWrapper() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (client_) {
            if (MQTTClient_isConnected(client_)) {
                MQTTClient_disconnect(client_, 1000);
            }
            MQTTClient_destroy(&client_);
        }
    }

private:
    // Private Constructor
    MQTTClientWrapper() = default;

    // Connection Loss Callback (Static Member Function)
    static void connectionLostCallback(void* context, char* cause) {
        auto* instance = static_cast<MQTTClientWrapper*>(context);
        std::cerr << "Connection lost! Cause: "
            << (cause ? cause : "unknown") << std::endl;
    }

    // Message Arrival Callback (Static Member Function)
    static int messageArrivedCallback(void* context, char* topicName, int topicLen,
        MQTTClient_message* message) {
        auto* instance = static_cast<MQTTClientWrapper*>(context);

        std::string topic(topicName);
        std::string payload(static_cast<char*>(message->payload), message->payloadlen);

        {
            std::lock_guard<std::mutex> lock(instance->mutex_);
            if (instance->messageHandler_) {
                instance->messageHandler_(topic, payload);
            }
        }

        MQTTClient_free(topicName);
        MQTTClient_freeMessage(&message);
        return 1;
    }

    // Message Delivery Callbacks (Static Member Functions)
    static void deliveryCompleteCallback(void* context, MQTTClient_deliveryToken token) {
        auto* instance = static_cast<MQTTClientWrapper*>(context);
        std::cout << "Message with token " << token << " delivered" << std::endl;
    }
};

/* Example of Use:
int main() {
    try {
        auto& mqtt = MQTTClientWrapper::getInstance();
        mqtt.initialize("tcp://localhost:1883", "client_id");
        mqtt.connect();

        mqtt.setMessageHandler([](const std::string& topic, const std::string& msg) {
            std::cout << "Received message on [" << topic << "]: " << msg << std::endl;
        });

        mqtt.subscribe("test/topic");
        mqtt.publish("test/topic", "Hello MQTT");

        // Stay Connected
        std::this_thread::sleep_for(std::chrono::seconds(5));
        mqtt.disconnect();
    } catch (const std::exception& e) {
        std::cerr << "MQTT Error: " << e.what() << std::endl;
    }
    return 0;
}
*/

