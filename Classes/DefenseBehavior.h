#ifndef _DEFENSE_BEHAVIOR_H
#define _DEFENSE_BEHAVIOR_H

#include "NormalBehavior.h"
#include "BattleUnit.h"
#include <string>

class DefenseBehavior : public NormalBehavior
{
public:
    // 防御建筑的行为
    bool CanAttack(BattleUnit* attacker, BattleUnit* defender) override
    {
        // 1. 基础检查
        if (!NormalBehavior::CanAttack(attacker, defender))
        {
            return false;
        }

        // 2. 今天简化：所有防御建筑都能攻击所有类型
        // 明天再添加详细的类型检查
        return true;
    }

    std::string GetBehaviorType() const override
    {
        return "Defense";
    }
};
#endif 