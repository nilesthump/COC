#ifndef __SESSION_MANAGER_H__
#define __SESSION_MANAGER_H__

#include <string>
#include <vector>

enum class LoginType {
    NONE,
    GUEST,
    ACCOUNT
};

struct ProductionData {
    std::string buildingType;
    float x;
    float y;
    int level;
    int currentStock;
    long long lastProductionTime;
};

struct UpgradeData {
    std::string buildingType;
    float x;
    float y;
    int targetLevel;
    long long startTime;
    int duration;
    int remainingTime;
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

    std::vector<ProductionData> _productionData;
    std::vector<UpgradeData> _upgradeData;

    bool _isProducingGold;
    bool _isProducingElixir;
    long long _goldProductionCompletionTime;
    long long _elixirProductionCompletionTime;

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

    void setProductionData(const std::vector<ProductionData>& data);
    bool getProductionData(std::vector<ProductionData>& data) const;
    void clearProductionData();

    void setUpgradeData(const std::vector<UpgradeData>& data);
    bool getUpgradeData(std::vector<UpgradeData>& data) const;
    bool getUpgradeDataForBuilding(float x, float y, UpgradeData& data) const;
    void clearUpgradeData();

    bool isProducingGold() const;
    bool isProducingElixir() const;
    void startProducingGold();
    void stopProducingGold();
    void startProducingElixir();
    void stopProducingElixir();
    long long getGoldProductionCompletionTime() const;
    long long getElixirProductionCompletionTime() const;
};

#endif // __SESSION_MANAGER_H__
