//normal作为通用行为
#ifndef _ATTACKER_NORMAL_BEHAVIOR_H_
#define _ATTACKER_NORMAL_BEHAVIOR_H_
#include "UnitBehavior.h"
class BattleUnit;

class AttackerNormalBehavior :public UnitBehavior
{
public:
    bool CanAttack(BattleUnit* attacker, BattleUnit* defender)override;
    void OnAttack(BattleUnit* attacker, BattleUnit* defender)override;
    virtual void OnUpdate(BattleUnit* unit, float dt) override;
    virtual void OnDeath(BattleUnit* unit) override;
    virtual void OnDamageTaken(BattleUnit* unit, float damage, BattleUnit* attacker) override;
    virtual std::string GetBehaviorType() const override { return "AttackerNormal"; }
};
#endif

