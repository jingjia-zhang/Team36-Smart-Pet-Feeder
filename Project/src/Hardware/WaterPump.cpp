#include "Hardware/WaterPump.hpp"
#include "Hardware/GPIOManager.hpp"

WaterPump::WaterPump(std::shared_ptr<GPIOChip> gpio, int pin) 
    : gpio_(GPIOManager::getInstance()), pin_(pin), state_(false) {
    gpio_->setDirection(pin, GPIOChip::Direction::Output);
    turnOff();
}

void WaterPump::turnOn() {
    gpio_->write(pin_, true);
    state_ = true;
}

void WaterPump::turnOff() {
    gpio_->write(pin_, false);
    state_ = false;
}

bool WaterPump::isOn() const {
    return state_;
}
