//12.10 我先把normal作为通用行为而不是基类
#ifndef NORMALBEHAVIOR_H
#define NORMALBEHAVIOR_H
#include "UnitBehavior.h"
#include "BattleUnit.h"

class NormalBehavior :public UnitBehavior
{
public:
	//基础伤害
	double CalculateDamage(BattleUnit* attacker, BattleUnit* defender)override
	{
		return attacker->GetBaseData().damage;
	}

	//基础攻击检查
	bool CanAttack(BattleUnit* attacker, BattleUnit* defender) override
	{
		return attacker->IsAlive() && defender->IsAlive();
	}

	//! 攻击后效果（默认无），不清晰
	void OnAttack(BattleUnit* attacker, BattleUnit* defender) override {}

    //? 每帧更新，为什么没实现
    void OnUpdate(BattleUnit* unit, double deltaTime) override
    {
        // 通用更新逻辑，如检查状态等
    }

    // 死亡效果
    void OnDeath(BattleUnit* unit) override
    {
        // 默认无死亡效果
    }

    //? 受到伤害，也不实现
    void OnDamageTaken(BattleUnit* unit, double damage, BattleUnit* source) override {}

    std::string GetBehaviorType() const override
    {
        return "Normal";
    }
};
#endif

