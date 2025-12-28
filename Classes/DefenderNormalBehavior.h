#ifndef _DEFENDER_NORMAL_BEHAVIOR_H_
#define _DEFENDER_NORMAL_BEHAVIOR_H_

#include "UnitBehavior.h"

class DefenderNormalBehavior : public UnitBehavior
{
public:
    virtual void OnUpdate(BattleUnit* unit, float dt) override;
    virtual void OnDeath(BattleUnit* unit) override;
    virtual void OnDamageTaken(BattleUnit* unit, float damage, BattleUnit* attacker) override;
    virtual void OnAttack(BattleUnit* attacker, BattleUnit* defender) override;
    virtual std::string GetBehaviorType() const override { return "DefenderNormal"; }
};

#endif 