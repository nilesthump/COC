#include <iostream>
#include <string>
#include <vector>
#include <libwebsockets.h>
#include "sqlite3.h"
#include <sstream>
#include <map>
#include <mutex>
#include <thread>
#include <chrono>
#include <ctime>

// 函数原型声明
std::map<std::string, std::string> parseJSON(const std::string& jsonStr);
bool registerUser(const std::string& username, const std::string& password);
bool loginUser(const std::string& username, const std::string& password);
bool deleteUser(const std::string& username);
bool changePassword(const std::string& username, const std::string& newPassword);
bool initDatabase();
bool isUserLoggedIn(const std::string& username);
void logoutUser(const std::string& username);
bool getResource(const std::string& username, int& gold, int& elixir, int& gems);
bool updateResource(const std::string& username, int gold, int elixir, int gems);
bool saveBuilding(const std::string& username, const std::string& buildingType, float x, float y, int level,
    int hp = 100, int maxHp = 100, int productionRate = 1, int maxStock = 100, int attack = 0);
bool getBuildings(const std::string& username, std::string& buildingsJson);
bool deleteBuilding(const std::string& username, float x, float y);
bool updateBuildingProduction(const std::string& username, const std::string& buildingType,
    float x, float y, int currentStock, long long lastProductionTime);
bool getBuildingProduction(const std::string& username, std::string& productionJson);
bool addOfflineProduction(const std::string& username);
bool collectProduction(const std::string& username, const std::string& buildingType,
    float x, float y, int remainingStock);
bool startBuildingUpgrade(const std::string& username, const std::string& buildingType,
    float x, float y, int targetLevel, int duration);
bool getBuildingUpgradeStatus(const std::string& username, std::string& upgradesJson);
bool finishBuildingUpgrade(const std::string& username, const std::string& buildingType,
    float x, float y);
bool broadcastUpgradeComplete(const std::string& username, const std::string& buildingType,
    float x, float y, int newLevel);
void processCompletedUpgrades();
bool updateBuildingLevel(const std::string& username, const std::string& buildingType,
    float x, float y, int newLevel);

// 客户端连接结构
struct ClientConnection {
    struct lws* wsi;
    bool isConnected;
    char username[64]; // 当前登录的用户名（如果是游客则为""）
    bool isLoggedIn; // 是否已登录
};

// 服务器状态
struct ServerState {
    std::vector<ClientConnection*> clients;
    bool isRunning;
    sqlite3* db; // SQLite数据库连接
    std::mutex dbMutex; // 数据库访问互斥锁
    std::mutex clientsMutex; // 客户端列表访问互斥锁
};

static ServerState serverState;

// 转义JSON字符串中的特殊字符，确保JSON格式正确
std::string escapeJSONString(const std::string& input) {
    std::string output;
    for (char c : input) {
        switch (c) {
            case '"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default: output += c; break;
        }
    }
    return output;
}

