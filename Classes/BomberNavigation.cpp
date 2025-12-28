#include "BomberNavigation.h"
#include "BattleUnit.h"

BattleUnit* BomberNavigation::FindTarget(BattleUnit* self, const std::vector<BattleUnit*>& allTargets, float searchRadius)
{
    if (!self || allTargets.empty()) return nullptr;

    BattleUnit* bestWall = nullptr;
    float minDistance = FLT_MAX;

    // 第一轮：严格搜索 searchRadius 内最近的墙
    for (auto target : allTargets)
    {
        // 使用你 UnitState 里定义的 IsWall() 接口
        if (!target || !target->IsAlive() || !target->GetState().IsWall()) continue;

        float dist = CalculateDistance(self, target);
        if (dist <= searchRadius && dist < minDistance)
        {
            minDistance = dist;
            bestWall = target;
        }
    }

    if (bestWall) return bestWall;

    // 第二轮：没墙，退化到通用逻辑
    return AttackerNavigation::FindTarget(self, allTargets, searchRadius);
}