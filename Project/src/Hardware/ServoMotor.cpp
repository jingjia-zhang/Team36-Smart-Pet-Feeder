#include "Hardware/ServoMotor.hpp"
#include "Hardware/GPIOManager.hpp"
#include <thread>
#include <chrono>

ServoMotor::ServoMotor(std::shared_ptr<GPIOChip> gpio, int pin)
    : gpio_(GPIOManager::getInstance()), pin_(pin), currentAngle_(90) {
    gpio_->setDirection(pin, GPIOChip::Direction::Output);
}

void ServoMotor::setAngle(int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    currentAngle_ = angle;
    int pulseWidth = 500 + (angle * 2000 / 180); // 500-2500us

    generatePulse(pulseWidth);
}

void ServoMotor::generatePulse(int widthUs) {
    const int periodMs = 20; // 50Hz

    gpio_->write(pin_, true);
    std::this_thread::sleep_for(std::chrono::microseconds(widthUs));
    gpio_->write(pin_, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(periodMs) -
                              std::chrono::microseconds(widthUs));
}