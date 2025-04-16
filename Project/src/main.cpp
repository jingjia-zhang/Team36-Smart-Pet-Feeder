#if 1
#include <stdio.h>
#include <iostream>
#include <hx711/common.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "MQTTClientWrapper.hpp"
#include "SerialPort.h"
#include "json.hpp"
#include <sstream>
#include <chrono>
#include <iostream>

#include <thread>
#define INFRA_RED_PIN 3      // IR sensor GPIO pin
#define WATER_PUMP_PIN 25    // Water pump control pin
#define SERVO_PIN 0          // Servo motor pin

int infrared_status = 0;
int servo_status = 0, water_pump_status = LOW;
int mode = 1;// 0 = auto, 1 = remote
float weights = 0;
float weights_threshold = 10;// threshold weight to decide food/water
int servo_flag = 0;

// Singleton serial port instance
SerialPort& serial_port = SerialPort::getInstance();

// MQTT related config
static std::string mSubscribeToptic = "/Pet/post";
static std::string mPublishTopic = "/Pet/update";
static std::string mUserName = "test";
static std::string mPassWord = "test1234";
static std::string mServerUrl = "mqtts://t09d1d6f.ala.cn-hangzhou.emqxsl.cn:8883";
static std::string mClientId = "Pi5Pet";

// Initialize GPIOs
void gpioInit(void)
{
    if (wiringPiSetup() < 0)
        return;
    pinMode(INFRA_RED_PIN, INPUT);
    pinMode(WATER_PUMP_PIN, OUTPUT);
    pinMode(SERVO_PIN, OUTPUT);
    softPwmCreate(SERVO_PIN, 0, 200);// PWM period = 200
    pullUpDnControl(INFRA_RED_PIN, PUD_DOWN);
    pullUpDnControl(WATER_PUMP_PIN, PUD_DOWN);
}

// Set servo motor angle (0 or 1 = different PWM duty cycles)
void setServoAngle(int angle)
{
    softPwmWrite(SERVO_PIN, 0);
    delay(1000);
    switch (angle)
    {
    case 0:
    {
        servo_status = 0;
        softPwmWrite(SERVO_PIN, 15);
        servo_flag = 0;
        break;
    }
    case 1:
    {
        servo_status = 1;
        softPwmWrite(SERVO_PIN, 25);
        servo_flag = 0;
        break;
    }
    }
}

void openWaterPump(void)
{
    water_pump_status = HIGH;
    digitalWrite(WATER_PUMP_PIN, HIGH);
}

void closeWaterPump(void)
{
    water_pump_status = LOW;
    digitalWrite(WATER_PUMP_PIN, LOW);
}

// Utility: split string by space
std::vector<std::string> splitString(const std::string& s)
{
    std::vector<std::string> tokens;
    std::stringstream stream(s);
    std::string token;
    while (std::getline(stream, token, ' '))
    { // 使用空格作为分隔符
        tokens.push_back(token);
    }
    return tokens;
}



