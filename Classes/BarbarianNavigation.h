#ifndef _BARBARIAN_NAVIGATION_H_
#define _BARBARIAN_NAVIGATION_H_
#include "UnitNavigation.h"
#include "BattleUnit.h"
#include <cmath>
#include <climits>
#include <string>
class BarbarianNavigation : public UnitNavigation
{
public:
    BattleUnit* FindTarget(BattleUnit* self,
        const std::vector<BattleUnit*>& allTargets) override
    {
        BattleUnit* nearest = nullptr;
        float minDistance = FLT_MAX;

        for (auto target : allTargets)
        {
            if (!target->IsAlive()) continue;

            float dist = CalculateDistance(self, target);
            if (dist < minDistance)
            {
                minDistance = dist;
                nearest = target;
            }
        }
        return nearest;
    }

    void CalculateMove(BattleUnit* self, BattleUnit* target, float deltaTime) override
    {
        if (!target) return;

        // 使用UnitState的接口获取速度
        float speed = self->GetState().GetMoveSpeed();  // 而不是GetBaseData().move_speed

        float dx = target->GetPositionX() - self->GetPositionX();
        float dy = target->GetPositionY() - self->GetPositionY();
        float distance = sqrt(dx * dx + dy * dy);

        if (distance > 0.01f)
        {
            float moveX = (dx / distance) * speed * deltaTime;
            float moveY = (dy / distance) * speed * deltaTime;

            self->SetPosition(
                self->GetPositionX() + moveX,
                self->GetPositionY() + moveY
            );
        }
    }


    bool IsInAttackRange(BattleUnit* self, BattleUnit* target) override
    {
        if (!target) return false;

        float distance = CalculateDistance(self, target);
        // 使用UnitState的接口获取攻击距离
        float attackRange = self->GetState().GetAttackDistance();
        return distance <= attackRange;
    }
    std::string GetNavigationType() const override
    {
        return "Barbarian";
    }

private:
    float CalculateDistance(BattleUnit* a, BattleUnit* b)
    {
        float dx = a->GetPositionX() - b->GetPositionX();
        float dy = a->GetPositionY() - b->GetPositionY();
        return sqrt(dx * dx + dy * dy);
    }
};
#endif