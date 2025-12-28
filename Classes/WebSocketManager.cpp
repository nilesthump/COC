#include "WebSocketManager.h"
#include <mutex>
#include <deque>
#include <atomic>
#include "cocos2d.h"

// 初始化单例实例
WebSocketManager* WebSocketManager::instance = nullptr;

WebSocketManager::WebSocketManager() :
    _webSocket(nullptr),
    _url(""),
    _onOpenCallback(nullptr),
    _onMessageCallback(nullptr),
    _onErrorCallback(nullptr),
    _onCloseCallback(nullptr),
    _isProcessingMessages(false),
    _destroyed(false) {
}

WebSocketManager::~WebSocketManager() {
    _destroyed = true;
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

    syncProductionData();
    syncUpgradeStatus();
}

void WebSocketManager::onMessage(WebSocket* ws, const WebSocket::Data& data) {
    if (_destroyed) {
        CCLOG("WebSocketManager: onMessage ignored - object destroyed");
        return;
    }

    std::string message(data.bytes, data.len);
    CCLOG("WebSocketManager: onMessage received (len=%d): %.100s...", data.len, message.c_str());

    std::lock_guard<std::mutex> lock(_messageQueueMutex);
    _messageQueue.push_back(message);
    CCLOG("WebSocketManager: Message queued, queue size=%zu", _messageQueue.size());

    if (!_isProcessingMessages) {
        _isProcessingMessages = true;
        CCLOG("WebSocketManager: Scheduling message processing");
        Director::getInstance()->getScheduler()->schedule([this](float dt) {
            if (!_destroyed) {
                processMessageQueue();
            }
            }, this, 0.0f, 0, 0.0f, false, "processQueue");
    }
}

