#include <chrono>
#include <thread>
#include <stdexcept>
#include <fstream>
#include "Hardware/WeightSensor.hpp"
#include "Hardware/GPIOChip.hpp"

HX711WeightSensor::HX711WeightSensor(int doutPin, int sckPin, int gain)
    : gpio_(GPIOManager::getInstance()),
      doutPin_(doutPin),
      sckPin_(sckPin),
      gain_(gain),
      offset_(0),
      scale_(1.0f) {

    gpio_ = GPIOManager::getInstance();
    gpio_->setDirection(sckPin_, GPIOChip::Direction::Output);
    gpio_->setDirection(doutPin_, GPIOChip::Direction::Input);
    gpio_->setBias(doutPin_, gpiod::line::bias::PULL_UP);

    calibrate(); // initial calibration
}

bool HX711WeightSensor::isReady() const {
    return !gpio_->read(doutPin_);
}

void HX711WeightSensor::pulseClock(int times) {
    for (int i = 0; i < times; i++) {
        gpio_->write(sckPin_, true);
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        gpio_->write(sckPin_, false);
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
}

void HX711WeightSensor::setGain() {
    gpio_->write(sckPin_, false);
    std::this_thread::sleep_for(std::chrono::microseconds(1));
    pulseClock(gain_);
}

long HX711WeightSensor::readData() {
    std::lock_guard<std::mutex> lock(sensorMutex_);

    while (!isReady()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    long data = 0;
    for (int i = 0; i < 24; ++i) {
        gpio_->write(sckPin_, true);
        std::this_thread::sleep_for(std::chrono::microseconds(1));

        data <<= 1;
        if (gpio_->read(doutPin_)) {
            data |= 1;
        }

        gpio_->write(sckPin_, false);
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    // Set Gain
    pulseClock(gain_);


    if (data & 0x800000) {
        data |= 0xFF000000; // Extend the height by 8 bits to 1
    }

    return data;
}

float HX711WeightSensor::getWeight() {
    const int samples = 5;
    long sum = 0;

    for (int i = 0; i < samples; i++) {
        sum += readData();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    float reading = (sum / samples - offset_) / scale_;
    return (reading > 0) ? reading : 0.0f;
}

void HX711WeightSensor::calibrate() {
    const int samples = 10;
    long sum = 0;

    for (int i = 0; i < samples; ++i) {
        sum += readData();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    offset_ = sum / samples;
}

void HX711WeightSensor::saveCalibration(const std::string& path) {
    std::ofstream file(path);
    if (file) {
        file << offset_ << "\n" << scale_;
    }
}

void HX711WeightSensor::loadCalibration(const std::string& path) {
    std::ifstream file(path);
    if (file) {
        file >> offset_ >> scale_;
    }
}

void HX711WeightSensor::tare() {
    calibrate();
}

void HX711WeightSensor::setScale(float scale) {
    scale_ = scale;
}

void HX711WeightSensor::calibrateWithKnownWeight(float knownWeight) {
    if (knownWeight <= 0) {
        throw std::invalid_argument("The known weight must be greater than 0");
    }

    long rawData = readData();
    scale_ = (rawData - offset_) / knownWeight;
}