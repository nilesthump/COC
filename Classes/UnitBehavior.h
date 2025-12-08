//这是一个所有角色的behavior（野蛮人弓箭手正常逻辑、炸弹人40倍以及爆炸逻辑等等）的通用接口
#ifndef UNITBEHAVIOR_H
#define UNITBEHAVIOR_H
#include <string>

class BattleUnit;

class UnitBehavior
{
public:
    virtual ~UnitBehavior() = default;

    //核心接口，需要注释
    //计算伤害
    virtual double CalculateDamage(BattleUnit* attacker, BattleUnit* defender) = 0;
    //是否可以攻击
    virtual bool CanAttack(BattleUnit* attacker, BattleUnit* defender) = 0;
    //! 攻击后效果，不清晰
    virtual void OnAttack(BattleUnit* attacker, BattleUnit* defender) = 0;
    //每帧更新
    virtual void OnUpdate(BattleUnit* unit, double deltaTime) = 0;
    //! 死亡效果，不清晰命名
    virtual void OnDeath(BattleUnit* unit) = 0;
    //受到伤害
    virtual void OnDamageTaken(BattleUnit* unit, double damage, BattleUnit* source) = 0;

    //获取行为类型（用于调试）
    virtual std::string GetBehaviorType() const = 0;
};

#endif