void WebSocketManager::processMessageQueue() {
    if (_destroyed) {
        CCLOG("WebSocketManager: processMessageQueue ignored - object destroyed");
        _isProcessingMessages = false;
        return;
    }

    if (_messageQueue.empty()) {
        CCLOG("WebSocketManager: Message queue empty, stopping processing");
        _isProcessingMessages = false;
        return;
    }

    std::string message;
    {
        std::lock_guard<std::mutex> lock(_messageQueueMutex);
        if (_destroyed || _messageQueue.empty()) {
            _isProcessingMessages = false;
            return;
        }
        message = _messageQueue.front();
        _messageQueue.pop_front();
    }

    if (_destroyed) {
        _isProcessingMessages = false;
        return;
    }

    CCLOG("WebSocketManager: Processing message, remaining queue=%zu", _messageQueue.size());

    bool isProductionData = (message.find("\"type\":\"production_data\"") != std::string::npos ||
        message.find("\"action\":\"getBuildingProduction\"") != std::string::npos);

    bool isUpgradeStatus = (message.find("\"action\":\"upgradeStatus\"") != std::string::npos ||
        message.find("\"type\":\"upgrade_status\"") != std::string::npos);

    bool isUpgradeComplete = (message.find("\"action\":\"upgradeComplete\"") != std::string::npos);

    bool isBuildingsData = (message.find("\"action\":\"getBuildings\"") != std::string::npos);

    CCLOG("WebSocketManager: isProductionData=%s, isUpgradeStatus=%s, isUpgradeComplete=%s, isBuildingsData=%s",
        isProductionData ? "true" : "false",
        isUpgradeStatus ? "true" : "false",
        isUpgradeComplete ? "true" : "false",
        isBuildingsData ? "true" : "false");

    if (isProductionData) {
        CCLOG("WebSocketManager: Detected production data message");
        handleProductionDataMessage(message);
    }

    if (isUpgradeStatus) {
        CCLOG("WebSocketManager: Detected upgrade status message");
        handleUpgradeStatusMessage(message);
    }

    if (isUpgradeComplete) {
        CCLOG("WebSocketManager: Detected upgrade complete message");
        handleUpgradeCompleteMessage(message);
    }

    if (message.find("\"action\":\"startUpgrade\"") != std::string::npos) {
        CCLOG("WebSocketManager: Detected startUpgrade response");

        size_t durationPos = message.find("\"duration\":");
        if (durationPos != std::string::npos) {
            size_t start = durationPos + 10;
            size_t end = message.find(",", start);
            if (end == std::string::npos) {
                end = message.find("}", start);
            }
            if (end != std::string::npos) {
                int duration = std::stoi(message.substr(start, end - start));

                if (duration == 0) {
                    CCLOG("WebSocketManager: Instant upgrade detected, auto-completing");

                    std::string buildingType = "";
                    float x = 0.0f, y = 0.0f;

                    size_t typePos = message.find("\"buildingType\":\"");
                    if (typePos != std::string::npos) {
                        size_t startType = typePos + 15;
                        size_t endType = message.find("\"", startType);
                        if (endType != std::string::npos) {
                            buildingType = message.substr(startType, endType - startType);
                        }
                    }

                    size_t xPos = message.find("\"x\":");
                    if (xPos != std::string::npos) {
                        size_t startX = xPos + 4;
                        size_t endX = message.find(",", startX);
                        if (endX == std::string::npos) {
                            endX = message.find("}", startX);
                        }
                        if (endX != std::string::npos) {
                            x = std::stof(message.substr(startX, endX - startX));
                        }
                    }

                    size_t yPos = message.find("\"y\":");
                    if (yPos != std::string::npos) {
                        size_t startY = yPos + 4;
                        size_t endY = message.find(",", startY);
                        if (endY == std::string::npos) {
                            endY = message.find("}", startY);
                        }
                        if (endY != std::string::npos) {
                            y = std::stof(message.substr(startY, endY - startY));
                        }
                    }

                    if (!buildingType.empty()) {
                        CCLOG("WebSocketManager: Auto-completing instant upgrade for %s at (%.1f, %.1f)",
                            buildingType.c_str(), x, y);
                        confirmUpgradeComplete(buildingType, x, y);
                    }
                }
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(_callbackMutex);
        if (_onMessageCallback) {
            CCLOG("WebSocketManager: Calling _onMessageCallback");
            try {
                _onMessageCallback(message);
                CCLOG("WebSocketManager: _onMessageCallback completed successfully");
            }
            catch (const std::exception& e) {
                CCLOG("WebSocketManager: Exception in _onMessageCallback: %s", e.what());
            }
            catch (...) {
                CCLOG("WebSocketManager: Unknown exception in _onMessageCallback");
            }
        }
        else {
            CCLOG("WebSocketManager: WARNING - _onMessageCallback is NULL!");
        }
    }

    if (_destroyed) {
        _isProcessingMessages = false;
        return;
    }

    Director::getInstance()->getScheduler()->schedule([this](float dt) {
        if (!_destroyed) {
            processMessageQueue();
        }
        }, this, 0.0f, 0, 0.0f, false, "processQueue");
}

void WebSocketManager::handleProductionDataMessage(const std::string& message) {
    parseProductionData(message);
}

void WebSocketManager::syncProductionData() {
    if (_webSocket == nullptr || _webSocket->getReadyState() != WebSocket::State::OPEN) {
        CCLOG("WebSocket not connected, cannot sync production data");
        return;
    }

    std::string username = SessionManager::getInstance()->getCurrentUsername();
    if (username.empty()) {
        CCLOG("No user logged in, cannot sync production data");
        return;
    }

    std::string request = "{\"action\":\"getBuildingProduction\",\"username\":\"" + username + "\"}";
    _webSocket->send(request);
    CCLOG("Requested production data sync for user: %s", username.c_str());
}

void WebSocketManager::parseProductionData(const std::string& jsonData) {
    CCLOG("Parsing production data: %s", jsonData.c_str());

    std::vector<ProductionData> productionList;

    size_t productionsPos = jsonData.find("\"productions\":[");
    if (productionsPos == std::string::npos) {
        CCLOG("No productions found in production data");
        return;
    }

    size_t currentPos = productionsPos + strlen("\"productions\":[");
    while (true) {
        size_t objStart = jsonData.find("{", currentPos);
        if (objStart == std::string::npos || objStart > jsonData.find("]", currentPos)) {
            break;
        }

        size_t objEnd = jsonData.find("}", objStart);
        if (objEnd == std::string::npos) {
            break;
        }

        std::string buildingObj = jsonData.substr(objStart, objEnd - objStart + 1);

        ProductionData data;
        data.buildingType = "";
        data.x = 0;
        data.y = 0;
        data.level = 1;
        data.currentStock = 0;
        data.lastProductionTime = 0;

        size_t typePos = buildingObj.find("\"type\":\"");
        if (typePos != std::string::npos) {
            size_t typeStart = typePos + strlen("\"type\":\"");
            size_t typeEnd = buildingObj.find("\"", typeStart);
            if (typeEnd != std::string::npos) {
                data.buildingType = buildingObj.substr(typeStart, typeEnd - typeStart);
            }
        }

        size_t xPos = buildingObj.find("\"x\":");
        if (xPos != std::string::npos) {
            size_t xEnd = buildingObj.find(",", xPos);
            std::string xStr = buildingObj.substr(xPos + 4, (xEnd != std::string::npos) ? (xEnd - xPos - 4) : 20);
            data.x = std::stof(xStr);
        }

        size_t yPos = buildingObj.find("\"y\":");
        if (yPos != std::string::npos) {
            size_t yEnd = buildingObj.find(",", yPos);
            std::string yStr = buildingObj.substr(yPos + 4, (yEnd != std::string::npos) ? (yEnd - yPos - 4) : 20);
            data.y = std::stof(yStr);
        }

        size_t levelPos = buildingObj.find("\"level\":");
        if (levelPos != std::string::npos) {
            size_t levelEnd = buildingObj.find(",", levelPos);
            std::string levelStr = buildingObj.substr(levelPos + 8, (levelEnd != std::string::npos) ? (levelEnd - levelPos - 8) : 10);
            data.level = std::stoi(levelStr);
        }

        size_t stockPos = buildingObj.find("\"currentStock\":");
        if (stockPos != std::string::npos) {
            size_t stockEnd = buildingObj.find(",", stockPos);
            std::string stockStr = buildingObj.substr(stockPos + 15, (stockEnd != std::string::npos) ? (stockEnd - stockPos - 15) : 10);
            data.currentStock = std::stoi(stockStr);
        }

        size_t timePos = buildingObj.find("\"lastProductionTime\":");
        if (timePos != std::string::npos) {
            size_t timeStart = timePos + 24;
            size_t braceEnd = buildingObj.rfind("}");
            size_t commaPos = buildingObj.find(",", timePos);
            size_t actualEnd = (commaPos != std::string::npos && commaPos < braceEnd) ? commaPos : braceEnd;

            if (timeStart < actualEnd && timeStart < buildingObj.length()) {
                size_t extractLength = std::min(actualEnd - timeStart, static_cast<size_t>(20));
                std::string timeStr = buildingObj.substr(timeStart, extractLength);
                size_t validEnd = timeStr.find_first_of(",}");
                if (validEnd != std::string::npos) {
                    timeStr = timeStr.substr(0, validEnd);
                }
                if (!timeStr.empty()) {
                    try {
                        data.lastProductionTime = std::stoll(timeStr);
                    }
                    catch (...) {
                        data.lastProductionTime = 0;
                    }
                }
            }
            else {
                data.lastProductionTime = 0;
            }
        }

        if (!data.buildingType.empty()) {
            productionList.push_back(data);
            CCLOG("Parsed production data: type=%s, x=%.1f, y=%.1f, level=%d, stock=%d, time=%lld",
                data.buildingType.c_str(), data.x, data.y, data.level, data.currentStock, data.lastProductionTime);
        }

        currentPos = objEnd + 1;
    }

    SessionManager::getInstance()->setProductionData(productionList);
    CCLOG("Saved %zu production data entries to SessionManager", productionList.size());
    }

    void WebSocketManager::sendUpgradeStart(const std::string& buildingType, float x, float y, int targetLevel, int duration) {
        if (_webSocket == nullptr || _webSocket->getReadyState() != WebSocket::State::OPEN) {
            CCLOG("WebSocket not connected, cannot send upgrade start");
            return;
        }

        std::string username = SessionManager::getInstance()->getCurrentUsername();
        if (username.empty()) {
            CCLOG("No user logged in, cannot send upgrade start");
            return;
        }

        std::string request = StringUtils::format(
            "{\"action\":\"startUpgrade\",\"username\":\"%s\",\"buildingType\":\"%s\",\"x\":%.1f,\"y\":%.1f,\"targetLevel\":%d,\"duration\":%d}",
            username.c_str(), buildingType.c_str(), x, y, targetLevel, duration);

        _webSocket->send(request);
        CCLOG("Sent upgrade start request: %s", request.c_str());
    }

    void WebSocketManager::syncUpgradeStatus() {
        if (_webSocket == nullptr || _webSocket->getReadyState() != WebSocket::State::OPEN) {
            CCLOG("WebSocket not connected, cannot sync upgrade status");
            return;
        }

        std::string username = SessionManager::getInstance()->getCurrentUsername();
        if (username.empty()) {
            CCLOG("No user logged in, cannot sync upgrade status");
            return;
        }

        std::string request = "{\"action\":\"getUpgradeStatus\",\"username\":\"" + username + "\"}";
        _webSocket->send(request);
        CCLOG("Requested upgrade status sync for user: %s", username.c_str());
    }

    void WebSocketManager::handleUpgradeStatusMessage(const std::string& message) {
        parseUpgradeStatus(message);
    }

    void WebSocketManager::parseUpgradeStatus(const std::string& jsonData) {
        CCLOG("Parsing upgrade status: %s", jsonData.c_str());

        std::vector<UpgradeData> upgradeList;

        size_t upgradesPos = jsonData.find("\"upgrades\":[");
        if (upgradesPos == std::string::npos) {
            CCLOG("No upgrades found in upgrade status data");
            SessionManager::getInstance()->setUpgradeData(upgradeList);
            return;
        }

        size_t currentPos = upgradesPos + strlen("\"upgrades\":[");
        while (true) {
            size_t objStart = jsonData.find("{", currentPos);
            if (objStart == std::string::npos || objStart > jsonData.find("]", currentPos)) {
                break;
            }

            size_t objEnd = jsonData.find("}", objStart);
            if (objEnd == std::string::npos) {
                break;
            }

            std::string upgradeObj = jsonData.substr(objStart, objEnd - objStart + 1);

            UpgradeData data;
            data.buildingType = "";
            data.x = 0;
            data.y = 0;
            data.targetLevel = 1;
            data.startTime = 0;
            data.duration = 0;
            data.remainingTime = 0;

            size_t typePos = upgradeObj.find("\"type\":\"");
            if (typePos != std::string::npos) {
                size_t typeStart = typePos + strlen("\"type\":\"");
                size_t typeEnd = upgradeObj.find("\"", typeStart);
                if (typeEnd != std::string::npos) {
                    data.buildingType = upgradeObj.substr(typeStart, typeEnd - typeStart);
                }
            }

            size_t xPos = upgradeObj.find("\"x\":");
            if (xPos != std::string::npos) {
                size_t xEnd = upgradeObj.find(",", xPos);
                std::string xStr = upgradeObj.substr(xPos + 4, (xEnd != std::string::npos) ? (xEnd - xPos - 4) : 20);
                data.x = std::stof(xStr);
            }

            size_t yPos = upgradeObj.find("\"y\":");
            if (yPos != std::string::npos) {
                size_t yEnd = upgradeObj.find(",", yPos);
                std::string yStr = upgradeObj.substr(yPos + 4, (yEnd != std::string::npos) ? (yEnd - yPos - 4) : 20);
                data.y = std::stof(yStr);
            }

            size_t targetLevelPos = upgradeObj.find("\"targetLevel\":");
            if (targetLevelPos != std::string::npos) {
                size_t levelEnd = upgradeObj.find(",", targetLevelPos);
                std::string levelStr = upgradeObj.substr(targetLevelPos + 14, (levelEnd != std::string::npos) ? (levelEnd - targetLevelPos - 14) : 10);
                data.targetLevel = std::stoi(levelStr);
            }

            size_t startTimePos = upgradeObj.find("\"startTime\":");
            if (startTimePos != std::string::npos) {
                size_t timeStart = startTimePos + 12;
                size_t braceEnd = upgradeObj.rfind("}");
                size_t commaPos = upgradeObj.find(",", startTimePos);
                size_t actualEnd = (commaPos != std::string::npos && commaPos < braceEnd) ? commaPos : braceEnd;

                if (timeStart < actualEnd && timeStart < upgradeObj.length()) {
                    size_t extractLength = std::min(actualEnd - timeStart, static_cast<size_t>(20));
                    std::string timeStr = upgradeObj.substr(timeStart, extractLength);
                    size_t validEnd = timeStr.find_first_of(",}");
                    if (validEnd != std::string::npos) {
                        timeStr = timeStr.substr(0, validEnd);
                    }
                    if (!timeStr.empty()) {
                        try {
                            data.startTime = std::stoll(timeStr);
                        }
                        catch (...) {
                            data.startTime = 0;
                        }
                    }
                }
                else {
                    data.startTime = 0;
                }
            }

            size_t durationPos = upgradeObj.find("\"duration\":");
            if (durationPos != std::string::npos) {
                size_t durationStart = durationPos + 11;
                size_t braceEnd = upgradeObj.rfind("}");
                size_t commaPos = upgradeObj.find(",", durationPos);
                size_t actualEnd = (commaPos != std::string::npos && commaPos < braceEnd) ? commaPos : braceEnd;

                if (durationStart < actualEnd && durationStart < upgradeObj.length()) {
                    size_t extractLength = std::min(actualEnd - durationStart, static_cast<size_t>(20));
                    std::string durationStr = upgradeObj.substr(durationStart, extractLength);
                    size_t validEnd = durationStr.find_first_of(",}");
                    if (validEnd != std::string::npos) {
                        durationStr = durationStr.substr(0, validEnd);
                    }
                    if (!durationStr.empty()) {
                        try {
                            data.duration = std::stoi(durationStr);
                        }
                        catch (...) {
                            data.duration = 0;
                        }
                    }
                }
                else {
                    data.duration = 0;
                }
            }

            size_t remainingTimePos = upgradeObj.find("\"remainingTime\":");
            if (remainingTimePos != std::string::npos) {
                size_t remainingStart = remainingTimePos + 16;
                size_t braceEnd = upgradeObj.rfind("}");
                size_t commaPos = upgradeObj.find(",", remainingTimePos);
                size_t actualEnd = (commaPos != std::string::npos && commaPos < braceEnd) ? commaPos : braceEnd;

                if (remainingStart < actualEnd && remainingStart < upgradeObj.length()) {
                    size_t extractLength = std::min(actualEnd - remainingStart, static_cast<size_t>(20));
                    std::string remainingStr = upgradeObj.substr(remainingStart, extractLength);
                    size_t validEnd = remainingStr.find_first_of(",}");
                    if (validEnd != std::string::npos) {
                        remainingStr = remainingStr.substr(0, validEnd);
                    }
                    if (!remainingStr.empty()) {
                        try {
                            data.remainingTime = std::stoi(remainingStr);
                        }
                        catch (...) {
                            data.remainingTime = 0;
                        }
                    }
                }
                else {
                    data.remainingTime = 0;
                }
            }

            if (!data.buildingType.empty()) {
                upgradeList.push_back(data);
                CCLOG("Parsed upgrade status: type=%s, x=%.1f, y=%.1f, targetLevel=%d, remaining=%ds",
                    data.buildingType.c_str(), data.x, data.y, data.targetLevel, data.remainingTime);
            }

            currentPos = objEnd + 1;
        }

        SessionManager::getInstance()->setUpgradeData(upgradeList);
        CCLOG("Saved %zu upgrade data entries to SessionManager", upgradeList.size());
    }

    void WebSocketManager::confirmUpgradeComplete(const std::string& buildingType, float x, float y) {
        if (_webSocket == nullptr || _webSocket->getReadyState() != WebSocket::State::OPEN) {
            CCLOG("WebSocket not connected, cannot confirm upgrade complete");
            return;
        }

        std::string username = SessionManager::getInstance()->getCurrentUsername();
        if (username.empty()) {
            CCLOG("No user logged in, cannot confirm upgrade complete");
            return;
        }

        std::string request = StringUtils::format(
            "{\"action\":\"completeUpgrade\",\"username\":\"%s\",\"buildingType\":\"%s\",\"x\":%.1f,\"y\":%.1f}",
            username.c_str(), buildingType.c_str(), x, y);

        _webSocket->send(request);
        CCLOG("Sent upgrade complete confirmation: %s", request.c_str());
    }

    void WebSocketManager::handleUpgradeCompleteMessage(const std::string& message) {
        parseUpgradeComplete(message);
    }

    void WebSocketManager::parseUpgradeComplete(const std::string& jsonData) {
        CCLOG("Parsing upgrade complete: %s", jsonData.c_str());

        std::string buildingType = "";
        float x = 0.0f;
        float y = 0.0f;
        int newLevel = 1;

        size_t buildingTypePos = jsonData.find("\"buildingType\":\"");
        if (buildingTypePos != std::string::npos) {
            size_t start = buildingTypePos + 16;
            size_t end = jsonData.find("\"", start);
            if (end != std::string::npos) {
                buildingType = jsonData.substr(start, end - start);
            }
        }

        size_t xPos = jsonData.find("\"x\":");
        if (xPos != std::string::npos) {
            size_t start = xPos + 4;
            size_t end = jsonData.find(",", start);
            if (end == std::string::npos) {
                end = jsonData.find("}", start);
            }
            if (end != std::string::npos) {
                std::string xStr = jsonData.substr(start, end - start);
                x = std::stof(xStr);
            }
        }

        size_t yPos = jsonData.find("\"y\":");
        if (yPos != std::string::npos) {
            size_t start = yPos + 4;
            size_t end = jsonData.find(",", start);
            if (end == std::string::npos) {
                end = jsonData.find("}", start);
            }
            if (end != std::string::npos) {
                std::string yStr = jsonData.substr(start, end - start);
                y = std::stof(yStr);
            }
        }

        size_t newLevelPos = jsonData.find("\"newLevel\":");
        if (newLevelPos != std::string::npos) {
            size_t start = newLevelPos + 11;
            size_t end = jsonData.find(",", start);
            if (end == std::string::npos) {
                end = jsonData.find("}", start);
            }
            if (end != std::string::npos) {
                std::string levelStr = jsonData.substr(start, end - start);
                newLevel = std::stoi(levelStr);
            }
        }

        if (!buildingType.empty()) {
            CCLOG("Upgrade completed: buildingType=%s, x=%.1f, y=%.1f, newLevel=%d",
                buildingType.c_str(), x, y, newLevel);

            Director::getInstance()->getScheduler()->performFunctionInCocosThread([buildingType, x, y, newLevel]() {
                EventCustom event("upgrade_complete");
                event.setUserData(new UpgradeCompleteData(buildingType, x, y, newLevel));
                Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
                });
        }
    }

    UpgradeCompleteData::UpgradeCompleteData(const std::string& type, float px, float py, int level)
        : buildingType(type), x(px), y(py), newLevel(level) {
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