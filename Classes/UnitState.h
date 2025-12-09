/*
 * UnitState.h - 单位状态数据类
 *
 * 核心职责：战斗单位的实时状态数据容器（纯数据类）
 * 设计模式：数据容器模式 + 值对象模式
 *
 * 架构位置：
 * BattleUnit（实体） → 包含 → UnitState（状态数据）
 *                           ↓
 *                    CharacterData（静态配置）
 *
 * 设计原则：
 * 1. 纯数据类：只包含数据，不包含逻辑
 * 2. 值语义：可以被复制，没有多态性
 * 3. 与CharacterData区分：静态配置 vs 动态状态
 *
 * 数据流：
 * 静态配置 → 动态状态 → 实时显示
 *   (Data)    (State)     (UI)
 */
#ifndef UNITSTATE_H
#define UNITSTATE_H
#include "CharacterData.h"

class UnitState
{
private:
	CharacterData base_data_;

	//实时状态
	double current_health_;
	double position_X_, position_Y_;
	double attack_cool_down_;
	bool is_alive_;

public:
	void Init(const CharacterData& data)
	{
		base_data_ = data;
		current_health_ = data.health;
		is_alive_ = true;
		attack_cool_down_ = 0;
		position_X_ = 0;
		position_Y_ = 0;
	}

	// ============ 新增接口 ============

	// 获取基础数据
	const CharacterData& GetBaseData() const
	{
		return base_data_;
	}

	// 获取当前位置
	double GetPositionX() const { return position_X_; }
	double GetPositionY() const { return position_Y_; }

	// 设置位置
	void SetPosition(double x, double y)
	{
		position_X_ = x;
		position_Y_ = y;
	}

	// 获取当前血量（不只是百分比）
	double GetHealth() const { return current_health_; }

	// ============ 原有接口 ============

	//获取当前血量百分比
	double GetHealthPercent()const
	{
		if (base_data_.health <= 0) return 0;
		return current_health_ / base_data_.health;
	}

	//承受伤害
	void TakeDamage(double damage)
	{
		current_health_ -= damage;
		if (current_health_ <= 0)
		{
			current_health_ = 0;
			is_alive_ = false;
		}
	}

	//是否存活
	bool IsAlive()const
	{
		// 简化：只需要检查current_health_
		return current_health_ > 0;
	}

	//冷却管理部分
	void UpdateCoolDowns(double dt)
	{
		if (attack_cool_down_ > 0)
			attack_cool_down_ -= dt;
	}
	bool CanAttack() const
	{
		return attack_cool_down_ <= 0;
	}
	void ResetAttackCooldown()
	{
		attack_cool_down_ = base_data_.attack_interval;
	}

	// 新增：获取攻击距离（方便外部使用）
	double GetAttackDistance() const
	{
		return base_data_.attack_distance;
	}

	// 新增：获取移动速度
	double GetMoveSpeed() const
	{
		return base_data_.move_speed;
	}
};
#endif