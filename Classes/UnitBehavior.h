/*
 * UnitBehavior.h - 单位行为接口定义
 *
 * 文件说明：
 * 1. 定义所有战斗单位行为的抽象基类
 * 2. 采用策略模式，不同单位实现不同行为
 *
 * 设计理念：
 * - 接口隔离：将战斗行为与其他逻辑分离
 * - 开闭原则：新增行为只需继承此接口，无需修改现有代码
 * - 依赖倒置：高层模块通过接口调用，不依赖具体实现
 *
 * 使用场景：
 * 1. BattleUnit持有UnitBehavior指针
 * 2. UnitFactory根据角色类型创建具体Behavior
 * 3. 战斗中通过多态调用具体行为逻辑
 */
#ifndef UNITBEHAVIOR_H
#define UNITBEHAVIOR_H
#include <string>

class BattleUnit;

class UnitBehavior
{
public:
    virtual ~UnitBehavior() = default;
    virtual void OnBattleStart(BattleUnit* unit) {}
    virtual void OnBattleEnd(BattleUnit* unit) {}
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
