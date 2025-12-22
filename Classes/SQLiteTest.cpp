#include "cocos2d.h"
#include "SQLiteTest.h"
#include "sqlite3.h"

USING_NS_CC;

SQLiteTest::SQLiteTest() {
    // 创建或打开数据库
    sqlite3 *db;
    int rc = sqlite3_open("test.db", &db);
    
    if (rc) {
        CCLOG("无法打开数据库: %s", sqlite3_errmsg(db));
    } else {
        CCLOG("成功打开数据库");
        
        // 创建表
        const char *sql = "CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT);";
        char *errMsg = nullptr;
        
        rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
        
        if (rc != SQLITE_OK) {
            CCLOG("创建表失败: %s", errMsg);
            sqlite3_free(errMsg);
        } else {
            CCLOG("成功创建表");
            
            // 插入数据
            sql = "INSERT INTO test_table (name) VALUES ('SQLite Test');";
            rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
            
            if (rc != SQLITE_OK) {
                CCLOG("插入数据失败: %s", errMsg);
                sqlite3_free(errMsg);
            } else {
                CCLOG("成功插入数据");
            }
        }
        
        // 关闭数据库
        sqlite3_close(db);
    }
}

SQLiteTest::~SQLiteTest() {
}