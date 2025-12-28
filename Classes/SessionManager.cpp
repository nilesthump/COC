#include "SessionManager.h"

// 初始化单例实例为nullptr
SessionManager* SessionManager::instance = nullptr;

// 私有构造函数，初始化登录状态为false，当前用户名为空
SessionManager::SessionManager() : 
    isLoggedIn(false),
    currentUsername(""), 
    loginType(LoginType::NONE),
    _gold(0),
    _elixir(0),
    _gems(0),
    _hasResourceData(false),
    _isProducingGold(false),
    _isProducingElixir(false),
    _goldProductionCompletionTime(0),
    _elixirProductionCompletionTime(0) {
}

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
void SessionManager::login(const std::string& username, LoginType type)
{
    isLoggedIn = true;
    currentUsername = username;
    loginType = type;
}

// 登出，设置登录状态为false，清空当前用户名
void SessionManager::logout()
{
    isLoggedIn = false;
    currentUsername = "";
    loginType = LoginType::NONE;
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

LoginType SessionManager::getLoginType() const
{
    return loginType;
}

bool SessionManager::isAccountLogin() const
{
    return isLoggedIn && loginType == LoginType::ACCOUNT;
}

bool SessionManager::isGuestLogin() const
{
    return isLoggedIn && loginType == LoginType::GUEST;
}

void SessionManager::setResourceData(int gold, int elixir, int gems)
{
    _gold = gold;
    _elixir = elixir;
    _gems = gems;
    _hasResourceData = true;
}

void SessionManager::getResourceData(int& gold, int& elixir, int& gems) const
{
    gold = _gold;
    elixir = _elixir;
    gems = _gems;
}

bool SessionManager::hasResourceData() const
{
    return _hasResourceData;
}

void SessionManager::clearResourceData()
{
    _gold = 0;
    _elixir = 0;
    _gems = 0;
    _hasResourceData = false;
}

void SessionManager::setProductionData(const std::vector<ProductionData>& data)
{
    _productionData = data;
}

bool SessionManager::getProductionData(std::vector<ProductionData>& data) const
{
    data = _productionData;
    return !_productionData.empty();
}

void SessionManager::clearProductionData()
{
    _productionData.clear();
}

void SessionManager::setUpgradeData(const std::vector<UpgradeData>& data)
{
    _upgradeData = data;
}

bool SessionManager::getUpgradeData(std::vector<UpgradeData>& data) const
{
    data = _upgradeData;
    return !_upgradeData.empty();
}

bool SessionManager::getUpgradeDataForBuilding(float x, float y, UpgradeData& data) const
{
    for (const auto& upgrade : _upgradeData) {
        if (std::abs(upgrade.x - x) < 0.01f && std::abs(upgrade.y - y) < 0.01f) {
            data = upgrade;
            return true;
        }
    }
    return false;
}

void SessionManager::clearUpgradeData()
{
    _upgradeData.clear();
}

bool SessionManager::isProducingGold() const
{
    return _isProducingGold;
}

bool SessionManager::isProducingElixir() const
{
    return _isProducingElixir;
}

void SessionManager::startProducingGold()
{
    _isProducingGold = true;
}

void SessionManager::stopProducingGold()
{
    _isProducingGold = false;
}

void SessionManager::startProducingElixir()
{
    _isProducingElixir = true;
}

void SessionManager::stopProducingElixir()
{
    _isProducingElixir = false;
}

long long SessionManager::getGoldProductionCompletionTime() const
{
    return _goldProductionCompletionTime;
}

long long SessionManager::getElixirProductionCompletionTime() const
{
    return _elixirProductionCompletionTime;
}