// WebSocket服务器回调函数，处理客户端连接和消息
static int callback_server(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len) {
    try {
        ClientConnection* client = (ClientConnection*)user;

        switch (reason) {
            case LWS_CALLBACK_ESTABLISHED:
            {
                char client_ip[128] = { 0 };
                int client_port = 0;

                // 获取客户端IP地址
#ifdef _WIN32
                SOCKET sock = lws_get_socket_fd(wsi);
#else
                int sock = lws_get_socket_fd(wsi);
#endif

                if (sock >= 0) {
                    struct sockaddr_storage addr;
                    socklen_t addr_len = sizeof(addr);
                    if (getpeername(sock, (struct sockaddr*)&addr, &addr_len) == 0) {
                        if (addr.ss_family == AF_INET) {
                            struct sockaddr_in* s = (struct sockaddr_in*)&addr;
                            inet_ntop(AF_INET, &s->sin_addr, client_ip, sizeof(client_ip));
                            client_port = ntohs(s->sin_port);
                        }
                        else if (addr.ss_family == AF_INET6) {
                            struct sockaddr_in6* s = (struct sockaddr_in6*)&addr;
                            inet_ntop(AF_INET6, &s->sin6_addr, client_ip, sizeof(client_ip));
                            client_port = ntohs(s->sin6_port);
                        }
                    }
                }

                if (client_ip[0] != '\0') {
                    std::cout << "Client connected from IP: " << client_ip << ", Port: " <<
                        client_port << std::endl;
                }
                else {
                    std::cout << "Client connected (unable to get IP address)" << std::endl;
                }

                client->isConnected = true;
                client->wsi = wsi;
                client->username[0] = '\0';
                client->isLoggedIn = false;
                {
                    std::lock_guard<std::mutex> lock(serverState.clientsMutex);
                    serverState.clients.push_back(client);
                }
                break;
            }

            case LWS_CALLBACK_RECEIVE:
            {
                std::string receivedData((char*)in, len);
                std::cout << "Received data: " << receivedData << std::endl;

                // 解析JSON数据
                std::map<std::string, std::string> jsonData;
                try {
                    jsonData = parseJSON(receivedData);
                }
                catch (const std::exception& e) {
                    std::cerr << "JSON parse error: " << e.what() << std::endl;
                    break;
                }

                if (jsonData.find("action") == jsonData.end()) {
                    std::cerr << "Missing action in JSON" << std::endl;
                    break;
                }

                std::string action = jsonData["action"];
                bool result = false;
                std::string message = "";
                std::string jsonResponse;

                // 根据action执行相应的操作
                if (action == "register") {
                    std::string username = jsonData["username"];
                    std::string password = jsonData["password"];
                    if (registerUser(username, password)) {
                        result = true;
                        message = u8"注册成功";
                    }
                    else {
                        result = false;
                        message = u8"用户名已存在";
                    }
                }
                else if (action == "login") {
                    std::string username = jsonData["username"];
                    std::string password = jsonData["password"];

                    // 检测是否是游客登录（用户名为空或特定标识）
                    bool isGuestLogin = username.empty();

                    if (!isGuestLogin) {
                        // 账号密码登录，检测是否已登录
                        if (isUserLoggedIn(username)) {
                            result = false;
                            message = u8"账号已登录，请先登出";
                        }
                        else if (loginUser(username, password)) {
                            result = true;
                            message = u8"登录成功";
                            // 更新客户端的登录状态
                            strncpy(client->username, username.c_str(), sizeof(client->username) - 1);
                            client->username[sizeof(client->username) - 1] = '\0';
                            client->isLoggedIn = true;
                            std::cout << "User " << username << " logged in, client->username set to: '" <<
                                client->username << "'" << std::endl;
                        }
                        else {
                            result = false;
                            message = u8"用户名或密码错误";
                        }
                    }
                    else {
                        // 游客登录，不检测登录状态
                        result = true;
                        message = u8"游客登录成功";
                        client->username[0] = '\0';
                        client->isLoggedIn = false;
                        std::cout << "Guest logged in" << std::endl;
                    }
                }
                else if (action == "delete") {
                    std::string username = jsonData["username"];
                    if (deleteUser(username)) {
                        result = true;
                        message = u8"账号删除成功";
                    }
                    else {
                        result = false;
                        message = u8"账号不存在";
                    }
                }
                else if (action == "changePassword") {
                    std::string username = jsonData["username"];
                    std::string newPassword = jsonData["newPassword"];
                    if (changePassword(username, newPassword)) {
                        result = true;
                        message = u8"密码修改成功";
                    }
                    else {
                        result = false;
                        message = u8"账号不存在";
                    }
                }
                // 验证旧密码
                else if (action == "verifyPassword") {
                    std::string username = jsonData["username"];
                    std::string password = jsonData["password"];
                    if (loginUser(username, password)) {
                        result = true;
                        message = u8"验证通过";
                    }
                    else {
                        result = false;
                        message = u8"密码错误";
                    }
                }
                else if (action == "logout") {
                    std::string username = jsonData["username"];

                    std::cout << "Logout request received - client username: '" << client->username <<
                        "', client isLoggedIn: " << client->isLoggedIn << ", requested username: '" <<
                        username << "'" << std::endl;

                    if (client->isLoggedIn && strcmp(client->username, username.c_str()) == 0) {
                        client->isLoggedIn = false;
                        client->username[0] = '\0';
                        result = true;
                        message = u8"退出登录成功";
                        std::cout << "User " << username << " logged out" << std::endl;
                    }
                    else {
                        result = false;
                        message = u8"未登录或账号不匹配";
                    }
                }
                else if (action == "getResource") {
                    std::string username = jsonData["username"];

                    std::cout << "getResource request received - username: '" << username << "'" << std::endl;

                    if (username.empty()) {
                        result = false;
                        message = u8"用户名为空";

                        // 构建发送给客户端的JSON响应
                        jsonResponse = "{\"action\":\"getResource\", \"result\":false, \"message\":\"" +
                            escapeJSONString(message) + "\"}";

                        std::cout << "Sending response: " << jsonResponse << std::endl;

                        // 发送响应数据
                        std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
                        memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
                        lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);
                    }
                    else {
                        int gold = 750;
                        int elixir = 750;
                        int gems = 15;
                        bool found = getResource(username, gold, elixir, gems);

                        result = true;
                        message = found ? u8"获取资源成功" : u8"使用默认资源值";

                        // 构建包含资源数据的JSON响应
                        jsonResponse = "{\"action\":\"getResource\", \"result\":true, \"message\":\"" +
                            escapeJSONString(message) +
                            "\", \"gold\":" + std::to_string(gold) +
                            ", \"elixir\":" + std::to_string(elixir) +
                            ", \"gems\":" + std::to_string(gems) + "}";

                        std::cout << "Sending resource response for user " << username << ": gold=" << gold
                            << ", elixir=" << elixir << ", gems=" << gems << std::endl;
                        std::cout << "Full response: " << jsonResponse << std::endl;

                        // 发送响应数据
                        std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
                        memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
                        lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);
                    }
                    // 跳过其他代码
                    goto skip_send_response;
                }
                else if (action == "updateResource") {
                    std::string username = jsonData["username"];
                    int gold = jsonData.count("gold") ? std::stoi(jsonData["gold"]) : 0;
                    int elixir = jsonData.count("elixir") ? std::stoi(jsonData["elixir"]) : 0;
                    int gems = jsonData.count("gems") ? std::stoi(jsonData["gems"]) : 0;

                    if (username.empty()) {
                        result = false;
                        message = u8"用户名为空";
                    }
                    else if (updateResource(username, gold, elixir, gems)) {
                        result = true;
                        message = u8"资源更新成功";
                    }
                    else {
                        result = false;
                        message = u8"资源更新失败";
                    }
                }
                else if (action == "saveBuilding") {
                    std::string username = jsonData["username"];
                    std::string buildingType = jsonData["buildingType"];
                    float x = jsonData.count("x") ? std::stof(jsonData["x"]) : 0.0f;
                    float y = jsonData.count("y") ? std::stof(jsonData["y"]) : 0.0f;
                    int level = jsonData.count("level") ? std::stoi(jsonData["level"]) : 1;

                    std::cout << "saveBuilding request received - username: " << username
                        << ", type: " << buildingType << ", x: " << x << ", y: " << y
                        << ", level: " << level << std::endl;

                    if (username.empty()) {
                        result = false;
                        message = u8"用户名为空";
                    }
                    else if (saveBuilding(username, buildingType, x, y, level)) {
                        result = true;
                        message = u8"建筑保存成功";
                    }
                    else {
                        result = false;
                        message = u8"建筑保存失败";
                    }
                }
                else if (action == "deleteBuilding") {
                    std::string username = jsonData["username"];
                    float x = jsonData.count("x") ? std::stof(jsonData["x"]) : 0.0f;
                    float y = jsonData.count("y") ? std::stof(jsonData["y"]) : 0.0f;

                    std::cout << "deleteBuilding request received - username: " << username
                        << ", x: " << x << ", y: " << y << std::endl;

                    if (username.empty()) {
                        result = false;
                        message = u8"用户名为空";
                    }
                    else if (deleteBuilding(username, x, y)) {
                        result = true;
                        message = u8"建筑删除成功";
                    }
                    else {
                        result = false;
                        message = u8"建筑删除失败";
                    }
                }
                else if (action == "getBuildings") {
                    std::string username = jsonData["username"];

                    std::cout << "getBuildings request received - username: " << username << std::endl;

                    if (username.empty()) {
                        result = false;
                        message = u8"用户名为空";
                        jsonResponse = "{\"action\":\"getBuildings\", \"result\":false, \"message\":\"" +
                            escapeJSONString(message) + "\", \"buildings\":\"\"}";

                        std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
                        memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
                        lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);
                    }
                    else {
                        std::string buildingsJson;
                        bool success = getBuildings(username, buildingsJson);

                        result = true;
                        message = success ? u8"获取建筑成功" : u8"建筑数据为空";

                        jsonResponse = "{\"action\":\"getBuildings\", \"result\":true, \"message\":\"" +
                            escapeJSONString(message) + "\", \"buildings\":" + buildingsJson + "}";

                        std::cout << "Sending buildings response for user " << username << ": " << jsonResponse << std::endl;

                        std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
                        memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
                        lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);
                    }
                    goto skip_send_response;
                }
                else if (action == "getBuildingProduction") {
                    std::string username = jsonData["username"];

                    std::cout << "getBuildingProduction request received - username: " << username << std::endl;

                    if (username.empty()) {
                        result = false;
                        message = u8"用户名为空";
                        jsonResponse = "{\"action\":\"getBuildingProduction\", \"result\":false, \"message\":\"" +
                            escapeJSONString(message) + "\", \"productions\":\"\"}";

                        std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
                        memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
                        lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);
                    }
                    else {
                        addOfflineProduction(username);

                        std::string productionsJson;
                        bool success = getBuildingProduction(username, productionsJson);

                        result = true;
                        message = success ? u8"获取生产状态成功" : u8"生产数据为空";

                        jsonResponse = "{\"action\":\"getBuildingProduction\", \"result\":true, \"message\":\"" +
                            escapeJSONString(message) + "\", \"productions\":" + productionsJson + "}";

                        std::cout << "Sending production status for user " << username << ": " << jsonResponse << std::endl;

                        std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
                        memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
                        lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);
                    }
                    goto skip_send_response;
                }
                else if (action == "updateBuildingProduction") {
                    std::string username = jsonData["username"];
                    std::string buildingType = jsonData["buildingType"];
                    float x = jsonData.count("x") ? std::stof(jsonData["x"]) : 0.0f;
                    float y = jsonData.count("y") ? std::stof(jsonData["y"]) : 0.0f;
                    int currentStock = jsonData.count("currentStock") ? std::stoi(jsonData["currentStock"]) : 0;
                    long long lastProductionTime = jsonData.count("lastProductionTime") ? std::stoll(jsonData["lastProductionTime"]) : time(nullptr);

                    std::cout << "updateBuildingProduction request - username: " << username
                        << ", type: " << buildingType << ", x: " << x << ", y: " << y
                        << ", stock: " << currentStock << ", lastTime: " << lastProductionTime << std::endl;

                    if (username.empty()) {
                        result = false;
                        message = u8"用户名为空";
                    }
                    else if (updateBuildingProduction(username, buildingType, x, y, currentStock, lastProductionTime)) {
                        result = true;
                        message = u8"生产状态更新成功";
                    }
                    else {
                        result = false;
                        message = u8"生产状态更新失败";
                    }
                }
                else if (action == "collectProduction") {
                    std::string username = jsonData["username"];
                    std::string buildingType = jsonData["buildingType"];
                    float x = jsonData.count("x") ? std::stof(jsonData["x"]) : 0.0f;
                    float y = jsonData.count("y") ? std::stof(jsonData["y"]) : 0.0f;
                    int collectedAmount = jsonData.count("collectedAmount") ? std::stoi(jsonData["collectedAmount"]) : 0;
                    int remainingStock = jsonData.count("remainingStock") ? std::stoi(jsonData["remainingStock"]) : 0;
                    int resourceType = jsonData.count("resourceType") ? std::stoi(jsonData["resourceType"]) : 1;

                    std::cout << "collectProduction request - username: " << username
                        << ", type: " << buildingType << ", x: " << x << ", y: " << y
                        << ", collected: " << collectedAmount << ", remaining: " << remainingStock << std::endl;

                    if (username.empty()) {
                        result = false;
                        message = u8"用户名为空";
                    }
                    else if (collectProduction(username, buildingType, x, y, remainingStock)) {
                        result = true;
                        message = u8"资源收集成功";
                    }
                    else {
                        result = false;
                        message = u8"资源收集失败";
                    }
                }
                else if (action == "syncProduction") {
                    std::string username = jsonData["username"];

                    std::cout << "syncProduction request received - username: " << username << std::endl;

                    if (username.empty()) {
                        result = false;
                        message = u8"用户名为空";
                        jsonResponse = "{\"action\":\"syncProduction\", \"result\":false, \"message\":\"" +
                            escapeJSONString(message) + "\", \"productions\":\"\"}";

                        std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
                        memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
                        lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);
                    }
                    else {
                        addOfflineProduction(username);

                        std::string productionsJson;
                        bool success = getBuildingProduction(username, productionsJson);

                        result = true;
                        message = success ? u8"同步成功" : u8"同步数据为空";

                        jsonResponse = "{\"action\":\"syncProduction\", \"result\":true, \"message\":\"" +
                            escapeJSONString(message) + "\", \"productions\":" + productionsJson + "}";

                        std::cout << "Sending synced production for user " << username << ": " << jsonResponse << std::endl;

                        std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
                        memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
                        lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);
                    }
                    goto skip_send_response;
                }
                else if (action == "startUpgrade") {
                    std::string username = jsonData["username"];
                    std::string buildingType = jsonData["buildingType"];
                    float x = jsonData.count("x") ? std::stof(jsonData["x"]) : 0.0f;
                    float y = jsonData.count("y") ? std::stof(jsonData["y"]) : 0.0f;
                    int targetLevel = jsonData.count("targetLevel") ? std::stoi(jsonData["targetLevel"]) : 1;
                    int duration = jsonData.count("duration") ? std::stoi(jsonData["duration"]) : 0;

                    std::cout << "startUpgrade request - username: " << username
                        << ", type: " << buildingType << ", x: " << x << ", y: " << y
                        << ", targetLevel: " << targetLevel << ", duration: " << duration << "s" << std::endl;

                    if (username.empty()) {
                        result = false;
                        message = u8"用户名为空";
                    }
                    else if (duration <= 0) {
                        std::cout << "Processing instant upgrade for " << buildingType << " at (" << x << ", " << y
                            << ") - target level: " << targetLevel << std::endl;

                        bool levelUpdated = updateBuildingLevel(username, buildingType, x, y, targetLevel);

                        if (levelUpdated) {
                            result = true;
                            message = u8"即时升级完成";
                            std::cout << "Instant upgrade completed for " << buildingType << " at (" << x << ", " << y
                                << ") - new level: " << targetLevel << std::endl;

                            jsonResponse = "{\"action\":\"startUpgrade\", \"result\":true, \"message\":\"" +
                                escapeJSONString(message) + "\", \"buildingType\":\"" + escapeJSONString(buildingType) +
                                "\", \"x\":" + std::to_string(x) + ", \"y\":" + std::to_string(y) +
                                ", \"targetLevel\":" + std::to_string(targetLevel) + ", \"duration\":0}";

                            std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
                            memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
                            lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);
                        }
                        else {
                            result = false;
                            message = u8"即时升级失败";
                            jsonResponse = "{\"action\":\"startUpgrade\", \"result\":false, \"message\":\"" +
                                escapeJSONString(message) + "\"}";

                            std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
                            memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
                            lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);
                        }
                        goto skip_send_response;
                    }
                    else if (startBuildingUpgrade(username, buildingType, x, y, targetLevel, duration)) {
                        result = true;
                        message = u8"升级已开始";
                        std::cout << "Upgrade started for " << buildingType << " at (" << x << ", " << y
                            << ") - target level: " << targetLevel << ", duration: " << duration << "s" << std::endl;
                    }
                    else {
                        result = false;
                        message = u8"升级开始失败，可能已有升级进行中";
                    }
                }
                else if (action == "getUpgradeStatus") {
                    std::string username = jsonData["username"];

                    std::cout << "getUpgradeStatus request - username: " << username << std::endl;

                    if (username.empty()) {
                        result = false;
                        message = u8"用户名为空";
                        jsonResponse = "{\"action\":\"getUpgradeStatus\", \"result\":false, \"message\":\"" +
                            escapeJSONString(message) + "\", \"upgrades\":\"\"}";

                        std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
                        memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
                        lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);
                    }
                    else {
                        processCompletedUpgrades();

                        std::string upgradesJson;
                        bool success = getBuildingUpgradeStatus(username, upgradesJson);

                        result = true;
                        message = success ? u8"获取升级状态成功" : u8"无进行中的升级";

                        jsonResponse = "{\"action\":\"getUpgradeStatus\", \"result\":true, \"message\":\"" +
                            escapeJSONString(message) + "\", \"upgrades\":" + upgradesJson + "}";

                        std::cout << "Sending upgrade status for user " << username << ": " << jsonResponse << std::endl;

                        std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
                        memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
                        lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);
                    }
                    goto skip_send_response;
                }
                else if (action == "completeUpgrade") {
                    std::string username = jsonData["username"];
                    std::string buildingType = jsonData["buildingType"];
                    float x = jsonData.count("x") ? std::stof(jsonData["x"]) : 0.0f;
                    float y = jsonData.count("y") ? std::stof(jsonData["y"]) : 0.0f;

                    std::cout << "completeUpgrade request - username: " << username
                        << ", type: " << buildingType << ", x: " << x << ", y: " << y << std::endl;

                    if (username.empty()) {
                        result = false;
                        message = u8"用户名为空";
                    }
                    else if (finishBuildingUpgrade(username, buildingType, x, y)) {
                        result = true;
                        message = u8"升级已完成";
                    }
                    else {
                        result = false;
                        message = u8"升级完成失败";
                    }
                }
                else {
                    result = false;
                    message = u8"未知操作";
                }

            skip_send_response:

                // 构建JSON响应并发送
                jsonResponse = "{\"action\":\"" + escapeJSONString(action) + "\", \"result\":" +
                    (result ? "true" : "false") + ", \"message\":\"" + escapeJSONString(message) + "\"}";

                // 跳过冒号后的空白字符
                std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
                memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
                lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);

                break;
            }

            case LWS_CALLBACK_CLOSED:
            {
                char client_ip[128] = { 0 };
                int client_port = 0;

#ifdef _WIN32
                SOCKET sock = lws_get_socket_fd(wsi);
#else
                int sock = lws_get_socket_fd(wsi);
#endif

                if (sock >= 0) {
                    struct sockaddr_storage addr;
                    socklen_t addr_len = sizeof(addr);
                    if (getpeername(sock, (struct sockaddr*)&addr, &addr_len) == 0) {
                        if (addr.ss_family == AF_INET) {
                            struct sockaddr_in* s = (struct sockaddr_in*)&addr;
                            inet_ntop(AF_INET, &s->sin_addr, client_ip, sizeof(client_ip));
                            client_port = ntohs(s->sin_port);
                        }
                        else if (addr.ss_family == AF_INET6) {
                            struct sockaddr_in6* s = (struct sockaddr_in6*)&addr;
                            inet_ntop(AF_INET6, &s->sin6_addr, client_ip, sizeof(client_ip));
                            client_port = ntohs(s->sin6_port);
                        }
                    }
                }

                if (client_ip[0] != '\0') {
                    std::cout << "Client disconnected from IP: " << client_ip <<
                        ", Port: " << client_port << std::endl;
                }
                else {
                    std::cout << "Client disconnected" << std::endl;
                }

                client->isConnected = false;
                // 处理字符串值
                if (client->isLoggedIn && client->username[0] != '\0') {
                    std::cout << "User " << client->username << " logged out due to disconnection" << std::endl;
                }
                // 从客户端列表中移除该客户端
                {
                    std::lock_guard<std::mutex> lock(serverState.clientsMutex);
                    serverState.clients.erase(
                        std::remove_if(serverState.clients.begin(), serverState.clients.end(),
                            [client](const ClientConnection* c) { return c == client; }),
                        serverState.clients.end());
                }
                // 注意：不要调用delete client，因为内存由libwebsockets管理
                break;
            }
            default:
                break;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in callback: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception in callback" << std::endl;
    }

    return 0;
}


