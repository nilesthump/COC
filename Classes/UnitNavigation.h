//12/24 重构navigation基类
#ifndef _UNIT_NAVIGATION_H_
#define _UNIT_NAVIGATION_H_
#include <vector>
#include <string>
#include <algorithm>
#include "IndexSystem.h"
#include "cocos2d.h"

class BattleUnit;

class UnitNavigation
{
public:
    virtual ~UnitNavigation() = default;
    virtual BattleUnit* FindTarget(BattleUnit* self, const std::vector<BattleUnit*>& allTargets,float searchRadius) = 0;
    virtual void CalculateMove(BattleUnit* self, BattleUnit* target, float deltaTime, IndexSystem* indexSys) = 0;
    virtual bool IsInAttackRange(BattleUnit* self, BattleUnit* target) = 0;
    virtual std::string GetNavigationType() const = 0;
    virtual void OnBattleStart(BattleUnit* unit) {};
    virtual void ClearPath() {} 
    //核心物理判定：我能打到这个目标吗？
    bool CanPhysicallyAttack(BattleUnit* attacker, BattleUnit* target);

    //核心距离计算：考虑建筑体积（点到矩形的距离）
    float CalculateDistance(BattleUnit* a, BattleUnit* b);

};
#endif