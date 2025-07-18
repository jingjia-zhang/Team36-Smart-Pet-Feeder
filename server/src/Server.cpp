#include "Server.hpp"
#include "DeviceMessageHandler.hpp"
#include "FrontendCommandHandler.hpp"
#include <iostream>

Server::Server(const std::string& db_path, uint16_t port)
    : port_(port), db_path_(db_path) {
    // Set WebSocket event handlers
    server_.set_open_handler([this](auto hdl) { this->on_open(hdl); });
    server_.set_close_handler([this](auto hdl) { this->on_close(hdl); });
    server_.set_message_handler([this](auto hdl, auto msg) { this->on_message(hdl, msg); });

    // Initialize ASIO networking
    server_.init_asio();

    // Initialize database connection
    init_database();

    // Register all request handlers
    registerHandlers();
}

Server::~Server() {
    // Gracefully stop the server
    server_.stop();
}

void Server::run() {
    std::cout << "Starting WebSocket server on port " << port_ << std::endl;
    try {
        server_.listen(port_);
        server_.start_accept();
        server_.run();
    } catch (const std::exception& e) {
        std::cerr << "Server run error: " << e.what() << std::endl;
    }
}

void Server::on_open(connection_hdl hdl) {
    std::lock_guard<std::mutex> lock(conn_mutex_);
    // New connections are marked as frontend by default
    connections_[hdl] = FRONTEND;
    std::cout << "New connection established. Total connections: "
              << connections_.size() << std::endl;
}

void Server::on_close(connection_hdl hdl) {
    std::lock_guard<std::mutex> lock(conn_mutex_);
    auto type = connections_[hdl];
    connections_.erase(hdl);

    // Clear device handle if the disconnected client was the device
    if (type == DEVICE && !device_hdl_.expired() &&
        !std::owner_less<connection_hdl>()(hdl, device_hdl_) &&
        !std::owner_less<connection_hdl>()(device_hdl_, hdl)) {
        device_hdl_.reset();
        std::cout << "Device disconnected" << std::endl;
    }

    std::cout << "Connection closed. Remaining connections: "
              << connections_.size() << std::endl;
}

void Server::on_message(connection_hdl hdl, websocket_server::message_ptr msg) {
    try {
        auto data = json::parse(msg->get_payload());
        std::lock_guard<std::mutex> lock(conn_mutex_);

        // Identify device connections (must contain device_id in first message)
        if (connections_[hdl] == DEVICE || data.contains("device_id")) {
            connections_[hdl] = DEVICE;
            device_hdl_ = hdl;
        }

        processMessage(hdl, data);
    } catch (const std::exception& e) {
        std::cerr << "Message processing error: " << e.what() << std::endl;
    }
}

void Server::registerHandlers() {
    // Register all available message handlers
    handlers_.push_back(std::make_unique<DeviceMessageHandler>());
    handlers_.push_back(std::make_unique<FrontendCommandHandler>());
}

void Server::processMessage(connection_hdl hdl, const json& data) {
    // Find the first handler that can process this message
    for (auto& handler : handlers_) {
        if (handler->canHandle(data)) {
            handler->handle(*this, hdl, data);
            return;
        }
    }
    std::cerr << "Unhandled message type: " << data.dump() << std::endl;
}

void Server::init_database() {
    try {
        // Open or create SQLite database
        db_ = std::make_unique<SQLite::Database>(
            db_path_,
            SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE
        );

        // Create feeding history table if not exists
        db_->exec(R"(
            CREATE TABLE IF NOT EXISTS feeding_history (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
                amount REAL NOT NULL
            )
        )");

        std::cout << "Database initialized successfully at: "
                  << db_path_ << std::endl;
    } catch (const SQLite::Exception& e) {
        std::cerr << "SQLite error during initialization: "
                  << e.what() << std::endl;
        exit(1);
    }
}

// Public interface implementations
void Server::updateDeviceState(const std::string& mode, float weight,
                             bool pet_detected, bool pump_status,
                             bool servo_status) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    device_state_.mode = mode;
    device_state_.weight = weight;
    device_state_.pet_detected = pet_detected;
    device_state_.pump_status = pump_status;
    device_state_.servo_status = servo_status;
    device_state_.last_update = std::chrono::system_clock::now();
}

void Server::broadcastToFrontends(const json& data) {
    std::lock_guard<std::mutex> lock(conn_mutex_);
    std::string payload = data.dump();

    for (auto& [hdl, type] : connections_) {
        if (type == FRONTEND) {
            server_.send(hdl, payload, websocketpp::frame::opcode::text);
        }
    }
}

void Server::sendToDevice(const json& data) {
    std::lock_guard<std::mutex> lock(conn_mutex_);
    if (device_hdl_.expired()) {
        std::cerr << "Cannot send to device - no active device connection" << std::endl;
        return;
    }
    server_.send(device_hdl_, data.dump(), websocketpp::frame::opcode::text);
}

void Server::sendToClient(connection_hdl hdl, const std::string& message) {
    server_.send(hdl, message, websocketpp::frame::opcode::text);
}

void Server::saveFeedingRecord(float amount) {
    try {
        SQLite::Statement query(*db_,
            "INSERT INTO feeding_history (amount) VALUES (?)");
        query.bind(1, amount);
        query.exec();
        std::cout << "Recorded feeding: " << amount << " grams" << std::endl;
    } catch (const SQLite::Exception& e) {
        std::cerr << "Failed to save feeding record: " << e.what() << std::endl;
    }
}

json Server::getFeedingHistory(int limit) const {
    json history = json::array();
    try {
        SQLite::Statement query(*db_,
            "SELECT timestamp, amount FROM feeding_history "
            "ORDER BY id DESC LIMIT ?");
        query.bind(1, limit);

        while (query.executeStep()) {
            json record;
            record["time"] = query.getColumn(0).getText();
            record["amount"] = query.getColumn(1).getDouble();
            history.push_back(record);
        }
    } catch (const SQLite::Exception& e) {
        std::cerr << "Failed to retrieve history: " << e.what() << std::endl;
    }
    return history;
}

bool Server::isDeviceConnected() const {
    return !device_hdl_.expired();
}