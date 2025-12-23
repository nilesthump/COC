/*
 * UnitState.h - 单位状态数据类
 * 核心职责：战斗单位的实时状态数据容器（纯数据类）
 * 设计模式：数据容器模式 + 值对象模式
 *
 * 设计原则：
 * 1. 纯数据类：只包含数据，不包含行为
 * 2. 值语义：可以被复制，没有多态性
 *
 * 数据流：
 * 静态配置 → 动态状态 → 实时显示
 *   (Data)    (State)     (UI)
 * 
 * 关注：
 * 1.UnitState提供了部分接口来获取静态数据，原因是在实际战斗中，Unit个体的寻路、
 *	 战斗表现需要关注静态数据的移动速度等，故提供部分接口。但是该接口传给BattleUnit进行二层传递
 *   最终unit寻路攻击表现可以直接在BattleUnit中调用得到静态数据
 */
#ifndef UNITSTATE_H
#define UNITSTATE_H
#include "BaseUnitData.h"
#include <memory>

class UnitState
{
private:
	std::unique_ptr<BaseUnitData> base_data_ptr_;

	//实时状态
	float current_health_;
	float position_X_, position_Y_;
	float attack_cool_down_;
	bool is_alive_;

public:
	//构造函数
	UnitState();
	~UnitState() = default;

	//禁止拷贝
	UnitState(const UnitState&) = delete;
	UnitState& operator=(const UnitState&) = delete;

	//允许移动
	UnitState(UnitState&&) = default;
	UnitState& operator=(UnitState&&) = default;

	//初始化接口
	//模板化的Init函数，可以接受任何派生自BaseUnitData的类型
	template<typename T>
	void Init(const T& data)
	{
		base_data_ptr_ = std::make_unique<T>(data);
		current_health_ = base_data_ptr_->health;
		is_alive_ = true;
		attack_cool_down_ = 0;
		position_X_ = 0;
		position_Y_ = 0;
	}
	
	//类型判断
	bool IsAttacker() const;
	bool IsDefender() const;

	//位置
	float GetPositionX() const;
	float GetPositionY() const;
	void SetPosition(float x, float y);

	//血量
	float GetCurrentHealth() const;
	float GetHealthPercent() const;
	void TakeDamage(float damage);
	bool IsAlive() const;

	//冷却管理部分
	void UpdateCoolDowns(float dt);
	bool CanAttack() const;
	void ResetAttackCooldown();
	float GetAttackCooldown() const;

	//部分静态数据访问
	float GetMoveSpeed() const;
	float GetAttackDistance() const;
	float GetDamage() const;
	float GetAttackInterval() const;
	float GetMaxHealth() const;
	AttackTargetType GetAttackTargetType() const;     //攻击目标类型（地面/空中/两者）
	UnitTargetType GetUnitTargetType()const;		//获取自身类型（空中/地面）
	AttackType GetAttackType() const;   //攻击类型（单体/范围/连锁）
	CombatType GetCombatType() const;   //近战/远程
};

#endif