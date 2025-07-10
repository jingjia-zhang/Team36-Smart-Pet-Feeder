#pragma once
#include <functional>
#include <string>
#include <unordered_map>

class RemoteMode {
public:
    using CommandHandler = std::function<void(const std::string&)>;
    
    void registerCommand(const std::string& cmd, CommandHandler handler);
    void handleMessage(const std::string& message);
    
private:
    std::unordered_map<std::string, CommandHandler> commandHandlers_;
};
