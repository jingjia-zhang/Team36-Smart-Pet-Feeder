#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>
class Server;
using json = nlohmann::json;
using connection_hdl = websocketpp::connection_hdl;

/**
 * @brief Abstract base class for all request handlers
 *
 * Defines the interface for handling different types of WebSocket messages
 */
class RequestHandler {
public:
    virtual ~RequestHandler() = default;

    /**
     * @brief Check if this handler can process the given message
     * @param data JSON payload from WebSocket message
     * @return true if this handler can process the message
     */
    [[nodiscard]] virtual bool canHandle(const json& data) const = 0;

    /**
     * @brief Process the WebSocket message
     * @param server Reference to the main server instance
     * @param hdl Connection handle of the client
     * @param data Parsed JSON message payload
     */
    virtual void handle(Server& server, connection_hdl hdl, const json& data) = 0;
};

#endif // REQUEST_HANDLER_HPP