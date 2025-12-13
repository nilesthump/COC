/*
 * BattleUnit.h - 战斗单位实体类
 *
 * 核心职责：战斗系统的实体容器，协调各个组件的工作
 * 设计模式：组合模式 + 策略模式
 *
 * 1. 这是战斗系统的最小可操作单元
 * 2. 每个BattleUnit代表战场上的一个独立实体（如一个野蛮人、一个箭塔）
 * 3. 采用组合模式，不继承具体功能，而是持有功能组件
 *
 * 组合关系：
 * BattleUnit = UnitState + UnitBehavior + UnitNavigation
 *      ↓            ↓             ↓             ↓
 *     实体       状态数据       行为逻辑       移动寻路
 *
 * 生命周期：
 * 1. 创建：UnitFactory根据配置创建
 * 2. 初始化：设置初始位置、血量等状态
 * 3. 战斗：每帧Update，协调各组件工作
 * 4. 销毁：死亡或战斗结束后删除
 */

#ifndef BATTLEUNIT_H
#define BATTLEUNIT_H
#include "UnitState.h"
#include "UnitBehavior.h"
#include "UnitNavigation.h"
#include <vector>

class BattleUnit
{
private:
	UnitState state_;
	UnitBehavior* behavior_;
	UnitNavigation* navigation_;
	BattleUnit* target_;

public:
	BattleUnit();
	~BattleUnit() = default;

	//初始化
	template<typename T>
	void Init(const T&data)
	{
		state_.Init(data);
	}

	//设置组件
	void SetBehavior(UnitBehavior* behavior);
	void SetNavigation(UnitNavigation* navigation);

	//更新函数
	void Update(double deltaTime, std::vector<BattleUnit*>& enemies);

	//受到伤害
	void TakeDamage(double damage, BattleUnit* source);

	//Getter接口
	UnitState& GetState();
	const UnitState& GetState() const;

	bool IsAlive() const;
	double GetPositionX() const;
	double GetPositionY() const;
	void SetPosition(double x, double y);

	double GetHealthPercent() const;
	double GetCurrentHealth() const;
	double GetMaxHealth()const;
	double GetMoveSpeed() const;
	double GetAttackDistance() const;
	double GetDamage() const;
	double GetAttackInterval() const;

	UnitType GetTargetType() const;
	AttackType GetAttackType() const;
	CombatType GetCombatType() const;
	BattleUnit* GetTarget() const;

	//新增：是否为资源建筑（给哥布林用）
	bool IsResourceBuilding() const;
};

#endif