int main(void)
{
    char value;
    bool isExit = false;
    bool isMqttConnStatus = false;

    gpioInit();

    // Init weight sensor (DOUT, SCK, gain, offset, rate)
    HX711::AdvancedHX711 hx(5, 6, 419, 233775, HX711::Rate::HZ_80);


    if (!serial_port.isOpen())
    {
        serial_port.open();
    }

    // Serial thread: reads commands and updates states
    std::thread threadSeral([&]() {
        while (!isExit)
        {
            uint8_t array[5] = { 0 };
            if (serial_port.receive(array, 5) > 0)
            {
                for (size_t i = 0; i < 5; i++)
                {
                    printf("%02X", array[i]);
                }
                printf("\r\n");
                if (array[0] == 0xFD && array[2] == 0xFF)
                {
                    switch (array[1])
                    {
                    case 1: { if (mode == 1) { openWaterPump(); }break; }
                    case 2: { if (mode == 1) { setServoAngle(1); }break; }
                    case 5: { if (mode == 1) { closeWaterPump(); }break; }
                    case 6: { if (mode == 1) { setServoAngle(0); }break; }
                    case 3: { mode = 0;break; }
                    case 4: { mode = 1;break; }
                    }
                }
            }
            delay(100);
        }}
    );

    // Automatic mode thread: logic for auto control
    std::thread threadAutoMode([&]() {
        while (!isExit)
        {
            if (mode == 0)
            {
                if (infrared_status == LOW) // Pet detected
                {
                    if (weights < weights_threshold)
                    {
                        servo_flag = 1;
                        openWaterPump();
                        // setServoAngle(1);
                    }
                    else
                    {
                        servo_flag = 2;
                        closeWaterPump();
                        // setServoAngle(0);
                    }
                }
                else {
                    closeWaterPump();
                    servo_flag = 0;
                    // setServoAngle(0);
                }
            }

            delay(100);
        }}
    );

    // MQTT communication thread
    std::thread threadMqtt(
        [&]() {
            auto& mqtt = MQTTClientWrapper::getInstance();
            try {
                mqtt.initialize(mServerUrl, mClientId);
                mqtt.connect(mUserName, mPassWord);
                mqtt.setMessageHandler([&](const std::string& topic, const std::string& msg) {
                    std::cout << "Received message on [" << topic << "]: " << msg << std::endl;
                    nlohmann::json j = nlohmann::json::parse(msg);
                    if (j["mode"] == 1)
                    {
                        if (j["state"] == 1)
                        {
                            openWaterPump();

                        }
                        else
                        {
                            closeWaterPump();

                        }
                    }
                    else if (j["mode"] == 2)
                    {
                        if (j["state"] == 1)
                        {
                            // setServoAngle(1);
                            servo_flag = 1;
                        }
                        else
                        {
                            // setServoAngle(0);
                            servo_flag = 2;
                        }
                    }
                    });
                mqtt.subscribe(mSubscribeToptic);
                isMqttConnStatus = true;
                // mqtt.publish("test/topic", "Hello MQTT");
            }
            catch (const std::exception& e) {
                std::cerr << "MQTT Error: " << e.what() << std::endl;
                isMqttConnStatus = false;
            }
            while (!isExit)
            {
                if (isMqttConnStatus)
                {
                    // 构建json
                    nlohmann::json j = nlohmann::json::object();
                    j["mode"] = ((mode == 1) ? "Remote" : "Auto");
                    j["detection"] = !infrared_status;
                    j["weight"] = weights;
                    j["pump"] = water_pump_status;
                    j["servo"] = servo_status;
                    mqtt.publish(mPublishTopic, j.dump());
                    // serial_port.send("hello world!\r\n");
                }
                delay(500);
            }
            mqtt.disconnect();
        }
    );

    // Main loop: read sensors and update servo
    while (1)
    {
        infrared_status = digitalRead(INFRA_RED_PIN);
        // setServoAngle(90);
        weights = hx.weight(std::chrono::seconds(1)).getValue();
        if (weights < 0) weights = .0f;
        if (servo_flag == 1)
        {
            setServoAngle(1);

        }
        else if (servo_flag == 2)
        {
            setServoAngle(0);
        }
    }


}
#else


#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#define PWM_PIN 0
int main(void)
{
    printf("wiringPi-C PWM test program\n");
    
    // Initialize WiringPi
    wiringPiSetup();
    pinMode(PWM_PIN, OUTPUT);
    softPwmCreate(PWM_PIN, 0, 200);// Create software PWM with 200 steps per period

    // Move servo motor through several predefined angles
    printf("Current direction: -90 degrees\n");
    softPwmWrite(PWM_PIN, 5);
    delay(1000);
    
    printf("Current direction: -45 degrees\n");
    softPwmWrite(PWM_PIN, 10);
    delay(1000);
    
    printf("Current direction: 0 degrees\n");
    softPwmWrite(PWM_PIN, 15);
    delay(1000);
    
    printf("Current direction: 45 degrees\n");
    softPwmWrite(PWM_PIN, 20);
    delay(1000);
    
    printf("Current direction: 90 degrees\n");
    softPwmWrite(PWM_PIN, 25);
    delay(1000);

    // Loop for continuous clockwise and counter-clockwise motion
    int i = 0;
    while (1) {
        printf("Clockwise rotation\n");
        for (i = 25; i >= 5; i--) {
            softPwmWrite(PWM_PIN, i);
            delay(20);
        }
        
        printf("Counter-clockwise rotation\n");
        for (i = 5; i <= 25; i++) {
            softPwmWrite(PWM_PIN, i);
            delay(20);
        }
    }
    return 0;
}
#endif
