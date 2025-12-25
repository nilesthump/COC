#include <iostream>
#include <string>
#include <vector>
#include <libwebsockets.h>
#include "sqlite3.h"
#include <sstream>
#include <map>

// 函数原型声明
std::map<std::string, std::string> parseJSON(const std::string& jsonStr);
bool registerUser(const std::string& username, const std::string& password);
bool loginUser(const std::string& username, const std::string& password);
bool deleteUser(const std::string& username);
bool changePassword(const std::string& username, const std::string& newPassword);
bool initDatabase();

// 客户端连接结构
struct ClientConnection {
    struct lws* wsi;
    bool isConnected;
    std::vector<unsigned char> buffer;
};

// 服务器状态
struct ServerState {
    std::vector<ClientConnection*> clients;
    bool isRunning;
    sqlite3* db; // SQLite数据库连接
};

static ServerState serverState;

// WebSocket回调处理函数
static int callback_server(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len) {
    ClientConnection* client = (ClientConnection*)user;

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            std::cout << "Client connected" << std::endl;
            client->isConnected = true;
            client->wsi = wsi;
            serverState.clients.push_back(client);
            break;

        case LWS_CALLBACK_RECEIVE:
        {
            std::string receivedData((char*)in, len);
            std::cout << "Received data: " << receivedData << std::endl;

            // 解析JSON数据
            std::map<std::string, std::string> jsonData = parseJSON(receivedData);
            std::string action = jsonData["action"];
            std::string response;

            // 根据action执行相应的操作
            if (action == "register") {
                std::string username = jsonData["username"];
                std::string password = jsonData["password"];
                if (registerUser(username, password)) {
                    response = R"("status":"success", "message":"注册成功")";
                }
                else {
                    response = R"("status":"error", "message":"注册失败")";
                }
            }
            else if (action == "login") {
                std::string username = jsonData["username"];
                std::string password = jsonData["password"];
                if (loginUser(username, password)) {
                    response = R"("status":"success", "message":"登录成功")";
                }
                else {
                    response = R"("status":"error", "message":"登录失败")";
                }
            }
            else if (action == "delete") {
                std::string username = jsonData["username"];
                if (deleteUser(username)) {
                    response = R"("status":"success", "message":"删除成功")";
                }
                else {
                    response = R"("status":"error", "message":"删除失败")";
                }
            }
            else if (action == "changePassword") {
                std::string username = jsonData["username"];
                std::string newPassword = jsonData["newPassword"];
                if (changePassword(username, newPassword)) {
                    response = R"("status":"success", "message":"密码修改成功")";
                }
                else {
                    response = R"("status":"error", "message":"密码修改失败")";
                }
            }
            else {
                response = R"("status":"error", "message":"未知动作")";
            }

            // 构建JSON响应
            std::string jsonResponse = "{" + response + "}";

            // 发送响应给客户端
            std::vector<unsigned char> buf(LWS_PRE + jsonResponse.size() + 1);
            memcpy(&buf[LWS_PRE], jsonResponse.c_str(), jsonResponse.size());
            lws_write(wsi, &buf[LWS_PRE], jsonResponse.size(), LWS_WRITE_TEXT);

            break;
        }

        case LWS_CALLBACK_CLOSED:
            std::cout << "Client disconnected" << std::endl;
            client->isConnected = false;
            // 从客户端列表中移除
            serverState.clients.erase(
                std::remove_if(serverState.clients.begin(), serverState.clients.end(),
                    [client](const ClientConnection* c) { return c == client; }),
                serverState.clients.end());
            delete client;
            break;

        default:
            break;
    }

    return 0;
}

// 协议配置
static struct lws_protocols protocols[] = {
    {
        "server-protocol",
        callback_server,
        sizeof(ClientConnection),
        4096,
    },
    { NULL, NULL, 0, 0 }
};

// 初始化SQLite数据库
bool initDatabase() {
    int rc = sqlite3_open("users.db", &serverState.db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(serverState.db) << std::endl;
        sqlite3_close(serverState.db);
        return false;
    }

    // 创建用户表
    const char* createTableSQL = "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"
        "password TEXT NOT NULL);";

    char* errorMsg = nullptr;
    rc = sqlite3_exec(serverState.db, createTableSQL, nullptr, nullptr, &errorMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
        sqlite3_close(serverState.db);
        return false;
    }

    std::cout << "Database initialized successfully" << std::endl;
    return true;
}

// 注册用户
bool registerUser(const std::string& username, const std::string& password) {
    const char* insertSQL = "INSERT INTO users (username, password) VALUES (?, ?);";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(serverState.db, insertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error: " << sqlite3_errmsg(serverState.db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "SQL step error: " << sqlite3_errmsg(serverState.db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

// 用户登录
bool loginUser(const std::string& username, const std::string& password) {
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

// 简单的JSON解析函数
std::map<std::string, std::string> parseJSON(const std::string& jsonStr) {
    std::map<std::string, std::string> result;
    std::string::size_type pos = 1; // 跳过开头的 { 字符

    while (pos < jsonStr.size()) {
        // 找到键的开始
        std::string::size_type keyStart = jsonStr.find('"', pos);
        if (keyStart == std::string::npos) 
            break;

        // 找到键的结束
        std::string::size_type keyEnd = jsonStr.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) 
            break;

        // 提取键
        std::string key = jsonStr.substr(keyStart + 1, keyEnd - keyStart - 1);

        // 找到值的开始
        std::string::size_type valueStart = jsonStr.find('"', keyEnd + 1);
        if (valueStart == std::string::npos) 
            break;

        // 找到值的结束
        std::string::size_type valueEnd = jsonStr.find('"', valueStart + 1);
        if (valueEnd == std::string::npos) 
            break;

        // 提取值
        std::string value = jsonStr.substr(valueStart + 1, valueEnd - valueStart - 1);

        // 将键值对添加到结果
        result[key] = value;

        // 移动到下一个键值对的位置
        pos = valueEnd + 1;
    }

    return result;
}

int main() {
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

    struct lws_context* context = lws_create_context(&info);
    if (!context) {
        std::cerr << "Failed to create context" << std::endl;
        return -1;
    }

    std::cout << "Server started on port 8080" << std::endl;

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