// 协议配置
static lws_protocols protocols[] = {
    {
        "server-protocol",
        callback_server,
        sizeof(ClientConnection),
        4096,
    },
    { NULL, NULL, 0, 0 }
};

// 初始化SQLite数据库
bool initDatabase()
{
    int rc = sqlite3_open("users.db", &serverState.db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(serverState.db) << std::endl;
        sqlite3_close(serverState.db);
        return false;
    }

    // 创建用户表
    const char* createUsersTableSQL = "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"
        "password TEXT NOT NULL);";

    char* errorMsg = nullptr;
    rc = sqlite3_exec(serverState.db, createUsersTableSQL, nullptr, nullptr, &errorMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error (users table): " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
        sqlite3_close(serverState.db);
        return false;
    }

    // 创建资源表
    const char* createResourcesTableSQL = "CREATE TABLE IF NOT EXISTS resources ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"
        "gold INTEGER NOT NULL DEFAULT 0,"
        "elixir INTEGER NOT NULL DEFAULT 0,"
        "gems INTEGER NOT NULL DEFAULT 0,"
        "FOREIGN KEY(username) REFERENCES users(username) ON DELETE CASCADE);";

    rc = sqlite3_exec(serverState.db, createResourcesTableSQL, nullptr, nullptr, &errorMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error (resources table): " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
        sqlite3_close(serverState.db);
        return false;
    }

    // 创建建筑表（包含生产相关列和升级属性）
    const char* createBuildingsTableSQL = "CREATE TABLE IF NOT EXISTS buildings ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL,"
        "building_type TEXT NOT NULL,"
        "position_x REAL NOT NULL,"
        "position_y REAL NOT NULL,"
        "level INTEGER NOT NULL DEFAULT 1,"
        "hp INTEGER NOT NULL DEFAULT 100,"
        "max_hp INTEGER NOT NULL DEFAULT 100,"
        "production_rate INTEGER NOT NULL DEFAULT 1,"
        "max_stock INTEGER NOT NULL DEFAULT 100,"
        "attack INTEGER NOT NULL DEFAULT 0,"
        "current_stock INTEGER NOT NULL DEFAULT 0,"
        "last_production_time INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),"
        "FOREIGN KEY(username) REFERENCES users(username) ON DELETE CASCADE);";

    rc = sqlite3_exec(serverState.db, createBuildingsTableSQL, nullptr, nullptr, &errorMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error (buildings table): " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
        sqlite3_close(serverState.db);
        return false;
    }

    // 创建建筑升级表
    const char* createUpgradesTableSQL = "CREATE TABLE IF NOT EXISTS building_upgrades ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL,"
        "building_type TEXT NOT NULL,"
        "position_x REAL NOT NULL,"
        "position_y REAL NOT NULL,"
        "target_level INTEGER NOT NULL,"
        "start_time INTEGER NOT NULL,"
        "duration INTEGER NOT NULL,"
        "completed INTEGER NOT NULL DEFAULT 0,"
        "FOREIGN KEY(username) REFERENCES users(username) ON DELETE CASCADE);";

    rc = sqlite3_exec(serverState.db, createUpgradesTableSQL, nullptr, nullptr, &errorMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error (building_upgrades table): " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
        sqlite3_close(serverState.db);
        return false;
    }

    // 为已存在的表添加新列（处理旧数据库的迁移）
    // 先检查列是否存在
    const char* checkColumnSQL = "SELECT COUNT(*) FROM pragma_table_info('buildings') WHERE name='current_stock';";
    sqlite3_stmt* checkStmt = nullptr;
    rc = sqlite3_prepare_v2(serverState.db, checkColumnSQL, -1, &checkStmt, nullptr);

    bool hasCurrentStock = false;
    if (rc == SQLITE_OK) {
        if (sqlite3_step(checkStmt) == SQLITE_ROW) {
            hasCurrentStock = (sqlite3_column_int(checkStmt, 0) > 0);
        }
        sqlite3_finalize(checkStmt);
    }

    if (!hasCurrentStock) {
        // 添加允许 NULL 的列（如果表已有数据，NOT NULL 会失败）
        const char* alterSQL1 = "ALTER TABLE buildings ADD COLUMN current_stock INTEGER;";
        rc = sqlite3_exec(serverState.db, alterSQL1, nullptr, nullptr, &errorMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Note: current_stock column may already exist or error: " << errorMsg << std::endl;
            sqlite3_free(errorMsg);
        }
        else {
            std::cout << "Added current_stock column to buildings table" << std::endl;
        }

        // 更新现有数据设置默认值
        const char* updateSQL1 = "UPDATE buildings SET current_stock = 0 WHERE current_stock IS NULL;";
        sqlite3_exec(serverState.db, updateSQL1, nullptr, nullptr, &errorMsg);
    }

    // 检查 last_production_time 列
    checkColumnSQL = "SELECT COUNT(*) FROM pragma_table_info('buildings') WHERE name='last_production_time';";
    rc = sqlite3_prepare_v2(serverState.db, checkColumnSQL, -1, &checkStmt, nullptr);

    bool hasLastProductionTime = false;
    if (rc == SQLITE_OK) {
        if (sqlite3_step(checkStmt) == SQLITE_ROW) {
            hasLastProductionTime = (sqlite3_column_int(checkStmt, 0) > 0);
        }
        sqlite3_finalize(checkStmt);
    }

    if (!hasLastProductionTime) {
        // 添加允许 NULL 的列
        const char* alterSQL2 = "ALTER TABLE buildings ADD COLUMN last_production_time INTEGER;";
        rc = sqlite3_exec(serverState.db, alterSQL2, nullptr, nullptr, &errorMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Note: last_production_time column may already exist or error: " << errorMsg << std::endl;
            sqlite3_free(errorMsg);
        }
        else {
            std::cout << "Added last_production_time column to buildings table" << std::endl;
        }

        // 更新现有数据设置默认值为当前时间
        const char* updateSQL2 = "UPDATE buildings SET last_production_time = strftime('%s', 'now') WHERE last_production_time IS NULL;";
        sqlite3_exec(serverState.db, updateSQL2, nullptr, nullptr, &errorMsg);
    }

    // 创建建筑索引
    const char* createBuildingsIndexSQL = "CREATE INDEX IF NOT EXISTS idx_buildings_username ON buildings(username);";
    rc = sqlite3_exec(serverState.db, createBuildingsIndexSQL, nullptr, nullptr, &errorMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error (buildings index): " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    }

    std::cout << "Database initialized successfully" << std::endl;
    return true;
}

// 注册用户
bool registerUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    // 检查用户名是否已存在
    const char* checkSQL = "SELECT id FROM users WHERE username = ?;";
    sqlite3_stmt* checkStmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, checkSQL, -1, &checkStmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(checkStmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(checkStmt) == SQLITE_ROW) {
        sqlite3_finalize(checkStmt);
        return false; // 用户名已存在
    }
    sqlite3_finalize(checkStmt);

    // 插入新用户
    const char* insertSQL = "INSERT INTO users (username, password) VALUES (?, ?);";
    sqlite3_stmt* stmt = nullptr;

    rc = sqlite3_prepare_v2(serverState.db, insertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);

    // 插入默认建筑 (1个 TownHall + 2个 BuilderHut)
    const char* insertBuildingsSQL = "INSERT INTO buildings (username, building_type, position_x, position_y, level) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* buildingStmt = nullptr;
    rc = sqlite3_prepare_v2(serverState.db, insertBuildingsSQL, -1, &buildingStmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    // TownHall
    sqlite3_bind_text(buildingStmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(buildingStmt, 2, "TownHall", -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(buildingStmt, 3, 1918.0);
    sqlite3_bind_double(buildingStmt, 4, 1373.0);
    sqlite3_bind_int(buildingStmt, 5, 1);
    rc = sqlite3_step(buildingStmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(buildingStmt);
        return false;
    }
    sqlite3_finalize(buildingStmt);

    // BuilderHut 1
    rc = sqlite3_prepare_v2(serverState.db, insertBuildingsSQL, -1, &buildingStmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_text(buildingStmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(buildingStmt, 2, "BuilderHut", -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(buildingStmt, 3, 1600.0);
    sqlite3_bind_double(buildingStmt, 4, 1373.0);
    sqlite3_bind_int(buildingStmt, 5, 1);
    rc = sqlite3_step(buildingStmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(buildingStmt);
        return false;
    }
    sqlite3_finalize(buildingStmt);

    // BuilderHut 2
    rc = sqlite3_prepare_v2(serverState.db, insertBuildingsSQL, -1, &buildingStmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_text(buildingStmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(buildingStmt, 2, "BuilderHut", -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(buildingStmt, 3, 2200.0);
    sqlite3_bind_double(buildingStmt, 4, 1373.0);
    sqlite3_bind_int(buildingStmt, 5, 1);
    rc = sqlite3_step(buildingStmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(buildingStmt);
        return false;
    }
    sqlite3_finalize(buildingStmt);

    std::cout << "User " << username << " registered with default buildings (1 TownHall, 2 BuilderHuts)" << std::endl;
    return true;
}

// 用户登录
bool loginUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    const char* selectSQL = "SELECT password FROM users WHERE username = ?;";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, selectSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error: " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    bool success = false;
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const unsigned char* dbPassword = sqlite3_column_text(stmt, 0);
        if (dbPassword && password == reinterpret_cast<const char*>(dbPassword)) {
            success = true;
        }
    }

    sqlite3_finalize(stmt);
    return success;
}

// 删除用户
bool deleteUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    const char* deleteUsersSQL = "DELETE FROM users WHERE username = ?;";
    const char* deleteResourcesSQL = "DELETE FROM resources WHERE username = ?;";
    const char* deleteBuildingsSQL = "DELETE FROM buildings WHERE username = ?;";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, deleteUsersSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error: " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "SQL step error: " << sqlite3_errmsg(serverState.db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);

    rc = sqlite3_prepare_v2(serverState.db, deleteResourcesSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error: " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "SQL step error: " << sqlite3_errmsg(serverState.db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);

    rc = sqlite3_prepare_v2(serverState.db, deleteBuildingsSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error: " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "SQL step error: " << sqlite3_errmsg(serverState.db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);

    return true;
}

// 修改密码
bool changePassword(const std::string& username, const std::string& newPassword) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    const char* updateSQL = "UPDATE users SET password = ? WHERE username = ?;";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, updateSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error: " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, newPassword.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "SQL step error: " << sqlite3_errmsg(serverState.db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    int changes = sqlite3_changes(serverState.db);
    sqlite3_finalize(stmt);

    return changes > 0;
}

// 检查用户是否已登录
bool isUserLoggedIn(const std::string& username) {
    std::lock_guard<std::mutex> lock(serverState.clientsMutex);
    for (const auto& client : serverState.clients) {
        if (client->isLoggedIn && client->username == username) {
            return true;
        }
    }
    return false;
}

// 用户登出
void logoutUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(serverState.clientsMutex);
    for (const auto& client : serverState.clients) {
        if (client->isLoggedIn && strcmp(client->username, username.c_str()) == 0) {
            client->isLoggedIn = false;
            client->username[0] = '\0';
            std::cout << "User " << username << " logged out" << std::endl;
            return;
        }
    }
}

// 退出登录
bool getResource(const std::string& username, int& gold, int& elixir, int& gems) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    const char* selectSQL = "SELECT gold, elixir, gems FROM resources WHERE username = ?;";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, selectSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error (getResource): " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    bool found = false;
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        gold = sqlite3_column_int(stmt, 0);
        elixir = sqlite3_column_int(stmt, 1);
        gems = sqlite3_column_int(stmt, 2);
        found = true;
        std::cout << "Resource loaded for user " << username << ": gold=" << gold
            << ", elixir=" << elixir << ", gems=" << gems << std::endl;
    }
    else {
        gold = 750;
        elixir = 750;
        gems = 15;
        std::cout << "No resource data found for user " << username
            << ", using default values: gold=" << gold
            << ", elixir=" << elixir << ", gems=" << gems << std::endl;
    }

    sqlite3_finalize(stmt);
    return found;
}

// 更新用户资源
bool updateResource(const std::string& username, int gold, int elixir, int gems) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    const char* checkSQL = "SELECT id FROM resources WHERE username = ?;";
    sqlite3_stmt* checkStmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, checkSQL, -1, &checkStmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(checkStmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(checkStmt) == SQLITE_ROW) {
        sqlite3_finalize(checkStmt);
        const char* updateSQL = "UPDATE resources SET gold = ?, elixir = ?, gems = ? WHERE username = ?;";
        sqlite3_stmt* stmt = nullptr;

        rc = sqlite3_prepare_v2(serverState.db, updateSQL, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            return false;
        }

        sqlite3_bind_int(stmt, 1, gold);
        sqlite3_bind_int(stmt, 2, elixir);
        sqlite3_bind_int(stmt, 3, gems);
        sqlite3_bind_text(stmt, 4, username.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc == SQLITE_DONE) {
            std::cout << "Resource updated for user " << username << ": gold=" << gold
                << ", elixir=" << elixir << ", gems=" << gems << std::endl;
            return true;
        }
        return false;
    }
    else {
        sqlite3_finalize(checkStmt);
        const char* insertSQL = "INSERT INTO resources (username, gold, elixir, gems) VALUES (?, ?, ?, ?);";
        sqlite3_stmt* stmt = nullptr;

        rc = sqlite3_prepare_v2(serverState.db, insertSQL, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            return false;
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, gold);
        sqlite3_bind_int(stmt, 3, elixir);
        sqlite3_bind_int(stmt, 4, gems);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc == SQLITE_DONE) {
            std::cout << "Resource created for user " << username << ": gold=" << gold
                << ", elixir=" << elixir << ", gems=" << gems << std::endl;
            return true;
        }
        return false;
    }
}

// 保存建筑（包含升级属性）
bool saveBuilding(const std::string& username, const std::string& buildingType, float x, float y, int level,
    int hp, int maxHp, int productionRate, int maxStock, int attack) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    // 检查位置是否已有建筑
    const char* checkSQL = "SELECT id FROM buildings WHERE username = ? AND position_x = ? AND position_y = ?;";
    sqlite3_stmt* checkStmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, checkSQL, -1, &checkStmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error (saveBuilding check): " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }

    sqlite3_bind_text(checkStmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(checkStmt, 2, x);
    sqlite3_bind_double(checkStmt, 3, y);

    if (sqlite3_step(checkStmt) == SQLITE_ROW) {
        sqlite3_finalize(checkStmt);
        // 更新建筑信息（包含升级属性）
        const char* updateSQL = "UPDATE buildings SET building_type = ?, level = ?, hp = ?, max_hp = ?, production_rate = ?, max_stock = ?, attack = ? WHERE username = ? AND position_x = ? AND position_y = ?;";
        sqlite3_stmt* stmt = nullptr;

        rc = sqlite3_prepare_v2(serverState.db, updateSQL, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            return false;
        }

        sqlite3_bind_text(stmt, 1, buildingType.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, level);
        sqlite3_bind_int(stmt, 3, hp);
        sqlite3_bind_int(stmt, 4, maxHp);
        sqlite3_bind_int(stmt, 5, productionRate);
        sqlite3_bind_int(stmt, 6, maxStock);
        sqlite3_bind_int(stmt, 7, attack);
        sqlite3_bind_text(stmt, 8, username.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 9, x);
        sqlite3_bind_double(stmt, 10, y);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc == SQLITE_DONE) {
            std::cout << "Building updated for user " << username << ": type=" << buildingType
                << ", x=" << x << ", y=" << y << ", level=" << level
                << ", hp=" << hp << ", max_hp=" << maxHp << ", prod=" << productionRate
                << ", max_stock=" << maxStock << ", attack=" << attack << std::endl;
            return true;
        }
        return false;
    }
    else {
        sqlite3_finalize(checkStmt);
        // 插入新建筑记录（包含升级属性）
        const char* insertSQL = "INSERT INTO buildings (username, building_type, position_x, position_y, level, hp, max_hp, production_rate, max_stock, attack) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt = nullptr;

        rc = sqlite3_prepare_v2(serverState.db, insertSQL, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL prepare error (saveBuilding insert): " << sqlite3_errmsg(serverState.db) << std::endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, buildingType.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 3, x);
        sqlite3_bind_double(stmt, 4, y);
        sqlite3_bind_int(stmt, 5, level);
        sqlite3_bind_int(stmt, 6, hp);
        sqlite3_bind_int(stmt, 7, maxHp);
        sqlite3_bind_int(stmt, 8, productionRate);
        sqlite3_bind_int(stmt, 9, maxStock);
        sqlite3_bind_int(stmt, 10, attack);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc == SQLITE_DONE) {
            std::cout << "Building saved for user " << username << ": type=" << buildingType
                << ", x=" << x << ", y=" << y << ", level=" << level
                << ", hp=" << hp << ", max_hp=" << maxHp << ", prod=" << productionRate
                << ", max_stock=" << maxStock << ", attack=" << attack << std::endl;
            return true;
        }
        std::cerr << "SQL step error (saveBuilding insert): " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }
}

// 获取所有建筑（包含升级属性）
bool getBuildings(const std::string& username, std::string& buildingsJson) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    const char* selectSQL = "SELECT building_type, position_x, position_y, level, hp, max_hp, production_rate, max_stock, attack FROM buildings WHERE username = ?;";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, selectSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error (getBuildings): " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<std::string> buildings;
    rc = sqlite3_step(stmt);
    while (rc == SQLITE_ROW) {
        std::string buildingType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        float x = static_cast<float>(sqlite3_column_double(stmt, 1));
        float y = static_cast<float>(sqlite3_column_double(stmt, 2));
        int level = sqlite3_column_int(stmt, 3);
        int hp = sqlite3_column_int(stmt, 4);
        int maxHp = sqlite3_column_int(stmt, 5);
        int productionRate = sqlite3_column_int(stmt, 6);
        int maxStock = sqlite3_column_int(stmt, 7);
        int attack = sqlite3_column_int(stmt, 8);

        std::string buildingStr = "{\"type\":\"" + escapeJSONString(buildingType) +
            "\", \"x\":" + std::to_string(x) +
            ", \"y\":" + std::to_string(y) +
            ", \"level\":" + std::to_string(level) +
            ", \"hp\":" + std::to_string(hp) +
            ", \"maxHp\":" + std::to_string(maxHp) +
            ", \"productionRate\":" + std::to_string(productionRate) +
            ", \"maxStock\":" + std::to_string(maxStock) +
            ", \"attack\":" + std::to_string(attack) + "}";
        buildings.push_back(buildingStr);

        rc = sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);

    // 构建JSON数组
    buildingsJson = "[";
    for (size_t i = 0; i < buildings.size(); i++) {
        buildingsJson += buildings[i];
        if (i < buildings.size() - 1) {
            buildingsJson += ", ";
        }
    }
    buildingsJson += "]";

    std::cout << "Loaded " << buildings.size() << " buildings for user " << username << std::endl;
    return true;
}

