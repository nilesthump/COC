#include "SQLiteManager.h"
#include "cocos2d.h"

USING_NS_CC;

// 初始化单例实例
SQLiteManager* SQLiteManager::_instance = nullptr;

SQLiteManager::SQLiteManager() : _db(nullptr), _isDatabaseOpen(false) {}

SQLiteManager::~SQLiteManager()
{
    closeDatabase();
}

SQLiteManager* SQLiteManager::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new SQLiteManager();
        if (!_instance->init())
        {
            delete _instance;
            _instance = nullptr;
        }
    }
    return _instance;
}

void SQLiteManager::destroyInstance()
{
    if (_instance != nullptr)
    {
        delete _instance;
        _instance = nullptr;
    }
}

bool SQLiteManager::init()
{
    // 默认初始化数据库路径
    return initDatabase();
}

bool SQLiteManager::initDatabase(const std::string& dbPath)
{
    _dbPath = dbPath;

    if (!openDatabase())
    {
        return false;
    }

    // 创建用户表（如果不存在）
    const char* sql = "CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password TEXT);";
    char* errMsg = nullptr;
    int rc = sqlite3_exec(_db, sql, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK)
    {
        _lastError = errMsg;
        sqlite3_free(errMsg);
        closeDatabase();
        return false;
    }

    closeDatabase();
    return true;
}

bool SQLiteManager::openDatabase()
{
    if (_isDatabaseOpen)
    {
        return true;
    }

    int rc = sqlite3_open(_dbPath.c_str(), &_db);
    if (rc)
    {
        _lastError = sqlite3_errmsg(_db);
        _db = nullptr;
        return false;
    }

    _isDatabaseOpen = true;
    return true;
}

void SQLiteManager::closeDatabase()
{
    if (_isDatabaseOpen && _db != nullptr)
    {
        sqlite3_close(_db);
        _db = nullptr;
        _isDatabaseOpen = false;
    }
}

bool SQLiteManager::executeTransaction(const std::function<bool()>& transactionFunc)
{
    if (!openDatabase())
    {
        return false;
    }

    // 开始事务
    if (sqlite3_exec(_db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        _lastError = sqlite3_errmsg(_db);
        closeDatabase();
        return false;
    }

    bool success = true;

    try
    {
        // 执行事务函数
        success = transactionFunc();
    }
    catch (...)
    {
        success = false;
    }

    // 根据执行结果提交或回滚事务
    if (success)
    {
        if (sqlite3_exec(_db, "COMMIT TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK)
        {
            _lastError = sqlite3_errmsg(_db);
            success = false;
        }
    }
    else
    {
        sqlite3_exec(_db, "ROLLBACK TRANSACTION;", nullptr, nullptr, nullptr);
    }

    closeDatabase();
    return success;
}

bool SQLiteManager::registerUser(const std::string& username, const std::string& password)
{
    // 检查用户是否已存在
    if (userExists(username))
    {
        _lastError = "用户已存在";
        return false;
    }

    return executeTransaction([this, username, password]() -> bool
        {
            const char* sql = "INSERT INTO users (username, password) VALUES (?, ?);";
            sqlite3_stmt* stmt;

            int rc = sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr);
            if (rc != SQLITE_OK)
            {
                _lastError = sqlite3_errmsg(_db);
                return false;
            }

            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

            rc = sqlite3_step(stmt);
            bool success = (rc == SQLITE_DONE);

            if (!success)
            {
                _lastError = sqlite3_errmsg(_db);
            }

            sqlite3_finalize(stmt);
            return success;
        });
}

bool SQLiteManager::loginUser(const std::string& username, const std::string& password)
{
    if (!openDatabase())
    {
        return false;
    }

    const char* sql = "SELECT password FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        _lastError = sqlite3_errmsg(_db);
        closeDatabase();
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    bool loginSuccess = false;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char* dbPassword = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (dbPassword != nullptr && password == dbPassword)
        {
            loginSuccess = true;
        }
    }

    sqlite3_finalize(stmt);
    closeDatabase();

    if (!loginSuccess)
    {
        _lastError = "用户名或密码错误";
    }

    return loginSuccess;
}

bool SQLiteManager::deleteUser(const std::string& username)
{
    return executeTransaction([this, username]() -> bool
        {
            sqlite3_stmt* stmt;

            const char* sqlUsers = "DELETE FROM users WHERE username = ?;";
            int rc = sqlite3_prepare_v2(_db, sqlUsers, -1, &stmt, nullptr);
            if (rc != SQLITE_OK)
            {
                _lastError = sqlite3_errmsg(_db);
                return false;
            }
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE)
            {
                _lastError = sqlite3_errmsg(_db);
                sqlite3_finalize(stmt);
                return false;
            }
            sqlite3_finalize(stmt);

            const char* sqlResources = "DELETE FROM resources WHERE username = ?;";
            rc = sqlite3_prepare_v2(_db, sqlResources, -1, &stmt, nullptr);
            if (rc != SQLITE_OK)
            {
                _lastError = sqlite3_errmsg(_db);
                return false;
            }
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE)
            {
                _lastError = sqlite3_errmsg(_db);
                sqlite3_finalize(stmt);
                return false;
            }
            sqlite3_finalize(stmt);

            const char* sqlBuildings = "DELETE FROM buildings WHERE username = ?;";
            rc = sqlite3_prepare_v2(_db, sqlBuildings, -1, &stmt, nullptr);
            if (rc != SQLITE_OK)
            {
                _lastError = sqlite3_errmsg(_db);
                return false;
            }
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE)
            {
                _lastError = sqlite3_errmsg(_db);
                sqlite3_finalize(stmt);
                return false;
            }
            sqlite3_finalize(stmt);

            return true;
        });
}

bool SQLiteManager::changePassword(const std::string& username, const std::string& newPassword)
{
    return executeTransaction([this, username, newPassword]() -> bool
        {
            const char* sql = "UPDATE users SET password = ? WHERE username = ?;";
            sqlite3_stmt* stmt;

            int rc = sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr);
            if (rc != SQLITE_OK)
            {
                _lastError = sqlite3_errmsg(_db);
                return false;
            }

            sqlite3_bind_text(stmt, 1, newPassword.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);

            rc = sqlite3_step(stmt);
            bool success = (rc == SQLITE_DONE);

            if (!success)
            {
                _lastError = sqlite3_errmsg(_db);
            }

            sqlite3_finalize(stmt);
            return success;
        });
}

bool SQLiteManager::userExists(const std::string& username)
{
    if (!openDatabase())
    {
        return false;
    }

    const char* sql = "SELECT username FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        _lastError = sqlite3_errmsg(_db);
        closeDatabase();
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);

    sqlite3_finalize(stmt);
    closeDatabase();

    return exists;
}

std::string SQLiteManager::getLastError() const
{
    return _lastError;
}