#pragma once
#include"UnitNavigation.h"

class DefenderNavigation : public UnitNavigation
{
public:
    virtual std::string GetNavigationType() const override
    {
        return "DefenderNormalNavigation";
    }
    BattleUnit* FindTarget(BattleUnit* self, const std::vector<BattleUnit*>& allTargets) override
    {
        // 防御者不需要偏好，只找最近的攻击者
        BattleUnit* bestTarget = nullptr;
        float minDistance = self->GetAttackDistance(); // 必须在射程内

        for (auto target : allTargets)
        {
            if (!target->IsAlive() || !target->GetState().IsAttacker()) continue;

            float dist = CalculateDistance(self, target);
            if (dist < minDistance) { minDistance = dist; bestTarget = target; }
        }
        return bestTarget;
    }

    void CalculateMove(BattleUnit* self, BattleUnit* target, float deltaTime) override
    {
        // 建筑不会动，留空即可
    }

    bool IsInAttackRange(BattleUnit* self, BattleUnit* target) override
    {
        return CalculateDistance(self, target) <= self->GetAttackDistance();
    }
};