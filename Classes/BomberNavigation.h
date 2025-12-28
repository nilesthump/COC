#pragma once
#include "AttackerNavigation.h"

class BomberNavigation : public AttackerNavigation
{
public:
    // 重写目标搜索，实现炸弹人“优先找墙”逻辑
    BattleUnit* FindTarget(BattleUnit* self, const std::vector<BattleUnit*>& allTargets, float searchRadius) override;

    std::string GetNavigationType() const override { return "BomberNavigation"; }
};