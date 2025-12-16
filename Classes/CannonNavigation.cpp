#include "CannonNavigation.h"
#include "BattleUnit.h"
#include "cocos2d.h"
#include <cmath>
#include <vector>
#include <string>
#include <limits>

CannonNavigation::CannonNavigation()
{}

CannonNavigation::~CannonNavigation()
{}

BattleUnit* CannonNavigation::FindTarget(BattleUnit* self, const std::vector<BattleUnit*>& allTargets)
{
    if (allTargets.empty())
        return nullptr;
    BattleUnit* nearestTarget = nullptr;
    double minDistance = std::numeric_limits<double>::max();

    // 寻找最近的目标
    for (auto target : allTargets)
    {
        if (!target || !target->IsAlive()) 
            continue;

        double distance = CalculateDistance(self, target);
        if (distance < minDistance)
        {
            minDistance = distance;
            nearestTarget = target;
        }
    }

    return nearestTarget;
}

void CannonNavigation::CalculateMove(BattleUnit* self, BattleUnit* target, double deltaTime)
{
    // 加农炮固定，无需移动
    (void)self;
    (void)target;
    (void)deltaTime;
}

bool CannonNavigation::IsInAttackRange(BattleUnit* self, BattleUnit* target)
{
    if (!self || !target)
        return false;

    double distance = CalculateDistance(self, target);
    double attackRange = self->GetAttackDistance();
    // 攻击范围现在表示格子数
    return distance <= attackRange;
}

std::string CannonNavigation::GetNavigationType() const
{
    return "Cannon";
}

double CannonNavigation::CalculateDistance(BattleUnit* a, BattleUnit* b)
{
    if (!a || !b)
        return std::numeric_limits<double>::max();

    // 直接使用屏幕坐标计算欧几里得距离
    double dx = a->GetPositionX() - b->GetPositionX();
    double dy = a->GetPositionY() - b->GetPositionY();
    return sqrt(dx * dx + dy * dy);
}