#include "DefenderNormalBehavior.h"


std::string DefenderNormalBehavior:: GetBehaviorType() const 
{
    return "DefenderNormal";
}

bool DefenderNormalBehavior::CanAttack(BattleUnit* attacker, BattleUnit* defender) 
{
    if (!BaseBehavior::CanAttack(attacker, defender))
        return false;

    //防御建筑的额外检查
    //例如：检查是否能攻击该类型目标
    return true;
}