// 删除建筑
bool deleteBuilding(const std::string& username, float x, float y) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    const char* deleteSQL = "DELETE FROM buildings WHERE username = ? AND position_x = ? AND position_y = ?;";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, deleteSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error (deleteBuilding): " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 2, x);
    sqlite3_bind_double(stmt, 3, y);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE) {
        int changes = sqlite3_changes(serverState.db);
        if (changes > 0) {
            std::cout << "Building deleted for user " << username << " at position (" << x << ", " << y << ")" << std::endl;
            return true;
        }
    }
    return false;
}

// 更新建筑生产状态
bool updateBuildingProduction(const std::string& username, const std::string& buildingType,
    float x, float y, int currentStock, long long lastProductionTime) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    const char* updateSQL = "UPDATE buildings SET current_stock = ?, last_production_time = ? "
        "WHERE username = ? AND building_type = ? AND position_x = ? AND position_y = ?;";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, updateSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error (updateBuildingProduction): " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, currentStock);
    sqlite3_bind_int64(stmt, 2, lastProductionTime);
    sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, buildingType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 5, x);
    sqlite3_bind_double(stmt, 6, y);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE) {
        int changes = sqlite3_changes(serverState.db);
        if (changes > 0) {
            std::cout << "Production updated for " << buildingType << " at (" << x << ", " << y
                << "): stock=" << currentStock << ", lastTime=" << lastProductionTime << std::endl;
            return true;
        }
    }
    return false;
}

