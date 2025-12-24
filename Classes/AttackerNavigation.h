//Attacker Navigation通用基类
#pragma once
#include "UnitNavigation.h"
#include "BattleUnit.h"

class AttackerNavigation : public UnitNavigation
{
public:
    BattleUnit* FindTarget(BattleUnit* self, const std::vector<BattleUnit*>& allTargets) override;
    
    void CalculateMove(BattleUnit* self, BattleUnit* target, float deltaTime) override;

    bool IsInAttackRange(BattleUnit* self, BattleUnit* target) override;
    
    std::string GetNavigationType() const override { return "AttackerNormalNavigation"; }
};