#pragma once
#include"UnitNavigation.h"

class DefenderNavigation : public UnitNavigation
{
public:
    virtual std::string GetNavigationType() const override
    {
        return "DefenderNormalNavigation";
    }

    //只找攻击范围内的目标
    BattleUnit* FindTarget(BattleUnit* self, const std::vector<BattleUnit*>& allTargets, float searchRadius)
    {
        if (!self || allTargets.empty()) return nullptr;

        BattleUnit* bestTarget = nullptr;
        float minDistance = FLT_MAX;

        for (auto target : allTargets)
        {
            if (!target || !target->IsAlive() || !target->GetState().IsAttacker()) continue;
            if (!CanPhysicallyAttack(self, target)) continue;

            float dist = CalculateDistance(self, target);

            // 防御建筑只看半径（射程）内的敌人
            if (dist <= searchRadius && dist < minDistance)
            {
                minDistance = dist;
                bestTarget = target;
            }
        }

        return bestTarget;
    }

    void CalculateMove(BattleUnit* self, BattleUnit* target, float deltaTime,IndexSystem* indexSys) override
    {
        // 建筑不会动，留空即可
    }

    bool IsInAttackRange(BattleUnit* self, BattleUnit* target) override
    {
        return CalculateDistance(self, target) <= self->GetAttackDistance();
    }
};