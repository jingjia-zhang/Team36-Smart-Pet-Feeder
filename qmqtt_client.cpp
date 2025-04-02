#include "qmqtt_client.h"

static std::string mSubscribeToptic = "/Pet/post";
static std::string mPublishTopic = "/Pet/update";
static bool isConn = false;
qmqtt_client* qmqtt_client::QMqttClient = nullptr;

qmqtt_client::qmqtt_client(/* args */)
{
    mMqttClient.onConnect = onConnect;
    mMqttClient.onClose = onClose;
    // mMqttClient.onMessage = onMessage;
    mMqttClient.setID("Pi5Pet");
    mMqttClient.setAuth(userName.c_str(), passWorld.c_str());
    reconn_setting_t reconn;
    reconn_setting_init(&reconn);
    reconn.min_delay = 1000;
    reconn.max_delay = 10000;
    reconn.delay_policy = 2;
    mMqttClient.setReconnect(&reconn);
    mMqttClient.setPingInterval(10);
    QMqttClient = this;
}

qmqtt_client::~qmqtt_client()
{
    mMqttClient.disconnect();
    mMqttClient.stop();
    // mRunThread.join();
}

qmqtt_client& qmqtt_client::getInstance()
{
    static qmqtt_client mqttClient;
    return mqttClient;
}

void qmqtt_client::setOnMsg(CallbackFunction callback)
{
    mMqttClient.onMessage = callback;
}

void qmqtt_client::run()
{
    mRunThread = std::thread([&]
        {
            mMqttClient.connect(hostUrl.c_str(), hostPort, 1);
            mMqttClient.run(); });
    // mMqttClient.connect(hostUrl.c_str(), hostPort, 0);
    // mMqttClient.run();
}

bool qmqtt_client::getConnStatus()
{
    return isConn;
}

void qmqtt_client::onConnect(hv::MqttClient* cli)
{
    std::cout << "mqttclient onConnect" << std::endl;
    cli->subscribe(mSubscribeToptic.c_str());
    isConn = true;
}
void qmqtt_client::onClose(hv::MqttClient* cli)
{
    isConn = false;
    std::cout << "mqttclient onClose" << std::endl;
}
// void qmqtt_client::onMessage(hv::MqttClient *cli, mqtt_message_t *msg)
// {
// }
void qmqtt_client::pulishMsg(const std::string& msg)
{
    QMqttClient->mMqttClient.publish(mPublishTopic, msg, 0, 0);
}