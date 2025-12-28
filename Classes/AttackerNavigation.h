//Attacker Navigation通用基类
#pragma once
#include "UnitNavigation.h"
#include "BattleUnit.h"

class AttackerNavigation : public UnitNavigation
{
public:
    
    void CalculateMove(BattleUnit* self, BattleUnit* target, float deltaTime,IndexSystem* indexSys) override;
    BattleUnit* FindTarget(BattleUnit* self, const std::vector<BattleUnit*>& allTargets, float searchRadius)override;
    bool IsInAttackRange(BattleUnit* self, BattleUnit* target) override;
    
    std::string GetNavigationType() const override { return "AttackerNormalNavigation"; }

private:
    std::vector<cocos2d::Vec2> _currentPath; // A* 算出的逻辑路径点列表
    BattleUnit* _lastTarget = nullptr;       // 记录上一个目标，用于判断是否需要重新寻路
    float _pathRefreshTimer = 0.0f;
    const float REFRESH_INTERVAL = 0.5f; // 每 0.5 秒重新寻路一次
};