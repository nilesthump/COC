// UnitBehavior.h
#ifndef UNIT_BEHAVIOR_H
#define UNIT_BEHAVIOR_H

#include <string>
#include "cocos2d.h"

class BattleUnit;

class UnitBehavior
{
public:
    virtual ~UnitBehavior() = default;
    virtual void OnBattleStart(BattleUnit* unit) {};

    virtual void OnUpdate(BattleUnit* unit, float deltaTime) = 0;
    virtual void OnDeath(BattleUnit* unit) = 0;
    virtual void OnDamageTaken(BattleUnit* unit, float damage, BattleUnit* source) = 0;

    virtual bool CanAttack(BattleUnit* attacker, BattleUnit* defender) { return false; }
    virtual void OnAttack(BattleUnit* attacker, BattleUnit* defender) {}

    virtual float ModifyDamage(BattleUnit* attacker, BattleUnit* target, float baseDamage)
    {
        return baseDamage;
    }

    virtual std::string GetBehaviorType() const = 0;
};

#endif