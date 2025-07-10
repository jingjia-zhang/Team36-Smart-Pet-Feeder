#include "Communication/WebSocketClient.hpp"
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <thread>
#include <mutex>

class WebSocketClient::Impl {
public:
    using Client = websocketpp::client<websocketpp::config::asio_client>;
    using MessagePtr = websocketpp::config::asio_client::message_type::ptr;

    Impl() : client_(), handler_(), connection_hdl_(), connected_(false), auto_reconnect_(false), g_running_(true) {
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

    ~Impl() {
        g_running_ = false;
        disconnect();
        if (reconnect_thread_.joinable()) {
            reconnect_thread_.join();
        }
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

    void setErrorHandler(ErrorHandler handler) {
        error_handler_ = std::move(handler);
    }

    bool isConnected() const {
        return connected_;
    }

    void startAutoReconnect(const std::string& url) {
        reconnect_thread_ = std::thread([this, url]() {
            while (g_running_ && auto_reconnect_) {
                std::this_thread::sleep_for(std::chrono::seconds(5));
                if (!connected_) {
                    try {
                        connect(url);
                    } catch (...) {
                        if (error_handler_) {
                            error_handler_("Reconnect failed");
                        }
                    }
                }
            }
        });
    }

private:
    Client client_;
    MessageHandler handler_;
    ErrorHandler error_handler_;
    websocketpp::connection_hdl connection_hdl_;
    std::thread thread_;
    std::thread reconnect_thread_;
    mutable std::mutex mutex_;
    bool connected_;
    bool auto_reconnect_;
    std::atomic<bool> g_running_;
};

WebSocketClient::WebSocketClient() : impl_(std::make_unique<Impl>()) {}
WebSocketClient::~WebSocketClient() = default;

void WebSocketClient::connect(const std::string& url, bool auto_reconnect) {
    impl_->auto_reconnect_ = auto_reconnect;
    impl_->connect(url);
    if (auto_reconnect) impl_->startAutoReconnect(url);
}

void WebSocketClient::disconnect() { impl_->disconnect(); }
void WebSocketClient::send(const std::string& message) { impl_->send(message); }
void WebSocketClient::setMessageHandler(MessageHandler handler) { impl_->setMessageHandler(handler); }
void WebSocketClient::setErrorHandler(ErrorHandler handler) { impl_->setErrorHandler(handler); }
bool WebSocketClient::isConnected() const { return impl_->isConnected(); }