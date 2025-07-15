#pragma once
#include "Hardware/GPIOChip.hpp"
#include <memory>
#include <chrono>

class IServoMotor {
public:
    virtual void setAngle(int angle) = 0; // 0-180 degrees
    virtual ~IServoMotor() = default;
};

class ServoMotor : public IServoMotor {
public:
    ServoMotor(std::shared_ptr<GPIOChip> gpio, int pin);
    void setAngle(int angle) override;

private:
    std::shared_ptr<GPIOChip> gpio_;
    int pin_;
    int currentAngle_;

    void generatePulse(int widthUs);
};
