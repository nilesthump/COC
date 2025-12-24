#include "SessionManager.h"

// 初始化单例实例为nullptr
SessionManager* SessionManager::instance = nullptr;

// 私有构造函数，初始化登录状态为false，当前用户名为空
SessionManager::SessionManager() : isLoggedIn(false), currentUsername("") {}

// 获取单例实例
SessionManager* SessionManager::getInstance() 
{
    if (instance == nullptr) 
    {
        instance = new SessionManager();
    }
    return instance;
}

// 销毁单例实例
void SessionManager::destroyInstance() 
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

// 登录，设置登录状态为true，保存当前用户名
void SessionManager::login(const std::string& username) 
{
    isLoggedIn = true;
    currentUsername = username;
}

// 登出，设置登录状态为false，清空当前用户名
void SessionManager::logout()
{
    isLoggedIn = false;
    currentUsername = "";
}

// 检查是否登录
bool SessionManager::getIsLoggedIn() const 
{
    return isLoggedIn;
}

// 获取当前登录用户名
std::string SessionManager::getCurrentUsername() const 
{
    return currentUsername;
}
