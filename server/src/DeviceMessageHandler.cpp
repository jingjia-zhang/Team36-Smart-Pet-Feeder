#include "DeviceMessageHandler.hpp"

#include <Server.hpp>

bool DeviceMessageHandler::canHandle(const json& data) const {
    // Handle messages containing device-specific fields
    return data.contains("device_id") ||
           data.contains("mode") ||
           data.contains("weight") ||
           data.contains("pet_detected");
}

void DeviceMessageHandler::handle(Server& server, connection_hdl hdl, const json& data) {
    // Extract data with default values if fields are missing
    std::string mode = data.value("mode", "");
    float weight = data.value("weight", 0.0f);
    bool pet_detected = data.value("pet_detected", false);
    bool pump_status = data.value("pump_status", false);
    bool servo_status = data.value("servo_status", false);

    // Update server state through public interface
    server.updateDeviceState(mode, weight, pet_detected, pump_status, servo_status);

    // Broadcast update to all frontend clients
    server.broadcastToFrontends(data);

    // Record feeding event if servo was activated
    if (servo_status) {
        server.saveFeedingRecord(weight);
    }
}