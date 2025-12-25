#ifndef __WEBSOCKET_MANAGER_H__
#define __WEBSOCKET_MANAGER_H__

#include <string>
#include <functional>
#include "cocos2d.h"
#include "network/WebSocket.h"

USING_NS_CC;
using namespace network;

class WebSocketManager : public Ref, public WebSocket::Delegate {
public:
    // 定义回调函数类型
    typedef std::function<void()> ConnectionCallback;
    typedef std::function<void(const std::string& message)> MessageCallback;
    typedef std::function<void(WebSocket::ErrorCode errorCode)> ErrorCallback;
    typedef std::function<void()> DisconnectionCallback;

    // 获取单例实例
    static WebSocketManager* getInstance();

    // 销毁单例实例
    static void destroyInstance();

    // 连接到WebSocket服务器
    bool connect(const std::string& url);

    // 断开WebSocket连接
    void disconnect();

    // 发送消息到服务器
    bool send(const std::string& message);

    // 设置回调函数
    void setOnOpenCallback(const ConnectionCallback& callback);
    void setOnMessageCallback(const MessageCallback& callback);
    void setOnErrorCallback(const ErrorCallback& callback);
    void setOnCloseCallback(const DisconnectionCallback& callback);

    // 获取连接状态
    WebSocket::State getReadyState() const;

    // WebSocket::Delegate 方法实现
    virtual void onOpen(WebSocket* ws) override;
    virtual void onMessage(WebSocket* ws, const WebSocket::Data& data) override;
    virtual void onClose(WebSocket* ws) override;
    virtual void onError(WebSocket* ws, const WebSocket::ErrorCode& error) override;

private:
    // 构造函数
    WebSocketManager();

    // 析构函数
    ~WebSocketManager();

    // 初始化
    bool init();

    static WebSocketManager* instance;

    WebSocket* _webSocket;
    std::string _url;

    // 回调函数实例
    ConnectionCallback _onOpenCallback;
    MessageCallback _onMessageCallback;
    ErrorCallback _onErrorCallback;
    DisconnectionCallback _onCloseCallback;
};

#endif // __WEBSOCKET_MANAGER_H__