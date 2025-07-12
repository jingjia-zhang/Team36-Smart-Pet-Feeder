#include "Core/AutoMode.hpp"
#include <iostream>

AutoMode::AutoMode(std::shared_ptr<StateManager> state)
    : state_manager_(std::move(state)) {

    auto condition = [](const StateManager& state) {
        return state.getCurrentWeight() < 10.0f && state.isPetDetected();
    };
    auto action = [](StateManager& state) {
        state.startFeeding();
        state.startWatering();
    };
    addRule(Rule{
        condition,
        action,
        std::chrono::milliseconds(5000), // The cooling time is 5 seconds
        "LowWeightFeedingAndWateringRule"
    });

    auto condition2 = [](const StateManager& state) {
        return state.getCurrentWeight() >= 10.0f || !state.isPetDetected();
    };
    auto action2 = [](StateManager& state) {
        state.stopFeeding();
        state.stopWatering();
    };
    addRule(Rule{
        condition2,
        action2,
        std::chrono::milliseconds(2000), // The cooling time is 2 seconds
        "NormalWeightStopRule"
    });
}

void AutoMode::addRule(Rule rule) {
    rules_.push_back(std::move(rule));
}

void AutoMode::update() {
    if (!enabled_ || !state_manager_) return;

    const auto now = std::chrono::steady_clock::now();
    for (size_t i = 0; i < rules_.size(); ++i) {
        try {
            const auto& rule = rules_[i];
            if (shouldExecuteRule(rule, now, i)) {
                if (rule.condition(*state_manager_)) {
                    rule.action(*state_manager_);
                    lastExecuted_[i] = now;
                    logExecution(rule.name);
                }
            }
        } catch (const std::exception& e) {
            logError(rules_[i].name, e.what());
        }
    }
}

bool AutoMode::shouldExecuteRule(const Rule& rule,
                                 const std::chrono::steady_clock::time_point& now,
                                 size_t ruleIndex) const {
    auto lastExec = lastExecuted_.find(ruleIndex);
    return lastExec == lastExecuted_.end() ||
           (now - lastExec->second) >= rule.cooldown;
}

void AutoMode::logExecution(const std::string& ruleName) const {
    std::cout << "Rule executed: " << ruleName << std::endl;
}

void AutoMode::logError(const std::string& ruleName, const std::string& error) const {
    std::cerr << "Error in rule " << ruleName << ": " << error << std::endl;
}

void AutoMode::setEnabled(bool enabled) {
    enabled_ = enabled;
}