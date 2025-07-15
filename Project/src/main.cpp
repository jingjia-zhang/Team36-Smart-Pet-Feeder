#include "Core/StateManager.hpp"
#include "Core/RemoteMode.hpp"
#include "Core/AutoMode.hpp"
#include "Hardware/ServoMotor.hpp"
#include "Hardware/WaterPump.hpp"
#include "Hardware/WeightSensor.hpp"
#include "Hardware/GPIOChip.hpp"
#include "Hardware/CameraModule.hpp"
#include "Communication/WebSocketClient.hpp"
#include "Data/Database.hpp"
#include "Data/DataApi.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <csignal>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>

using json = nlohmann::json;
std::atomic_bool g_running{true};

void signalHandler(int) {
    g_running = false;
}

void handleWebSocket(string url) {
    WebSocketClient websocket;
    websocket.setMessageHandler([&](const std::string& msg) {
        try {
            auto j = json::parse(msg);
            if (j["command"] == "get_history") {
                auto history = dataAPI->getFeedingHistory(j.value("limit", 100));
                websocket.send(history.dump());
            } else if (j["command"] == "remote_command") {
                remoteMode->handleMessage(j["args"].get<std::string>());
            } else if (j["command"] == "start_stream") {
                // Process the command to start the video stream
                if (cameraModule) {
                    cameraModule->startStreaming();
                }
            } else if (j["command"] == "stop_stream") {
                // Process the command to stop video streaming
                if (cameraModule) {
                    cameraModule->stopStreaming();
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "WebSocket message handling error: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown error in WebSocket message handling" << std::endl;
        }
    });
    websocket.connect("ws://" + url);
}

int main() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try {
        // hardware initialization
        auto gpio = std::make_shared<GPIOChip>();
        auto servo = std::make_shared<ServoMotor>(gpio, 17);
        auto pump = std::make_shared<WaterPump>(gpio, 27);
        auto weightSensor = std::make_shared<WeightSensor>(5, 6);
        gpio->setDirection(22, GPIOChip::Direction::Input); // infrared sensor

        // Database and service initialization
        auto database = std::make_shared<Database>();
        auto dataAPI = std::make_shared<DataApi>(database);
        auto stateManager = std::make_shared<StateManager>(servo, pump, weightSensor, database);

        // Remote mode initialization
        auto remoteMode = std::make_shared<RemoteMode>();
        remoteMode->registerCommand("feed", [&](const std::string& args) {
            try {
                float targetAmount = std::stof(args);
                stateManager->controlledFeeding(targetAmount);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument for feed command: " << args << std::endl;
            }
        });

        // Automatic mode initialization
        auto autoMode = std::make_shared<AutoMode>(stateManager);

        // WebSocket service
		handleWebSocket("localhost:8080/pet-feeder");

        // Camera module initialization
        auto cameraModule = std::make_shared<CameraModule>(webSocketClient);
        cameraModule->initialize("/dev/video0", 640, 480);
        cameraModule->setFramerate(15); // Set to 15fps to reduce network burden

        // main control loop
        while (g_running) {
            float weight = weightSensor->getWeight();
            bool petDetected = !gpio->read(22);

            stateManager->updateSensors(weight, petDetected);

            // Update automatic mode
            autoMode->update();

            // Status reporting
            json status;
            status["weight"] = weight;
            status["pet_detected"] = petDetected;
            status["mode"] = (stateManager->getMode() == StateManager::Mode::Auto) ? "auto" : "remote";
            websocket.send(status.dump());

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // clean
        stateManager->stopFeeding();
        stateManager->stopWatering();
        websocket.disconnect();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}