//纯动态数据
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
	double attack_cool_down_;//? 使用方式是什么
	bool is_alive_;

public:
	void Init(const CharacterData& data)
	{
		base_data_ = data;
		current_health_ = data.health;
		is_alive_ = true;
		attack_cool_down_ = 0;
	}

	//获取当前血量百分比
	double GetHealthPercent()const
	{
		return current_health_ / base_data_.health;
	}

	//承受伤害
	void TakeDamage(double damage)
	{
		current_health_ -= damage;
	}

	//是否存活
	bool IsAlive()const
	{
		return is_alive_ && current_health_ > 0;//? 这里的双重约束其实不用吧 
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
};
#endif
