#include <iostream>
#include <mutex>
#include <stdexcept>
#include <cstring>
#include <functional>
#include "MQTTClient.h"

class MQTTClientWrapper {
public:
    // 删除拷贝构造函数和赋值运算符
    MQTTClientWrapper(const MQTTClientWrapper&) = delete;
    MQTTClientWrapper& operator=(const MQTTClientWrapper&) = delete;

    // 获取单例实例
    static MQTTClientWrapper& getInstance() {
        static MQTTClientWrapper instance;
        return instance;
    }

    // 初始化MQTT客户端
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

        // 设置回调函数
        MQTTClient_setCallbacks(client_, this, connectionLostCallback, messageArrivedCallback, deliveryCompleteCallback);

        initialized_ = true;
    }

    // 连接到MQTT代理
    void connect(const std::string& username = "", const std::string& password = "") {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!initialized_) {
            throw std::runtime_error("MQTT client not initialized");
        }

        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
        MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
        conn_opts.keepAliveInterval = 60;
        conn_opts.cleansession = 1;
        conn_opts.ssl = &ssl_opts;  // 传递合法左值地址

        if (!username.empty()) {
            conn_opts.username = username.c_str();
            conn_opts.password = password.c_str();
        }

        int rc = MQTTClient_connect(client_, &conn_opts);
        if (rc != MQTTCLIENT_SUCCESS) {
            throw std::runtime_error("Failed to connect: " + std::to_string(rc));
        }
    }

    // 发布消息
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

        // 等待消息确认（可选）
        rc = MQTTClient_waitForCompletion(client_, token, 1000L);
        if (rc != MQTTCLIENT_SUCCESS) {
            throw std::runtime_error("Message not acknowledged: " + std::to_string(rc));
        }
    }

    // 订阅主题
    void subscribe(const std::string& topic, int qos = 0) {
        std::lock_guard<std::mutex> lock(mutex_);

        int rc = MQTTClient_subscribe(client_, topic.c_str(), qos);
        if (rc != MQTTCLIENT_SUCCESS) {
            throw std::runtime_error("Subscribe failed: " + std::to_string(rc));
        }
    }

    // 断开连接
    void disconnect() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (MQTTClient_isConnected(client_)) {
            MQTTClient_disconnect(client_, 1000);
        }
    }

    // 设置消息到达回调
    void setMessageHandler(std::function<void(const std::string& topic, const std::string& message)> handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        messageHandler_ = std::move(handler);
    }

private:
    MQTTClient client_;
    bool initialized_ = false;
    std::mutex mutex_;
    std::function<void(const std::string&, const std::string&)> messageHandler_;

    // 清理资源
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
    // 私有构造函数
    MQTTClientWrapper() = default;

    // 连接丢失回调（静态成员函数）
    static void connectionLostCallback(void* context, char* cause) {
        auto* instance = static_cast<MQTTClientWrapper*>(context);
        std::cerr << "Connection lost! Cause: "
            << (cause ? cause : "unknown") << std::endl;
        // 可以在这里添加重连逻辑
    }

    // 消息到达回调（静态成员函数）
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

    // 消息送达回调（静态成员函数）
    static void deliveryCompleteCallback(void* context, MQTTClient_deliveryToken token) {
        auto* instance = static_cast<MQTTClientWrapper*>(context);
        std::cout << "Message with token " << token << " delivered" << std::endl;
    }
};

/* 使用示例：
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

        // 保持连接
        std::this_thread::sleep_for(std::chrono::seconds(5));
        mqtt.disconnect();
    } catch (const std::exception& e) {
        std::cerr << "MQTT Error: " << e.what() << std::endl;
    }
    return 0;
}
*/