// 获取所有建筑的生产状态
bool getBuildingProduction(const std::string& username, std::string& productionJson) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    // 首先检查列是否存在
    const char* checkSQL = "SELECT last_production_time FROM buildings LIMIT 1;";
    sqlite3_stmt* checkStmt = nullptr;
    int rc = sqlite3_prepare_v2(serverState.db, checkSQL, -1, &checkStmt, nullptr);

    bool hasTimeColumn = (rc == SQLITE_OK);
    if (checkStmt) {
        sqlite3_finalize(checkStmt);
    }

    // 如果列不存在，尝试添加它
    if (!hasTimeColumn) {
        std::cerr << "Column last_production_time not found, attempting to add..." << std::endl;
        // 注意：SQLite ALTER TABLE ADD COLUMN 不允许使用非常量默认值
        // 所以我们先添加允许 NULL 的列，然后更新所有行的值
        const char* alterSQL = "ALTER TABLE buildings ADD COLUMN last_production_time INTEGER;";  // 不使用 DEFAULT
        char* errorMsg = nullptr;
        rc = sqlite3_exec(serverState.db, alterSQL, nullptr, nullptr, &errorMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to add last_production_time column: " << errorMsg << std::endl;
            sqlite3_free(errorMsg);
        }
        else {
            std::cout << "Successfully added last_production_time column" << std::endl;
            // 更新现有数据设置时间为当前时间
            const char* updateSQL = "UPDATE buildings SET last_production_time = strftime('%s', 'now') WHERE last_production_time IS NULL;";
            sqlite3_exec(serverState.db, updateSQL, nullptr, nullptr, nullptr);
            hasTimeColumn = true;
        }
    }

    // 同样检查 current_stock 列
    bool hasStockColumn = true;
    checkSQL = "SELECT current_stock FROM buildings LIMIT 1;";
    rc = sqlite3_prepare_v2(serverState.db, checkSQL, -1, &checkStmt, nullptr);
    if (rc != SQLITE_OK) {
        hasStockColumn = false;
    }
    else {
        sqlite3_finalize(checkStmt);
    }

    if (!hasStockColumn) {
        std::cerr << "Column current_stock not found, attempting to add..." << std::endl;
        // SQLite ALTER TABLE ADD COLUMN 不允许 NOT NULL 且没有 DEFAULT 的列
        // 所以我们先添加允许 NULL 的列，然后更新所有行的值
        const char* alterSQL = "ALTER TABLE buildings ADD COLUMN current_stock INTEGER;";  // 不使用 NOT NULL DEFAULT
        char* errorMsg = nullptr;
        rc = sqlite3_exec(serverState.db, alterSQL, nullptr, nullptr, &errorMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to add current_stock column: " << errorMsg << std::endl;
            sqlite3_free(errorMsg);
        }
        else {
            std::cout << "Successfully added current_stock column" << std::endl;
            // 更新现有数据设置初始值为 0
            const char* updateSQL = "UPDATE buildings SET current_stock = 0 WHERE current_stock IS NULL;";
            sqlite3_exec(serverState.db, updateSQL, nullptr, nullptr, nullptr);
            hasStockColumn = true;
        }
    }

    // 如果任何必要的列仍然不存在，返回空数据
    if (!hasTimeColumn || !hasStockColumn) {
        std::cerr << "Required columns missing, cannot get production data" << std::endl;
        productionJson = "[]";
        return false;
    }

    const char* selectSQL = "SELECT building_type, position_x, position_y, level, current_stock, last_production_time "
        "FROM buildings WHERE username = ? AND (building_type = 'GoldMine' OR building_type = 'ElixirCollector');";
    sqlite3_stmt* stmt = nullptr;

    rc = sqlite3_prepare_v2(serverState.db, selectSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error (getBuildingProduction): " << sqlite3_errmsg(serverState.db) << std::endl;
        productionJson = "[]";
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<std::string> productions;
    rc = sqlite3_step(stmt);
    while (rc == SQLITE_ROW) {
        std::string buildingType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        float x = static_cast<float>(sqlite3_column_double(stmt, 1));
        float y = static_cast<float>(sqlite3_column_double(stmt, 2));
        int level = sqlite3_column_int(stmt, 3);
        int currentStock = sqlite3_column_int(stmt, 4);
        long long lastTime = sqlite3_column_int64(stmt, 5);

        std::string prodStr = "{\"type\":\"" + escapeJSONString(buildingType) +
            "\", \"x\":" + std::to_string(x) +
            ", \"y\":" + std::to_string(y) +
            ", \"level\":" + std::to_string(level) +
            ", \"currentStock\":" + std::to_string(currentStock) +
            ", \"lastProductionTime\":" + std::to_string(lastTime) + "}";
        productions.push_back(prodStr);

        rc = sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);

    productionJson = "[";
    for (size_t i = 0; i < productions.size(); i++) {
        productionJson += productions[i];
        if (i < productions.size() - 1) {
            productionJson += ", ";
        }
    }
    productionJson += "]";

    std::cout << "Loaded " << productions.size() << " production buildings for user " << username << std::endl;
    return true;
}

