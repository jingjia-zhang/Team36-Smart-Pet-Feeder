#ifndef DEVICE_MESSAGE_HANDLER_HPP
#define DEVICE_MESSAGE_HANDLER_HPP

#include "RequestHandler.hpp"

/**
 * @brief Handles messages coming from IoT pet feeder device
 *
 * Processes status updates and sensor data from the physical device
 */
class DeviceMessageHandler : public RequestHandler {
public:
    bool canHandle(const json& data) const override;
    void handle(Server& server, connection_hdl hdl, const json& data) override;
};

#endif // DEVICE_MESSAGE_HANDLER_HPP