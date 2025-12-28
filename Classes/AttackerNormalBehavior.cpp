#include"AttackerNormalBehavior.h"
#include"BattleUnit.h"

bool AttackerNormalBehavior::CanAttack(BattleUnit* attacker, BattleUnit* defender)
{
    // 如果目标是地面的，而我是打空的，就不能打，这里已经在找目标那里判断过了
    // 目前简单判断是否在 CD 即可
    return attacker->GetState().CanAttack();
}

void AttackerNormalBehavior::OnAttack(BattleUnit* attacker, BattleUnit* defender)
{
    if (!defender || !defender->IsAlive()) return;

    //伤害计算（这里可以加入兵种克制系数）
    float damage = attacker->GetDamage();

    if (attacker->GetState().GetUnitType() == UnitType::GOBLIN)
    {
        if (defender->GetState().IsResourceBuilding() || defender->GetState().IsTownHall())
        {
            damage *= 2.0f;
        }
    }
    //触发受击者扣血
    defender->TakeDamage(damage, attacker);
}

void AttackerNormalBehavior::OnUpdate(BattleUnit* unit, float dt)
{
   
}

void AttackerNormalBehavior::OnDeath(BattleUnit* unit)
{
    // 进攻者死亡逻辑：播放死亡特效
   
}

void AttackerNormalBehavior::OnDamageTaken(BattleUnit* unit, float damage, BattleUnit* attacker)
{
    
}