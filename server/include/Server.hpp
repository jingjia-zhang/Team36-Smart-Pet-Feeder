#ifndef SERVER_HPP
#define SERVER_HPP

#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <nlohmann/json.hpp>
#include <SQLiteCpp/SQLiteCpp.h>
#include <mutex>
#include <map>
#include <string>
#include <memory>
#include <chrono>
#include <vector>

// Forward declarations
class RequestHandler;

using json = nlohmann::json;
using websocket_server = websocketpp::server<websocketpp::config::asio>;
using connection_hdl = websocketpp::connection_hdl;

// Device status structure
struct DeviceState {
    std::string mode = "Remote";          // Operation mode (Auto/Remote)
    float weight = 0.0f;                  // Current weight measurement (grams)
    bool pet_detected = false;            // Pet presence detection flag
    bool pump_status = false;             // Water pump status (true = running)
    bool servo_status = false;            // Servo motor status (true = open)
    std::chrono::system_clock::time_point last_update; // Last update timestamp
};

class Server {
public:
    Server(const std::string& db_path, uint16_t port);
    ~Server();

    // Server control
    void run();

    // Public interface for handlers
    void updateDeviceState(const std::string& mode, float weight,
                         bool pet_detected, bool pump_status, bool servo_status);
    void broadcastToFrontends(const json& data);
    void sendToDevice(const json& data);
    void sendToClient(connection_hdl hdl, const std::string& message);
    void saveFeedingRecord(float amount);
    json getFeedingHistory(int limit) const;
    bool isDeviceConnected() const;
    void init_database();

private:
    // WebSocket callbacks
    void on_open(connection_hdl hdl);
    void on_close(connection_hdl hdl);
    void on_message(connection_hdl hdl, websocket_server::message_ptr msg);

    // Message processing
    void registerHandlers();
    void processMessage(connection_hdl hdl, const json& data);

    // Connection management
    enum ConnectionType { DEVICE, FRONTEND };
    std::map<connection_hdl, ConnectionType, std::owner_less<connection_hdl>> connections_;
    connection_hdl device_hdl_;  // Handle to device connection
    std::mutex conn_mutex_;      // Mutex for connection operations

    // Device state
    DeviceState device_state_;
    std::mutex state_mutex_;     // Mutex for state operations

    // Core components
    websocket_server server_;
    uint16_t port_;
    std::string db_path_;
    std::unique_ptr<SQLite::Database> db_;

    // Request handlers
    std::vector<std::unique_ptr<RequestHandler>> handlers_;
};

#endif // SERVER_HPP