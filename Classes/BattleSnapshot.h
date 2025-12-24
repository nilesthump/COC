//存储一些组件
#pragma once
#include "UnitEnums.h"
#include <map>
#include <vector>
#include "math/Vec2.h"

// 建筑快照
struct BuildingSnapshot
{
    BuildingType type;
    int level;
    cocos2d::Vec2 logicalPos; // 存的是逻辑中心坐标 (-0.5 到 49.5)
};

// 战斗开始前的完整数据包
struct BattleStartParams
{
    std::vector<BuildingSnapshot> buildings; // 敌方阵型
    std::map<UnitType, int> attackerInventory; // 我方携带的兵力
};