#include "FrontendCommandHandler.hpp"

#include <Server.hpp>

bool FrontendCommandHandler::canHandle(const json& data) const {
    // Handle messages containing command field
    return data.contains("command");
}

void FrontendCommandHandler::handle(Server& server, connection_hdl hdl, const json& data) {
    const std::string command = data["command"];

    // Process device control commands
    if (command == "toggle_pump" || command == "toggle_servo" ||
        command == "start_stream" || command == "stop_stream") {
        server.sendToDevice(data);
        }
    // Process history data requests
    else if (command == "get_history") {
        int limit = data.value("limit", 100);
        json history = server.getFeedingHistory(limit);
        server.sendToClient(hdl, history.dump());
    }
}