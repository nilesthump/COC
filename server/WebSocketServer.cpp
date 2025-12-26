#include <iostream>
#include <string>
#include <vector>
#include <libwebsockets.h>
#include "sqlite3.h"
#include <sstream>
#include <map>
#include <mutex>

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

// 辅助函数：转义JSON字符串中的特殊字符
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

// WebSocket回调处理函数
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

                        // 构建JSON响应（与客户端格式匹配）
                        jsonResponse = "{\"action\":\"getResource\", \"result\":false, \"message\":\"" +
                            escapeJSONString(message) + "\"}";

                        std::cout << "Sending response: " << jsonResponse << std::endl;

                        // 发送响应给客户端
                        std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
                        memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
                        lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);
                    }
                    else {
                        int gold = 0;
                        int elixir = 0;
                        int gems = 0;
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

                        // 发送响应给客户端
                        std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
                        memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
                        lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);
                    }
                    // 资源响应已发送，跳过通用响应
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
                else {
                    result = false;
                    message = u8"未知动作";
                }

            skip_send_response:
            send_response:

                // 构建JSON响应（与客户端格式匹配）
                jsonResponse = "{\"action\":\"" + escapeJSONString(action) + "\", \"result\":" +
                    (result ? "true" : "false") + ", \"message\":\"" + escapeJSONString(message) + "\"}";

                // 发送响应给客户端
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
                // 如果客户端已登录，执行登出操作
                if (client->isLoggedIn && client->username[0] != '\0') {
                    std::cout << "User " << client->username << " logged out due to disconnection" << std::endl;
                }
                // 从客户端列表中移除
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

    const char* deleteSQL = "DELETE FROM users WHERE username = ?;";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, deleteSQL, -1, &stmt, nullptr);
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

    int changes = sqlite3_changes(serverState.db);
    sqlite3_finalize(stmt);

    return changes > 0;
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

// 获取用户资源
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

// 简单的JSON解析函数（支持字符串和数字类型）
std::map<std::string, std::string> parseJSON(const std::string& jsonStr) {
    std::map<std::string, std::string> result;
    std::string::size_type pos = 1; // 跳过开头的 { 字符

    while (pos < jsonStr.size()) {
        // 跳过空白和逗号
        while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\t' || jsonStr[pos] == ',' || jsonStr[pos] == '\n' || jsonStr[pos] == '\r')) {
            pos++;
        }
        if (pos >= jsonStr.size()) break;

        // 找到键的开始
        if (jsonStr[pos] != '"') break;
        std::string::size_type keyStart = pos;
        std::string::size_type keyEnd = jsonStr.find('"', keyStart + 1);
        if (keyEnd == std::string::npos)
            break;

        // 提取键
        std::string key = jsonStr.substr(keyStart + 1, keyEnd - keyStart - 1);

        // 找到冒号
        std::string::size_type colonPos = jsonStr.find(':', keyEnd + 1);
        if (colonPos == std::string::npos)
            break;

        // 找到值的开始
        std::string::size_type valueStart = colonPos + 1;
        while (valueStart < jsonStr.size() && (jsonStr[valueStart] == ' ' || jsonStr[valueStart] == '\t')) {
            valueStart++;
        }
        if (valueStart >= jsonStr.size()) break;

        std::string value;

        // 判断值是字符串（以引号开头）还是数字
        if (jsonStr[valueStart] == '"') {
            // 字符串值：找到结束引号
            std::string::size_type valueEnd = jsonStr.find('"', valueStart + 1);
            if (valueEnd == std::string::npos)
                break;
            value = jsonStr.substr(valueStart + 1, valueEnd - valueStart - 1);
            pos = valueEnd + 1;
        }
        else {
            // 数字值：找到值的结束（逗号、右括号或空白）
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

        // 将键值对添加到结果
        result[key] = value;
    }

    return result;
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

    // 初始化libwebsockets
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));
    info.port = 8080;
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;

    // 绑定到指定IP地址（笔记本电脑的IPv4地址）
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