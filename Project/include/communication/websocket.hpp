#pragma once
#include <functional>
#include <string>
#include <memory>

class WebSocketClient {
public:
    using MessageHandler = std::function<void(const std::string&)>;
    
    WebSocketClient();
    ~WebSocketClient();
    
    void connect(const std::string& url);
    void disconnect();
    void send(const std::string& message);
    
    void setMessageHandler(MessageHandler handler);
    bool isConnected() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};