// 处理离线生产（当用户登录时计算离线期间产生的资源）
bool addOfflineProduction(const std::string& username) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    const char* selectSQL = "SELECT id, building_type, level, current_stock, last_production_time, position_x, position_y "
        "FROM buildings WHERE username = ? AND (building_type = 'GoldMine' OR building_type = 'ElixirCollector');";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, selectSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    long long currentTime = time(nullptr);
    bool anyUpdated = false;

    rc = sqlite3_step(stmt);
    while (rc == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string buildingType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int level = sqlite3_column_int(stmt, 2);
        int currentStock = sqlite3_column_int(stmt, 3);
        long long lastTime = sqlite3_column_int64(stmt, 4);
        float x = static_cast<float>(sqlite3_column_double(stmt, 5));
        float y = static_cast<float>(sqlite3_column_double(stmt, 6));

        long long elapsed = currentTime - lastTime;
        if (elapsed <= 0) {
            rc = sqlite3_step(stmt);
            continue;
        }

        int maxCapacity = (buildingType == "GoldMine") ? (100 + (level - 1) * 100) : (100 + (level - 1) * 50);
        int productionRate = level;
        int produced = static_cast<int>(elapsed * productionRate);
        int newStock = min(currentStock + produced, maxCapacity);

        if (newStock > currentStock) {
            const char* updateSQL = "UPDATE buildings SET current_stock = ?, last_production_time = ? WHERE id = ?;";
            sqlite3_stmt* updateStmt = nullptr;

            rc = sqlite3_prepare_v2(serverState.db, updateSQL, -1, &updateStmt, nullptr);
            if (rc == SQLITE_OK) {
                sqlite3_bind_int(updateStmt, 1, newStock);
                sqlite3_bind_int64(updateStmt, 2, currentTime);
                sqlite3_bind_int(updateStmt, 3, id);
                sqlite3_step(updateStmt);
                sqlite3_finalize(updateStmt);

                std::cout << "Offline production for " << buildingType << " at (" << x << ", " << y
                    << "): +" << (newStock - currentStock) << " resources" << std::endl;
                anyUpdated = true;
            }
        }

        rc = sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);
    return anyUpdated;
}

