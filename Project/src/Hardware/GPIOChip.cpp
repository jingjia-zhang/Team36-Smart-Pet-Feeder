#include "Hardware/GPIOChip.hpp"
#include <thread>
#include <poll.h>

GPIOChip::GPIOChip(const std::string& chipName) {
    try {
        chip_ = std::make_unique<gpiod::chip>(chipName);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to open GPIO chip: " + std::string(e.what()));
    }
}

void GPIOChip::requestLineIfNeeded(int pin) {
    if (lines_.find(pin) == lines_.end()) {
        try {
            lines_[pin] = chip_->get_line(pin);
        } catch (...) {
            throw std::runtime_error("Failed to get line for pin " + std::to_string(pin));
        }
    }
}

void GPIOChip::setDirection(int pin, Direction dir) {
    requestLineIfNeeded(pin);
    
    gpiod::line_request config;
    config.consumer = "pet_feeder";
    
    switch (dir) {
        case Direction::Input:
            config.request_type = gpiod::line_request::DIRECTION_INPUT;
            break;
        case Direction::Output:
            config.request_type = gpiod::line_request::DIRECTION_OUTPUT;
            break;
    }
    
    lines_[pin].request(config);
}

void GPIOChip::write(int pin, bool value) {
    requestLineIfNeeded(pin);
    lines_[pin].set_value(value ? 1 : 0);
}

bool GPIOChip::read(int pin) {
    requestLineIfNeeded(pin);
    return lines_[pin].get_value() != 0;
}

void GPIOChip::setEdgeDetection(int pin, Edge edge) {
    requestLineIfNeeded(pin);
    
    gpiod::line_request config;
    config.consumer = "pet_feeder";
    config.request_type = gpiod::line_request::EVENT_BOTH_EDGES;
    
    switch (edge) {
        case Edge::None:
            config.request_type = gpiod::line_request::DIRECTION_INPUT;
            break;
        case Edge::Rising:
            config.request_type = gpiod::line_request::EVENT_RISING_EDGE;
            break;
        case Edge::Falling:
            config.request_type = gpiod::line_request::EVENT_FALLING_EDGE;
            break;
        case Edge::Both:
            config.request_type = gpiod::line_request::EVENT_BOTH_EDGES;
            break;
    }
    
    lines_[pin].request(config);
}

void GPIOChip::setInterruptCallback(int pin, std::function<void()> callback) {
    setEdgeDetection(pin, Edge::Both);
    callbacks_[pin] = callback;
    
    std::thread([this, pin]() {
        while (true) {
            if (lines_[pin].event_wait(std::chrono::seconds(1))) {
                if (callbacks_[pin]) {
                    callbacks_[pin]();
                }
            }
        }
    }).detach();
}