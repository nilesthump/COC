#ifndef _DEFENDER_NORMAL_BEHAVIOR_H_
#define _DEFENDER_NORMAL_BEHAVIOR_H_

#include "BaseBehavior.h"

class DefenderNormalBehavior : public BaseBehavior
{
public:
    std::string GetBehaviorType() const override;

    //·ÀÓù½¨ÖþµÄ¹¥»÷¼ì²é
    bool CanAttack(BattleUnit* attacker, BattleUnit* defender) override;

};

#endif 