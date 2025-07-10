#pragma once
#include <functional>
#include <atomic>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <string>
#include "StateManager.hpp"

class AutoMode {
public:
    using StateCheck = std::function<bool(const StateManager&)>;
    using StateAction = std::function<void(StateManager&)>;

    struct Rule {
        StateCheck condition;
        StateAction action;
        std::chrono::milliseconds cooldown;
        std::string name;
    };

    explicit AutoMode(std::shared_ptr<StateManager> state);

    void addRule(Rule rule);
    void update();
    void setEnabled(bool enabled);

private:
    std::shared_ptr<StateManager> state_manager_;
    std::vector<Rule> rules_;
    std::atomic<bool> enabled_{true};
    std::unordered_map<size_t, std::chrono::steady_clock::time_point> lastExecuted_;

    bool shouldExecuteRule(const Rule& rule,
                           const std::chrono::steady_clock::time_point& now,
                           size_t ruleIndex) const;
    void logExecution(const std::string& ruleName) const;
    void logError(const std::string& ruleName, const std::string& error) const;
};