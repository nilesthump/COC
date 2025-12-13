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
        double minDistance = FLT_MAX;

        for (auto target : allTargets)
        {
            if (!target->IsAlive()) continue;

            double dist = CalculateDistance(self, target);
            if (dist < minDistance)
            {
                minDistance = dist;
                nearest = target;
            }
        }
        return nearest;
    }

    void CalculateMove(BattleUnit* self, BattleUnit* target, double deltaTime) override
    {
        if (!target) return;

        double speed = self->GetMoveSpeed();
        double dx = target->GetPositionX() - self->GetPositionX();
        double dy = target->GetPositionY() - self->GetPositionY();
        double distance = sqrt(dx * dx + dy * dy);

        if (distance > 0.01f)
        {
            double moveX = (dx / distance) * speed * deltaTime;
            double moveY = (dy / distance) * speed * deltaTime;

            self->SetPosition(
                self->GetPositionX() + moveX,
                self->GetPositionY() + moveY
            );
        }
    }


    bool IsInAttackRange(BattleUnit* self, BattleUnit* target) override
    {
        if (!target) return false;

        double distance = CalculateDistance(self, target);
        double attackRange = self->GetAttackDistance();
        return distance <= attackRange;
    }
    std::string GetNavigationType() const override
    {
        return "Barbarian";
    }

private:
    double CalculateDistance(BattleUnit* a, BattleUnit* b)
    {
        double dx = a->GetPositionX() - b->GetPositionX();
        double dy = a->GetPositionY() - b->GetPositionY();
        return sqrt(dx * dx + dy * dy);
    }
};
#endif