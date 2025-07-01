#include "Communication/WebSocketClient.hpp"
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <thread>
#include <mutex>

class WebSocketClient::Impl {
public:
    using Client = websocketpp::client<websocketpp::config::asio_client>;
    using MessagePtr = websocketpp::config::asio_client::message_type::ptr;
    
    Impl() : client_(), handler_(), connection_hdl_(), connected_(false) {
        client_.init_asio();
        client_.set_open_handler([this](auto hdl) {
            std::lock_guard<std::mutex> lock(mutex_);
            connection_hdl_ = hdl;
            connected_ = true;
        });
        client_.set_close_handler([this](auto) {
            std::lock_guard<std::mutex> lock(mutex_);
            connected_ = false;
        });
        client_.set_message_handler([this](auto, MessagePtr msg) {
            if (handler_) {
                handler_(msg->get_payload());
            }
        });
    }
    
    void connect(const std::string& url) {
        websocketpp::lib::error_code ec;
        Client::connection_ptr con = client_.get_connection(url, ec);
        if (ec) {
            throw std::runtime_error("Connection error: " + ec.message());
        }
        client_.connect(con);
        thread_ = std::thread([this]() { client_.run(); });
    }
    
    void disconnect() {
        if (connected_) {
            client_.close(connection_hdl_, websocketpp::close::status::normal, "");
        }
        if (thread_.joinable()) {
            thread_.join();
        }
    }
    
    void send(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (connected_) {
            client_.send(connection_hdl_, message, websocketpp::frame::opcode::text);
        }
    }
    
    void setMessageHandler(MessageHandler handler) {
        handler_ = std::move(handler);
    }
    
    bool isConnected() const {
        return connected_;
    }
    
private:
    Client client_;
    MessageHandler handler_;
    websocketpp::connection_hdl 