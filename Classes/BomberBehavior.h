#pragma once
#include "AttackerNormalBehavior.h"

class BomberBehavior : public AttackerNormalBehavior
{
public:
    void OnAttack(BattleUnit* attacker, BattleUnit* defender) override;
    void OnUpdate(BattleUnit* unit, float dt) override;
    void OnDeath(BattleUnit* unit) override;

    std::string GetBehaviorType() const override { return "BomberBehavior"; }

private:
    void ExecuteExplosion(BattleUnit* owner, bool isActive);

    bool _isFusing = false;      // 是否点燃引信
    float _fuseTimer = 0.0f;     // 计时器
    bool _hasExploded = false;   // 爆炸锁
};