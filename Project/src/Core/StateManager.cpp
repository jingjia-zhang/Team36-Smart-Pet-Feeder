#include "Core/StateManager.hpp"
#include <algorithm>
#include <sstream>

StateManager::StateManager(std::shared_ptr<IServoMotor> servo,
                         std::shared_ptr<IWaterPump> pump,
                         std::shared_ptr<IWeightSensor> weightSensor,
                         std::shared_ptr<Database> db)
    : servo_(servo), pump_(pump), weightSensor_(weightSensor), database_(db) {
    // Initialize default state
    currentMode_ = Mode::Auto;
    lastWeightBeforeFeeding_ = 0.0f;
}



void StateManager::controlledFeeding(float targetAmount) {
    if (emergencyFlag_) return;

    constexpr float tolerance = 2.0f;
    constexpr auto timeout = std::chrono::seconds(30);
    auto startTime = std::chrono::steady_clock::now();
    float initialWeight = weightSensor_->getWeight();

    startFeeding();

    while (!emergencyFlag_) {
        float currentWeight = weightSensor_->getWeight();
        float delta = currentWeight - initialWeight;

        if (delta >= targetAmount - tolerance) break;
        if (std::chrono::steady_clock::now() - startTime > timeout) {
            notifyStatus("Feeding timeout");
            break;
        }
        if (!petDetected_) {
            notifyStatus("Pet left during feeding");
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    stopFeeding();
}

void StateManager::notifyStatus(const std::string& message) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    if (statusCallback_) {
        statusCallback_(message);
    }
}

void StateManager::startFeeding() {
    if (!feeding_.exchange(true)) {
        lastWeightBeforeFeeding_ = currentWeight_;
        servo_->setAngle(45); // Opening angle
        notifyStatus("Feeding started");
    }
}

void StateManager::stopFeeding() {
    if (feeding_.exchange(false)) {
        servo_->setAngle(0); // Close angle
        logFeedingEvent();
        notifyStatus("Feeding stopped");
    }
}

void StateManager::emergencyStop() {
    emergencyFlag_ = true;
    stopFeeding();
    stopWatering();
    notifyStatus("EMERGENCY STOP ACTIVATED");
}

void StateManager::logFeedingEvent() {
    if (database_ && lastWeightBeforeFeeding_ > 0) {
        try {
            database_->logFeeding(lastWeightBeforeFeeding_, currentWeight_);
        } catch (const std::exception& e) {
            notifyStatus("Database error: " + std::string(e.what()));
        }
    }
}