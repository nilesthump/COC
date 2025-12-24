//战斗配置单例，管理一次完整的战斗会话
#pragma once
#include "BattleSnapshot.h"
#include <string>

class CombatSessionManager
{
public:
    static CombatSessionManager* getInstance()
    {
        static CombatSessionManager instance;
        return &instance;
    }

    BattleStartParams battle_start_params;

    // 环境配置
    std::string mapBackgroundPath;

    void reset()
    {
        battle_start_params.attackerInventory.clear();
        battle_start_params.buildings.clear();
        mapBackgroundPath = "normal(winter).jpg";
    }

private:
    CombatSessionManager() { reset(); } // 构造函数初始化
};