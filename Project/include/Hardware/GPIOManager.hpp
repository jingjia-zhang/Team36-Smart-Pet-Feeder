#pragma once
#include <memory>
#include <mutex>
#include "GPIOChip.hpp"

class GPIOManager {
public:
    static std::shared_ptr<GPIOChip> getInstance() {
        std::call_once(initFlag, [](){
            instance = std::make_shared<GPIOChip>("gpiochip0");
            // Initialize default pin direction
            instance->setDirection(17, GPIOChip::Direction::Output); // steering engine
            instance->setDirection(27, GPIOChip::Direction::Output); // water pump
            instance->setDirection(22, GPIOChip::Direction::Input);  // infrared
        });
        return instance;
    }

    // Disable copying and moving
    GPIOManager(const GPIOManager&) = delete;
    GPIOManager& operator=(const GPIOManager&) = delete;

private:
    static std::shared_ptr<GPIOChip> instance;
    static std::once_flag initFlag;
    GPIOManager() = default;
};


std::shared_ptr<GPIOChip> GPIOManager::instance = nullptr;
std::once_flag GPIOManager::initFlag;