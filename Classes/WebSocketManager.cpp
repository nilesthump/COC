#include "WebSocketManager.h"

// 初始化单例实例
WebSocketManager* WebSocketManager::instance = nullptr;

WebSocketManager::WebSocketManager() :
    _webSocket(nullptr),
    _url(""),
    _onOpenCallback(nullptr),
    _onMessageCallback(nullptr),
    _onErrorCallback(nullptr),
    _onCloseCallback(nullptr) {
}

WebSocketManager::~WebSocketManager() {
    disconnect();
    CC_SAFE_DELETE(_webSocket);
}

WebSocketManager* WebSocketManager::getInstance() {
    if (instance == nullptr) {
        instance = new WebSocketManager();
        instance->init();
    }
    return instance;
}

void WebSocketManager::destroyInstance() {
    CC_SAFE_DELETE(instance);
}

bool WebSocketManager::init() {
    return true;
}

bool WebSocketManager::connect(const std::string& url) {
    if (_webSocket != nullptr) {
        disconnect();
        CC_SAFE_DELETE(_webSocket);
    }

    _webSocket = new WebSocket();
    _url = url;

    if (!_webSocket->init(*this, url)) {
        CC_SAFE_DELETE(_webSocket);
        _webSocket = nullptr;
        return false;
    }

    return true;
}

void WebSocketManager::disconnect() {
    if (_webSocket != nullptr) {
        _webSocket->close();
    }
}

bool WebSocketManager::send(const std::string& message) {
    if (_webSocket == nullptr || _webSocket->getReadyState() != WebSocket::State::OPEN) {
        return false;
    }

    _webSocket->send(message);
    return true;
}

void WebSocketManager::setOnOpenCallback(const ConnectionCallback& callback) {
    _onOpenCallback = callback;
}

void WebSocketManager::setOnMessageCallback(const MessageCallback& callback) {
    _onMessageCallback = callback;
}

void WebSocketManager::setOnErrorCallback(const ErrorCallback& callback) {
    _onErrorCallback = callback;
}

void WebSocketManager::setOnCloseCallback(const DisconnectionCallback& callback) {
    _onCloseCallback = callback;
}

WebSocket::State WebSocketManager::getReadyState() const {
    if (_webSocket == nullptr) {
        return WebSocket::State::CLOSED;
    }
    return _webSocket->getReadyState();
}

void WebSocketManager::onOpen(WebSocket* ws) {
    CCLOG("WebSocket connected to %s", _url.c_str());
    if (_onOpenCallback) {
        _onOpenCallback();
    }
}

void WebSocketManager::onMessage(WebSocket* ws, const WebSocket::Data& data) {
    std::string message(data.bytes, data.len);
    CCLOG("Received WebSocket message: %s", message.c_str());
    if (_onMessageCallback) {
        _onMessageCallback(message);
    }
}

void WebSocketManager::onClose(WebSocket* ws) {
    CCLOG("WebSocket disconnected from %s", _url.c_str());
    if (_onCloseCallback) {
        _onCloseCallback();
    }

    CC_SAFE_DELETE(_webSocket);
    _webSocket = nullptr;
}

void WebSocketManager::onError(WebSocket* ws, const WebSocket::ErrorCode& error) {
    CCLOG("WebSocket error: %d", static_cast<int>(error));
    if (_onErrorCallback) {
        _onErrorCallback(error);
    }

    CC_SAFE_DELETE(_webSocket);
    _webSocket = nullptr;
}