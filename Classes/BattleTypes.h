//存储一些组件
#pragma once
#include "UnitEnums.h"
#include <map>
#include <vector>
#include "math/Vec2.h"

// 建筑快照
struct BuildingSnapshot
{
    BuildingType type;      // 建筑枚举类型
    int level;              // 等级
    cocos2d::Vec2 gridPos;  // 逻辑网格坐标 (0-49)
};

using UnitInventory = std::map<UnitType, int>;