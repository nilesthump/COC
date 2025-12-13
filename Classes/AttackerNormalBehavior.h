//normal作为通用行为
#ifndef _ATTACKER_NORMAL_BEHAVIOR_H_
#define _ATTACKER_NORMAL_BEHAVIOR_H_
#include "BaseBehavior.h"

class AttackerNormalBehavior :public BaseBehavior
{
public:
    //可以覆盖基类方法或添加新方法
    std::string GetBehaviorType() const override;
 
    bool CanAttack(BattleUnit* attacker, BattleUnit* defender) override;
    
};
#endif

