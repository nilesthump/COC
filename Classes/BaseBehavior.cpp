#include "BaseBehavior.h"
#include "BattleUnit.h"

double BaseBehavior::CalculateDamage(BattleUnit* attacker, BattleUnit* defender)
{
    return attacker->GetDamage(); 
}

bool BaseBehavior::CanAttack(BattleUnit* attacker, BattleUnit* defender)
{
    return attacker->IsAlive() && defender->IsAlive();
}

void BaseBehavior::OnAttack(BattleUnit* attacker, BattleUnit* defender)
{
    //默认无特殊效果
}

void BaseBehavior::OnUpdate(BattleUnit* unit, double deltaTime)
{
    //默认无更新逻辑
}

void BaseBehavior::OnDeath(BattleUnit* unit)
{
    //默认无死亡效果
}

void BaseBehavior::OnDamageTaken(BattleUnit* unit, double damage, BattleUnit* source)
{
    //默认无特殊处理
}

std::string BaseBehavior::GetBehaviorType() const
{
    return "Base";
}