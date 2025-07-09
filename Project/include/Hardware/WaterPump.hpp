#pragma once
#include "Hardware/GPIOChip.hpp"
#include <memory>

class IWaterPump {
public:
    virtual void turnOn() = 0;
    virtual void turnOff() = 0;
    virtual bool isOn() const = 0;
    virtual ~IWaterPump() = default;
};

class WaterPump : public IWaterPump {
public:
    WaterPump(std::shared_ptr<GPIOChip> gpio, int pin);
    void turnOn() override;
    void turnOff() override;
    bool isOn() const override;
    
private:
    std::shared_ptr<GPIOChip> gpio_;
    int pin_;
    bool state_;
};
