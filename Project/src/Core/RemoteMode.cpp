#include "Core/RemoteMode.hpp"
#include <sstream>
#include <iostream>

void RemoteMode::registerCommand(const std::string& cmd, CommandHandler handler) {
    commandHandlers_[cmd] = handler;
}

void RemoteMode::handleMessage(const std::string& message) {
    std::istringstream iss(message);
    std::string cmd;
    iss >> cmd;

    if (auto it = commandHandlers_.find(cmd); it != commandHandlers_.end()) {
        std::string args;
        std::getline(iss, args);
        args.erase(0, args.find_first_not_of(" \t"));
        it->second(args);
    } else {
        std::cerr << "Unknown command: " << cmd << std::endl;
    }
}
