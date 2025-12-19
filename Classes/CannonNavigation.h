#ifndef _CANNON_NAVIGATION_H_
#define _CANNON_NAVIGATION_H_
#include "UnitNavigation.h"

class BattleUnit;

class CannonNavigation : public UnitNavigation
{
public:
    CannonNavigation();
    ~CannonNavigation();
    // 寻找目标
    BattleUnit* FindTarget(BattleUnit* self,
        const std::vector<BattleUnit*>& allTargets) override;

    // 计算移动（防御者固定，无需移动）
    void CalculateMove(BattleUnit* self,
        BattleUnit* target,
        float deltaTime) override;

    // 判断是否在攻击范围内
    bool IsInAttackRange(BattleUnit* self, BattleUnit* target) override;

    // 获取导航类型
    std::string GetNavigationType() const override;
    float CalculateDistance(BattleUnit* a, BattleUnit* b);

};

#endif