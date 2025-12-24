#include"AttackerNavigation.h"

BattleUnit* AttackerNavigation::FindTarget(BattleUnit* self, const std::vector<BattleUnit*>& allTargets)
{
    auto priority = self->GetState().GetPreferredTarget();
    BattleUnit* bestTarget = nullptr;
    float minDistance = FLT_MAX;

    // 第一次尝试：寻找符合偏好的目标（如：巨人找防御塔）
    for (auto target : allTargets)
    {
        if (!target->IsAlive() || !target->GetState().IsDefender()) continue;

        // 1. 物理检查：地对地，地对空？
        if (!CanPhysicallyAttack(self, target)) continue;

        // 2. 偏好检查
        bool isPreferred = false;
        if (priority == TargetPriority::ANY) isPreferred = true;
        else if (priority == TargetPriority::DEFENSES) isPreferred = true;
        else if (priority == TargetPriority::RESOURCES && target->GetState().IsResourceBuilding()) isPreferred = true;

        if (isPreferred)
        {
            float dist = CalculateDistance(self, target);
            if (dist < minDistance) { minDistance = dist; bestTarget = target; }
        }
    }

    // 第二次尝试：如果偏好目标没了，退化到寻找最近的“任何可攻击建筑”
    //! 这里不知道炸弹人炸不炸别的
    if (!bestTarget && priority != TargetPriority::ANY)
    {
        minDistance = FLT_MAX;
        for (auto target : allTargets)
        {
            if (!target->IsAlive() || !target->GetState().IsDefender() || !CanPhysicallyAttack(self, target)) continue;
            float dist = CalculateDistance(self, target);
            if (dist < minDistance) { minDistance = dist; bestTarget = target; }
        }
    }
    return bestTarget;
}

void AttackerNavigation::CalculateMove(BattleUnit* self, BattleUnit* target, float deltaTime)
{
    if (!target) return;

    cocos2d::Vec2 currentPos(self->GetPositionX(), self->GetPositionY());
    cocos2d::Vec2 targetPos(target->GetPositionX(), target->GetPositionY());

    //! 简单的直线寻路（A*逻辑通常建议放在单独的PathFinder中，这里先跑通逻辑）
    cocos2d::Vec2 direction = targetPos - currentPos;
    direction.normalize();

    float moveDist = self->GetMoveSpeed() * deltaTime;
    cocos2d::Vec2 nextPos = currentPos + direction * moveDist;

    self->SetPositionAttacker(nextPos.x, nextPos.y);
}

bool AttackerNavigation::IsInAttackRange(BattleUnit* self, BattleUnit* target)
{
    if (!target) return false;
    return CalculateDistance(self, target) <= self->GetAttackDistance();
}