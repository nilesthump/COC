#include"AttackerNormalBehavior.h"

std::string AttackerNormalBehavior::GetBehaviorType() const
{
	return "AttackerNormalBehavior";
}

bool AttackerNormalBehavior::CanAttack(BattleUnit* attacker, BattleUnit* defender)
{
    if (!BaseBehavior::CanAttack(attacker, defender))
        return false;

    //进攻单位的额外检查
    //比如检查攻击类型匹配等
    return true;
}