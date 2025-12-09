/*
 * BattleUnit.h - 战斗单位实体类
 *
 * 核心职责：战斗系统的实体容器，协调各个组件的工作
 * 设计模式：组合模式 + 策略模式
 *
 * 架构位置：
 * Data层 → Unit层 → Battle层
 *      ↓         ↓         ↓
 *  静态数据  战斗实体  战斗管理
 *
 * 组合关系：
 * BattleUnit = UnitState + UnitBehavior + UnitNavigation
 *      ↓            ↓             ↓             ↓
 *     实体       状态数据       行为逻辑       移动寻路
 */
#ifndef BATTLEUNIT_H
#define BATTLEUNIT_H
#include "UnitState.h"
#include "UnitBehavior.h"
#include "UnitNavigation.h"
#include <vector>

 /**
  * @class BattleUnit
  * @brief 战斗单位实体类，战场上的一个独立战斗单位
  *
  * 类说明：
  * 1. 这是战斗系统的最小可操作单元
  * 2. 每个BattleUnit代表战场上的一个独立实体（如一个野蛮人、一个箭塔）
  * 3. 采用组合模式，不继承具体功能，而是持有功能组件
  *
  * 生命周期：
  * 1. 创建：UnitFactory根据配置创建
  * 2. 初始化：设置初始位置、血量等状态
  * 3. 战斗：每帧Update，协调各组件工作
  * 4. 销毁：死亡或战斗结束后删除
  */
class BattleUnit
{
private:
	UnitState state_;
	UnitBehavior* behavior_;
	UnitNavigation* navigation_;
	BattleUnit* target_;

public:
	BattleUnit() : behavior_(nullptr), navigation_(nullptr), target_(nullptr) {}

	~BattleUnit()
	{
		delete behavior_;
		delete navigation_;
	}

	// 初始化
	void Init(const CharacterData& data)
	{
		state_.Init(data);
	}

	// 设置组件
	void SetBehavior(UnitBehavior* behavior)
	{
		behavior_ = behavior;
	}

	void SetNavigation(UnitNavigation* navigation)
	{
		navigation_ = navigation;
	}

	// 更新函数
	void Update(float deltaTime, std::vector<BattleUnit*>& enemies)
	{
		if (!state_.IsAlive()) return;

		// 1. 行为更新
		if (behavior_)
		{
			behavior_->OnUpdate(this, deltaTime);
		}

		// 2. 寻找目标
		if (navigation_ && (!target_ || !target_->IsAlive()))
		{
			target_ = navigation_->FindTarget(this, enemies);
		}

		// 3. 移动
		if (navigation_ && target_ && !navigation_->IsInAttackRange(this, target_))
		{
			navigation_->CalculateMove(this, target_, deltaTime);
		}

		// 4. 攻击
		if (behavior_ && target_ && navigation_ &&
			navigation_->IsInAttackRange(this, target_) &&
			state_.CanAttack())
		{

			if (behavior_->CanAttack(this, target_))
			{
				float damage = behavior_->CalculateDamage(this, target_);
				target_->TakeDamage(damage, this);
				behavior_->OnAttack(this, target_);
				state_.ResetAttackCooldown();
			}
		}

		// 5. 更新状态
		state_.UpdateCoolDowns(deltaTime);
	}

	// 受到伤害
	void TakeDamage(float damage, BattleUnit* source)
	{
		state_.TakeDamage(damage);
		if (behavior_)
		{
			behavior_->OnDamageTaken(this, damage, source);
		}
		if (!state_.IsAlive() && behavior_)
		{
			behavior_->OnDeath(this);
		}
	}

	// ============ Getter接口 ============
	// 注意：这里返回的是UnitState&，不是GetState()
	UnitState& GetState() { return state_; }
	const UnitState& GetState() const { return state_; }

	// 为了方便，提供一些常用getter
	bool IsAlive() const { return state_.IsAlive(); }
	float GetPositionX() const { return state_.GetPositionX(); }
	float GetPositionY() const { return state_.GetPositionY(); }
	void SetPosition(float x, float y) { state_.SetPosition(x, y); }
	const CharacterData& GetBaseData() const { return state_.GetBaseData(); }
	float GetHealthPercent() const { return state_.GetHealthPercent(); }
	float GetHealth() const { return state_.GetHealth(); }

	BattleUnit* GetTarget() const { return target_; }

	// 新增：是否为资源建筑（给哥布林用）
	bool IsResourceBuilding() const
	{
		// 暂时返回false，以后可以根据CharacterData判断
		return false;
	}
};

#endif