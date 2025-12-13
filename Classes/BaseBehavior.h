#ifndef _BASEBEHAVIOR_H_
#define _BASEBEHAVIOR_H_

#include "UnitBehavior.h"

class BaseBehavior : public UnitBehavior
{
public:
    //基础伤害计算
    double CalculateDamage(BattleUnit* attacker, BattleUnit* defender) override;

    //基础攻击检查
    bool CanAttack(BattleUnit* attacker, BattleUnit* defender) override;

    //攻击效果触发
    void OnAttack(BattleUnit* attacker, BattleUnit* defender) override;

    //每帧更新
    void OnUpdate(BattleUnit* unit, double deltaTime) override;

    //死亡处理
    void OnDeath(BattleUnit* unit) override;

    //受到伤害反应
    void OnDamageTaken(BattleUnit* unit, double damage, BattleUnit* source) override;

    // 获取行为类型
    std::string GetBehaviorType() const override;
};

#endif // BASEBEHAVIOR_H