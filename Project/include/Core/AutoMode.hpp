#pragma once
#include <functional>
#include <atomic>
#include <vector>
#include <unordered_map>
#include <chrono>

class AutoMode {
public:
    using ConditionCheck = std::function<bool()>;
    using ActionCallback = std::function<void()>;

    struct Rule {
        ConditionCheck condition;
        ActionCallback action;
        std::chrono::milliseconds cooldown;
    };

    AutoMode() = default;

    void addRule(Rule rule);
    void update();
    void setEnabled(bool enabled);

private:
    std::vector<Rule> rules_;
    std::atomic<bool> enabled_{true};
    std::unordered_map<size_t, std::chrono::steady_clock::time_point> lastExecuted_;
};