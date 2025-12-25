#ifndef __SQLITE_MANAGER_H__
#define __SQLITE_MANAGER_H__

#include <string>
#include "cocos2d.h"
#include "sqlite3.h"

USING_NS_CC;

class SQLiteManager : public Ref {
public:
    // 获取单例实例
    static SQLiteManager* getInstance();

    // 销毁单例实例
    static void destroyInstance();

    // 初始化
    virtual bool init();

    // 初始化数据库
    bool initDatabase(const std::string& dbPath = "users.db");

    // 注册用户
    bool registerUser(const std::string& username, const std::string& password);

    // 用户登录
    bool loginUser(const std::string& username, const std::string& password);

    // 删除用户
    bool deleteUser(const std::string& username);

    // 修改密码
    bool changePassword(const std::string& username, const std::string& newPassword);

    // 检查用户是否存在
    bool userExists(const std::string& username);

    // 获取最后一次错误信息
    std::string getLastError() const;

private:
    // 私有构造函数和析构函数
    SQLiteManager();
    ~SQLiteManager();

    // 禁止拷贝构造和赋值操作
    SQLiteManager(const SQLiteManager&) = delete;
    SQLiteManager& operator=(const SQLiteManager&) = delete;

    // 打开数据库
    bool openDatabase();

    // 关闭数据库
    void closeDatabase();

    // 执行事务
    bool executeTransaction(const std::function<bool()>& transactionFunc);

    // 成员变量
    std::string _dbPath;      // 数据库路径
    sqlite3* _db;             // SQLite数据库连接
    std::string _lastError;   // 最后一次错误信息
    bool _isDatabaseOpen;     // 数据库是否打开

    // 单例实例
    static SQLiteManager* _instance;
};

#endif // __SQLITE_MANAGER_H__