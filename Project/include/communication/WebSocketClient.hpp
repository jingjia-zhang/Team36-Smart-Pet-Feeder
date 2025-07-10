#pragma once
#include <functional>
#include <string>
#include <memory>
#include <atomic>

class WebSocketClient {
public:
    using MessageHandler = std::function<void(const std::string&)>;
    using ErrorHandler = std::function<void(const std::string&)>;

    WebSocketClient();
    ~WebSocketClient();

    void connect(const std::string& url, bool auto_reconnect = false);
    void disconnect();
    void send(const std::string& message);

    void setMessageHandler(MessageHandler handler);
    void setErrorHandler(ErrorHandler handler);
    bool isConnected() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};