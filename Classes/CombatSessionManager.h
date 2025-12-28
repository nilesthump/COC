//战斗配置单例，管理一次完整的战斗会话
#pragma once
#include "cocos2d.h"
#include "BattleSnapshot.h"
#include <string>

class CombatSessionManager
{
public:
    BattleStartParams battle_start_params;
    std::string mapBackgroundPath;
    static CombatSessionManager* getInstance()
    {
        static CombatSessionManager instance;
        return &instance;
    }
    void reset()
    {
        battle_start_params.attackerInventory.clear();
        battle_start_params.buildings.clear();
        mapBackgroundPath = "normal(winter).jpg";
    }
  
    void setAttackerInventory(const std::map<UnitType, int>& inventory)
    {
        battle_start_params.attackerInventory = inventory;
    }
    void setBuildings(const std::vector<BuildingSnapshot>& buildings)
    {
        battle_start_params.buildings = buildings;
    }

private:
    CombatSessionManager() { reset(); } // 构造函数初始化
};