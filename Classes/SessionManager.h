#ifndef __SESSION_MANAGER_H__
#define __SESSION_MANAGER_H__

#include <string>

enum class LoginType {
    NONE,
    GUEST,
    ACCOUNT
};

class SessionManager 
{
private:
    // 私有构造函数，防止外部实例化
    SessionManager();
    // 单例实例
    static SessionManager* instance;

    // 登录状态
    bool isLoggedIn;
    // 当前登录用户名
    std::string currentUsername;
    LoginType loginType;

    int _gold;
    int _elixir;
    int _gems;
    bool _hasResourceData;

public:
    // 获取单例实例
    static SessionManager* getInstance();

    // 销毁单例实例
    static void destroyInstance();

    // 登录
    void login(const std::string& username, LoginType type = LoginType::ACCOUNT);

    // 登出
    void logout();

    // 检查是否登录
    bool getIsLoggedIn() const;

    // 获取当前登录用户名
    std::string getCurrentUsername() const;
    LoginType getLoginType() const;
    bool isAccountLogin() const;
    bool isGuestLogin() const;

    void setResourceData(int gold, int elixir, int gems);
    void getResourceData(int& gold, int& elixir, int& gems) const;
    bool hasResourceData() const;
    void clearResourceData();
};

#endif // __SESSION_MANAGER_H__
