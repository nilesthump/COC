//A*算法抽象数学逻辑，通用情况，只不过找目标不一致。炸弹人和气球兵特殊不调用即可
#pragma once
#include<vector>
#include "cocos2d.h"
#include "IndexSystem.h"

USING_NS_CC;

class UnitNavigationLogic
{
public:
    static std::vector<cocos2d::Vec2> FindPath(
        const cocos2d::Vec2& startLogPos,
        const cocos2d::Vec2& endLogPos,
        const IndexSystem* indexSys,
        bool canBreakWall);
};

