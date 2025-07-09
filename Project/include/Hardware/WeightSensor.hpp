#pragma once
#include <chrono>
#include <memory>

class IWeightSensor {
public:
    virtual float getWeight() = 0;
    virtual ~IWeightSensor() = default;
};

class HX711WeightSensor : public IWeightSensor {
public:
    HX711WeightSensor(int doutPin, int sckPin, int gain = 128);
    float getWeight() override;
    
private:
    int doutPin_;
    int sckPin_;
    int gain_;
    long offset_;
    float scale_;
    
    bool isReady();
    long readData();
    void setGain();
};