#pragma once
#include <gpiod.hpp>
#include <memory>
#include <unordered_map>
#include <stdexcept>

class GPIOChip {
public:
    enum class Direction { Input, Output };
    enum class Edge { None, Rising, Falling, Both };
    
    explicit GPIOChip(const std::string& chipName = "gpiochip0");
    
    void setDirection(int pin, Direction dir);
    void setEdgeDetection(int pin, Edge edge);
    void setBias(int pin, gpiod::line::bias bias);
    
    void write(int pin, bool value);
    bool read(int pin);
    
    void setInterruptCallback(int pin, std::function<void()> callback);
    
private:
    std::unique_ptr<gpiod::chip> chip_;
    std::unordered_map<int, gpiod::line> lines_;
    std::unordered_map<int, std::function<void()>> callbacks_;
    
    void requestLineIfNeeded(int pin);
};