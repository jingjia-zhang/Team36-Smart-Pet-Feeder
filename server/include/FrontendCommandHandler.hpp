#ifndef FRONTEND_COMMAND_HANDLER_HPP
#define FRONTEND_COMMAND_HANDLER_HPP

#include "RequestHandler.hpp"

/**
 * @brief Handles commands from frontend/web clients
 *
 * Processes control commands and data requests from user interfaces
 */
class FrontendCommandHandler : public RequestHandler {
public:
    bool canHandle(const json& data) const override;
    void handle(Server& server, connection_hdl hdl, const json& data) override;
};

#endif // FRONTEND_COMMAND_HANDLER_HPP