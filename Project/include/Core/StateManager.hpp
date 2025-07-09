#pragma once
#include <memory>
#include <atomic>
#include <mutex>
#include <vector>
#include <chrono>
#include <functional>
#include "../Hardware/ServoMotor.hpp"
#include "../Hardware/WaterPump.hpp"
#include "../Hardware/WeightSensor.hpp"
#include "../Data/Database.hpp"

class StateManager {
public:
    enum class Mode { Auto, Remote, Manual };

    struct FeedingSchedule {
        std::chrono::system_clock::time_point time;
        float amount;
        bool recurring;
        std::chrono::hours interval;
    };

    StateManager(std::shared_ptr<IServoMotor> servo,
               std::shared_ptr<IWaterPump> pump,
               std::shared_ptr<IWeightSensor> weightSensor,
               std::shared_ptr<Database> database);

    // mode control
    void setMode(Mode mode);
    Mode getMode() const;

    // Sensor update
    void updateSensors(float weight, bool petDetected);

    // control command
    void startFeeding();
    void stopFeeding();
    void startWatering();
    void stopWatering();
    void emergencyStop();

    // Status inquiry
    float getCurrentWeight() const;
    bool isPetDetected() const;
    bool isFeeding() const;
    bool isWatering() const;

    // Callback settings
    using StatusCallback = std::function<void(const std::string&)>;
    void setStatusCallback(StatusCallback callback);

private:
    std::shared_ptr<IServoMotor> servo_;
    std::shared_ptr<IWaterPump> pump_;
    std::shared_ptr<IWeightSensor> weightSensor_;
    std::shared_ptr<Database> database_;

    std::atomic<Mode> currentMode_{Mode::Auto};
    std::atomic<float> currentWeight_{0};
    std::atomic<bool> petDetected_{false};
    std::atomic<bool> feeding_{false};
    std::atomic<bool> watering_{false};
    std::atomic<bool> emergencyFlag_{false};
    float lastWeightBeforeFeeding_{0};

    std::vector<FeedingSchedule> schedules_;
    mutable std::mutex scheduleMutex_;
    mutable std::mutex callbackMutex_;
    StatusCallback statusCallback_;

    void executeAutoModeLogic();
    void controlledFeeding(float targetAmount);
    void logFeedingEvent();
    void notifyStatus(const std::string& message);
};