// 收集建筑生产的资源 - 更新为设置剩余存储量（不更新last_production_time，让离线生产继续计算）
bool collectProduction(const std::string& username, const std::string& buildingType,
    float x, float y, int remainingStock) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    // 使用ROUND处理浮点数精度问题
    // 注意：不更新last_production_time，让生产继续计算
    const char* updateSQL = "UPDATE buildings SET current_stock = ? "
        "WHERE username = ? AND building_type = ? AND ROUND(position_x, 1) = ROUND(?, 1) AND ROUND(position_y, 1) = ROUND(?, 1);";
    sqlite3_stmt* updateStmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, updateSQL, -1, &updateStmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error (collectProduction): " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }

    sqlite3_bind_int(updateStmt, 1, remainingStock);
    sqlite3_bind_text(updateStmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(updateStmt, 3, buildingType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(updateStmt, 4, x);
    sqlite3_bind_double(updateStmt, 5, y);

    rc = sqlite3_step(updateStmt);
    int affectedRows = sqlite3_changes(serverState.db);
    sqlite3_finalize(updateStmt);

    if (rc == SQLITE_DONE && affectedRows > 0) {
        std::cout << "Collected from " << buildingType << " for user " << username
            << " - remaining stock: " << remainingStock << std::endl;
        return true;
    }
    else if (affectedRows == 0) {
        std::cerr << "collectProduction: No rows updated - building not found at position (" << x << ", " << y << ")" << std::endl;
    }
    return false;
}

// 开始建筑升级
bool startBuildingUpgrade(const std::string& username, const std::string& buildingType,
    float x, float y, int targetLevel, int duration) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    // 检查是否已有该位置的升级在进行中
    const char* checkSQL = "SELECT id FROM building_upgrades WHERE username = ? "
        "AND building_type = ? AND position_x = ? AND position_y = ? AND completed = 0;";
    sqlite3_stmt* checkStmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, checkSQL, -1, &checkStmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error (startBuildingUpgrade check): " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }

    sqlite3_bind_text(checkStmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(checkStmt, 2, buildingType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(checkStmt, 3, x);
    sqlite3_bind_double(checkStmt, 4, y);

    if (sqlite3_step(checkStmt) == SQLITE_ROW) {
        sqlite3_finalize(checkStmt);
        std::cerr << "Upgrade already in progress for building at (" << x << ", " << y << ")" << std::endl;
        return false;
    }
    sqlite3_finalize(checkStmt);

    // 插入新的升级记录
    const char* insertSQL = "INSERT INTO building_upgrades (username, building_type, position_x, position_y, "
        "target_level, start_time, duration, completed) VALUES (?, ?, ?, ?, ?, ?, ?, 0);";
    sqlite3_stmt* stmt = nullptr;

    rc = sqlite3_prepare_v2(serverState.db, insertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error (startBuildingUpgrade insert): " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }

    long long startTime = time(nullptr);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, buildingType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, x);
    sqlite3_bind_double(stmt, 4, y);
    sqlite3_bind_int(stmt, 5, targetLevel);
    sqlite3_bind_int64(stmt, 6, startTime);
    sqlite3_bind_int(stmt, 7, duration);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE) {
        std::cout << "Upgrade started for " << buildingType << " at (" << x << ", " << y
            << ") - target level: " << targetLevel << ", duration: " << duration << "s" << std::endl;
        return true;
    }

    std::cerr << "SQL step error (startBuildingUpgrade insert): " << sqlite3_errmsg(serverState.db) << std::endl;
    return false;
}

// 获取建筑升级状态
bool getBuildingUpgradeStatus(const std::string& username, std::string& upgradesJson) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    const char* selectSQL = "SELECT building_type, position_x, position_y, target_level, "
        "start_time, duration, completed FROM building_upgrades WHERE username = ? AND completed = 0;";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, selectSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error (getBuildingUpgradeStatus): " << sqlite3_errmsg(serverState.db) << std::endl;
        upgradesJson = "[]";
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<std::string> upgrades;
    long long currentTime = time(nullptr);
    rc = sqlite3_step(stmt);
    while (rc == SQLITE_ROW) {
        std::string buildingType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        float x = static_cast<float>(sqlite3_column_double(stmt, 1));
        float y = static_cast<float>(sqlite3_column_double(stmt, 2));
        int targetLevel = sqlite3_column_int(stmt, 3);
        long long startTime = sqlite3_column_int64(stmt, 4);
        int duration = sqlite3_column_int(stmt, 5);
        int completed = sqlite3_column_int(stmt, 6);

        long long elapsed = currentTime - startTime;
        int remainingTime = max(0, duration - static_cast<int>(elapsed));

        std::string upgradeStr = "{\"type\":\"" + escapeJSONString(buildingType) +
            "\", \"x\":" + std::to_string(x) +
            ", \"y\":" + std::to_string(y) +
            ", \"targetLevel\":" + std::to_string(targetLevel) +
            ", \"startTime\":" + std::to_string(startTime) +
            ", \"duration\":" + std::to_string(duration) +
            ", \"remainingTime\":" + std::to_string(remainingTime) +
            ", \"completed\":" + std::to_string(completed) + "}";
        upgrades.push_back(upgradeStr);

        rc = sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);

    upgradesJson = "[";
    for (size_t i = 0; i < upgrades.size(); i++) {
        upgradesJson += upgrades[i];
        if (i < upgrades.size() - 1) {
            upgradesJson += ", ";
        }
    }
    upgradesJson += "]";

    std::cout << "Found " << upgrades.size() << " active upgrades for user " << username << std::endl;
    return true;
}

