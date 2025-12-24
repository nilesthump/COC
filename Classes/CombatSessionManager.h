//战斗配置单例，管理一次完整的战斗会话
#pragma once
#include "BattleTypes.h"
#include <string>

class CombatSessionManager
{
public:
    static CombatSessionManager* getInstance()
    {
        static CombatSessionManager instance;
        return &instance;
    }

    // 兵力配置
    UnitInventory attackerArmy;

    // 地图建筑布局（包含所有防御、资源、建筑）
    std::vector<BuildingSnapshot> mapBuildings;

    // 环境配置
    std::string mapBackgroundPath;

    void reset()
    {
        attackerArmy.clear();
        mapBuildings.clear();
        mapBackgroundPath = "normal(winter).jpg";
    }

private:
    CombatSessionManager() { reset(); } // 构造函数初始化
};