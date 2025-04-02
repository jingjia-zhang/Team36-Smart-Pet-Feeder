#ifndef _QMQTT_CLIENT_H
#define _QMQTT_CLIENT_H
#include "hv/mqtt_client.h"
#include <thread>
#include <iostream>
using namespace hv;
typedef void (*CallbackFunction)(hv::MqttClient* cli, mqtt_message_t* msg);
class qmqtt_client
{
private:
    hv::MqttClient mMqttClient;
    int mHostPort;
    const std::string hostUrl = "120.55.89.49";
    const int hostPort = 8883;
    const std::string userName = "test";
    const std::string passWorld = "test1234";
    std::thread mRunThread;
    static qmqtt_client* QMqttClient;

public:
    qmqtt_client(/* args */);
    ~qmqtt_client();

    static qmqtt_client& getInstance();
    void setOnMsg(CallbackFunction callback);
    void run();
    bool getConnStatus();
    static void onConnect(hv::MqttClient* cli);
    static void onClose(hv::MqttClient* cli);
    // static void onMessage(hv::MqttClient *cli, mqtt_message_t *msg);
    static void pulishMsg(const std::string& msg);
};

#endif