// 完成建筑升级
bool finishBuildingUpgrade(const std::string& username, const std::string& buildingType,
    float x, float y) {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    const char* selectSQL = "SELECT id, target_level FROM building_upgrades WHERE username = ? "
        "AND building_type = ? AND position_x = ? AND position_y = ? AND completed = 0;";
    sqlite3_stmt* selectStmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, selectSQL, -1, &selectStmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(selectStmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(selectStmt, 2, buildingType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(selectStmt, 3, x);
    sqlite3_bind_double(selectStmt, 4, y);

    if (sqlite3_step(selectStmt) != SQLITE_ROW) {
        sqlite3_finalize(selectStmt);
        std::cerr << "No active upgrade found for building at (" << x << ", " << y << ")" << std::endl;
        return false;
    }

    int upgradeId = sqlite3_column_int(selectStmt, 0);
    int newLevel = sqlite3_column_int(selectStmt, 1);
    sqlite3_finalize(selectStmt);

    // 更新升级记录为已完成
    const char* updateSQL = "UPDATE building_upgrades SET completed = 1 WHERE id = ?;";
    sqlite3_stmt* updateStmt = nullptr;

    rc = sqlite3_prepare_v2(serverState.db, updateSQL, -1, &updateStmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(updateStmt, 1, upgradeId);
    rc = sqlite3_step(updateStmt);
    sqlite3_finalize(updateStmt);

    if (rc != SQLITE_DONE) {
        return false;
    }

    // 更新建筑等级
    bool levelUpdated = updateBuildingLevel(username, buildingType, x, y, newLevel);

    if (levelUpdated) {
        std::cout << "Upgrade completed for " << buildingType << " at (" << x << ", " << y
            << ") - new level: " << newLevel << std::endl;
    }

    return levelUpdated;
}

// 广播升级完成消息给指定用户的所有在线客户端
bool broadcastUpgradeComplete(const std::string& username, const std::string& buildingType,
    float x, float y, int newLevel) {
    std::lock_guard<std::mutex> lock(serverState.clientsMutex);

    std::string jsonMessage = "{\"action\":\"upgradeComplete\", \"buildingType\":\"" +
        escapeJSONString(buildingType) +
        "\", \"x\":" + std::to_string(x) +
        ", \"y\":" + std::to_string(y) +
        ", \"newLevel\":" + std::to_string(newLevel) + "}";

    int sentCount = 0;
    for (const auto& client : serverState.clients) {
        if (client->isConnected && client->isLoggedIn &&
            strcmp(client->username, username.c_str()) == 0) {

            std::vector<unsigned char> buf(LWS_PRE + jsonMessage.size() + 1);
            memcpy(&buf[LWS_PRE], jsonMessage.c_str(), jsonMessage.size());
            lws_write(client->wsi, &buf[LWS_PRE], jsonMessage.size(), LWS_WRITE_TEXT);

            sentCount++;
            std::cout << "Sent upgradeComplete to user " << username << " (client)" << std::endl;
        }
    }

    std::cout << "Broadcast upgradeComplete for " << buildingType << " at (" << x << ", " << y
        << ") to " << sentCount << " clients" << std::endl;
    return sentCount > 0;
}

// 更新建筑等级
bool updateBuildingLevel(const std::string& username, const std::string& buildingType,
    float x, float y, int newLevel) {
    const char* updateSQL = "UPDATE buildings SET level = ? WHERE username = ? "
        "AND building_type = ? AND position_x = ? AND position_y = ?;";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, updateSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error (updateBuildingLevel): " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, newLevel);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, buildingType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, x);
    sqlite3_bind_double(stmt, 5, y);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE) {
        int changes = sqlite3_changes(serverState.db);
        if (changes > 0) {
            return true;
        }
    }
    return false;
}

// 处理所有已完成的升级（服务器定时调用）
void processCompletedUpgrades() {
    std::lock_guard<std::mutex> lock(serverState.dbMutex);

    long long currentTime = time(nullptr);

    const char* selectSQL = "SELECT id, username, building_type, position_x, position_y, "
        "target_level, start_time, duration FROM building_upgrades WHERE completed = 0;";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, selectSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return;
    }

    rc = sqlite3_step(stmt);
    while (rc == SQLITE_ROW) {
        int upgradeId = sqlite3_column_int(stmt, 0);
        std::string username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string buildingType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        float x = static_cast<float>(sqlite3_column_double(stmt, 3));
        float y = static_cast<float>(sqlite3_column_double(stmt, 4));
        int targetLevel = sqlite3_column_int(stmt, 5);
        long long startTime = sqlite3_column_int64(stmt, 6);
        int duration = sqlite3_column_int(stmt, 7);

        long long elapsed = currentTime - startTime;
        if (elapsed >= duration) {
            // 升级已完成，标记为完成并更新建筑等级
            const char* completeSQL = "UPDATE building_upgrades SET completed = 1 WHERE id = ?;";
            sqlite3_stmt* completeStmt = nullptr;

            rc = sqlite3_prepare_v2(serverState.db, completeSQL, -1, &completeStmt, nullptr);
            if (rc == SQLITE_OK) {
                sqlite3_bind_int(completeStmt, 1, upgradeId);
                sqlite3_step(completeStmt);
                sqlite3_finalize(completeStmt);
            }

            // 更新建筑等级
            const char* updateSQL = "UPDATE buildings SET level = ? WHERE username = ? "
                "AND building_type = ? AND position_x = ? AND position_y = ?;";
            sqlite3_stmt* updateStmt = nullptr;

            rc = sqlite3_prepare_v2(serverState.db, updateSQL, -1, &updateStmt, nullptr);
            if (rc == SQLITE_OK) {
                sqlite3_bind_int(updateStmt, 1, targetLevel);
                sqlite3_bind_text(updateStmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(updateStmt, 3, buildingType.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_double(updateStmt, 4, x);
                sqlite3_bind_double(updateStmt, 5, y);
                sqlite3_step(updateStmt);
                sqlite3_finalize(updateStmt);

                // 广播升级完成通知给客户端
                broadcastUpgradeComplete(username, buildingType, x, y, targetLevel);

                std::cout << "Server processed upgrade completion for " << buildingType
                    << " at (" << x << ", " << y << ") - new level: " << targetLevel
                    << " (offline upgrade)" << std::endl;
            }
        }

        rc = sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);
}

// 解析JSON函数，将JSON字符串转换为键值对映射
std::map<std::string, std::string> parseJSON(const std::string& jsonStr) {
    std::map<std::string, std::string> result;
    std::string::size_type pos = 1; // 从位置1开始，跳过前导{字符

    while (pos < jsonStr.size()) {
        // 跳过空白字符
        while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\t' || jsonStr[pos] == ',' || jsonStr[pos] == '\n' || jsonStr[pos] == '\r')) {
            pos++;
        }
        if (pos >= jsonStr.size()) break;

        // 检查键名开始
        if (jsonStr[pos] != '"') break;
        std::string::size_type keyStart = pos;
        std::string::size_type keyEnd = jsonStr.find('"', keyStart + 1);
        if (keyEnd == std::string::npos)
            break;

        // 提取键名
        std::string key = jsonStr.substr(keyStart + 1, keyEnd - keyStart - 1);

        // 查找冒号
        std::string::size_type colonPos = jsonStr.find(':', keyEnd + 1);
        if (colonPos == std::string::npos)
            break;

        // 跳过冒号后的空白字符
        std::string::size_type valueStart = colonPos + 1;
        while (valueStart < jsonStr.size() && (jsonStr[valueStart] == ' ' || jsonStr[valueStart] == '\t')) {
            valueStart++;
        }
        if (valueStart >= jsonStr.size()) break;

        std::string value;

        // 判断是否为字符串值
        if (jsonStr[valueStart] == '"') {
            // 处理字符串值
            std::string::size_type valueEnd = jsonStr.find('"', valueStart + 1);
            if (valueEnd == std::string::npos)
                break;
            value = jsonStr.substr(valueStart + 1, valueEnd - valueStart - 1);
            pos = valueEnd + 1;
        }
        else {
            // 处理非字符串值
            std::string::size_type valueEnd = valueStart;
            while (valueEnd < jsonStr.size() &&
                jsonStr[valueEnd] != ',' &&
                jsonStr[valueEnd] != '}' &&
                jsonStr[valueEnd] != ' ' &&
                jsonStr[valueEnd] != '\t' &&
                jsonStr[valueEnd] != '\n' &&
                jsonStr[valueEnd] != '\r') {
                valueEnd++;
            }
            value = jsonStr.substr(valueStart, valueEnd - valueStart);
            pos = valueEnd;
        }

        // 添加到结果映射
        result[key] = value;
    }

    return result;
}

// 服务器端资源生产定时器
void productionTimerThread() {
    const int PRODUCTION_INTERVAL = 10; // 每10秒更新一次生产

    std::cout << "Production timer thread started" << std::endl;

    while (serverState.isRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(PRODUCTION_INTERVAL));

        if (!serverState.isRunning) break;

        std::lock_guard<std::mutex> lock(serverState.dbMutex);
        long long currentTime = time(nullptr);

        const char* selectSQL = "SELECT id, building_type, level, current_stock, username, position_x, position_y "
            "FROM buildings WHERE building_type = 'GoldMine' OR building_type = 'ElixirCollector';";
        sqlite3_stmt* stmt = nullptr;

        int rc = sqlite3_prepare_v2(serverState.db, selectSQL, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            continue;
        }

        rc = sqlite3_step(stmt);
        while (rc == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string buildingType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            int level = sqlite3_column_int(stmt, 2);
            int currentStock = sqlite3_column_int(stmt, 3);
            std::string username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            float x = static_cast<float>(sqlite3_column_double(stmt, 5));
            float y = static_cast<float>(sqlite3_column_double(stmt, 6));

            int maxCapacity = (buildingType == "GoldMine") ? (100 + (level - 1) * 100) : (100 + (level - 1) * 50);
            int productionRate = level;

            bool needUpdate = false;
            int newStock = currentStock;

            if (currentStock < maxCapacity) {
                int produced = productionRate;
                newStock = min(currentStock + produced, maxCapacity);
                needUpdate = true;
            }

            if (needUpdate) {
                const char* updateSQL = "UPDATE buildings SET current_stock = ?, last_production_time = ? WHERE id = ?;";
                sqlite3_stmt* updateStmt = nullptr;

                rc = sqlite3_prepare_v2(serverState.db, updateSQL, -1, &updateStmt, nullptr);
                if (rc == SQLITE_OK) {
                    sqlite3_bind_int(updateStmt, 1, newStock);
                    sqlite3_bind_int64(updateStmt, 2, currentTime);
                    sqlite3_bind_int(updateStmt, 3, id);
                    sqlite3_step(updateStmt);
                    sqlite3_finalize(updateStmt);

                    if (newStock >= maxCapacity) {
                        std::cout << "Building " << buildingType << " (ID: " << id << ") reached max capacity: " << newStock << std::endl;
                    }
                }
            }

            rc = sqlite3_step(stmt);
        }

        sqlite3_finalize(stmt);
    }

    std::cout << "Production timer thread stopped" << std::endl;
}

int main()
{
    // 设置控制台输出编码为UTF-8
#ifdef _WIN32
    system("chcp 65001");
#endif

    std::cout << "Starting WebSocket server..." << std::endl;

    serverState.isRunning = true;
    serverState.db = nullptr;

    // 初始化数据库
    if (!initDatabase()) {
        return -1;
    }

    // 启动资源生产定时器线程
    std::thread prodThread(productionTimerThread);
    prodThread.detach();

    // 启动升级处理定时器线程（每5秒检查一次完成升级）
    std::thread upgradeThread([]() {
        while (serverState.isRunning) {
            processCompletedUpgrades();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        });
    upgradeThread.detach();

    std::cout << "Upgrade timer thread started" << std::endl;

    // 初始化libwebsockets
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));
    info.port = 8080;
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;

    // 配置服务器仅监听IPv4
    const char* serverAddress = "100.80.250.106";
    info.iface = serverAddress;

    std::cout << "Attempting to bind to: " << serverAddress << std::endl;

    struct lws_context* context = lws_create_context(&info);
    if (!context) {
        std::cerr << "Failed to create context" << std::endl;
        return -1;
    }

    std::cout << "Server started on " << serverAddress << ":8080" << std::endl;

    // 主循环
    while (serverState.isRunning) {
        lws_service(context, 50);
    }

    // 清理资源
    lws_context_destroy(context);

    // 关闭数据库连接
    if (serverState.db) {
        sqlite3_close(serverState.db);
        std::cout << "Database connection closed" << std::endl;
    }

    return 0;
}