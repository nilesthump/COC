#include "BattleManager.h"
#include <vector>
#include <memory>

//添加战斗单位
void BattleManager::AddUnit(std::unique_ptr<BattleUnit> unit, bool is_attacker)
{
	BattleUnit* raw = unit.get();
	bool battle_just_started = false;

	if (is_attacker)
	{
		attackers_.push_back(raw);
		heroes_placed_++;
		if (!battle_active_)
		{
			battle_active_ = true;
			battle_time_elapsed_ = 0;
			battle_just_started = true;
		}
	}
	else
		defenders_.push_back(raw);

	all_units_.push_back(std::move(unit));

	if (battle_just_started)
	{
		//战斗刚开始，拉所有单位参战
		for (auto& u : all_units_)
		{
			u->OnBattleStart();
		}
	}
	else if (battle_active_)
	{
		raw->OnBattleStart();
	}
}

//获取敌人的列表
std::vector<BattleUnit*> BattleManager::GetEnemiesFor(BattleUnit* unit) const
{
	//简单判断：如果在进攻方列表中，敌人就是防守方
	for (auto attacker : attackers_)
	{
		if (attacker == unit)
		{
			return defenders_;
		}
	}
	return attackers_; //否则是防守方，敌人是进攻方
}

//主更新循环
void BattleManager::Update(double deltaTime)
{
	if (!battle_active_)
		return;

	battle_time_elapsed_ += deltaTime;
	//更新所有单位
	for (auto& unit : all_units_)
	{
		if (unit->IsAlive())
		{
			std::vector<BattleUnit*> enemies = GetEnemiesFor(unit.get());
			unit->Update(deltaTime, enemies);
		}
	}

	RemoveDeadUnits();
	battle_result_ = EvaluateBattleResult();
	//检查战斗是否结束
	if (battle_result_ != BattleResult::NONE)
	{
		battle_active_ = false;
	}

}

//! 返回战斗结果，目前对战斗结果的判断还不明确，需要别的判据
BattleResult BattleManager::EvaluateBattleResult()
{
	//还没开始战斗，不可能结束
	if (!battle_active_)
		return BattleResult::NONE;

	//时间到
	if (battle_time_elapsed_ >= MAX_BATTLE_TIME_)
		return BattleResult::TIME_UP;

	bool attackers_alive = false;
	for (auto a : attackers_)
	{
		if (a->IsAlive())
		{
			attackers_alive = true;
			break;
		}
	}

	bool defenders_alive = false;
	for (auto d : defenders_)
	{
		if (d->IsAlive())
		{
			defenders_alive = true;
			break;
		}
	}

	//正确的胜负判断
	if (!attackers_alive && heroes_placed_ == total_heroes_)
	{
		return BattleResult::DEFENDERS_WIN;
	}

	//! 还没有加上对资源型建筑的问题
	if (!defenders_alive)
	{//或者是全部打完才算胜利
		return BattleResult::ATTACKERS_WIN;
	}

	return BattleResult::NONE;
}

void BattleManager::SetBattleActive(bool s)
{
	battle_active_ = s;
}

//移除死亡单位
void BattleManager::RemoveDeadUnits()
{
	// 遍历所有单位，找出死亡的单位
	for (auto it = all_units_.begin(); it != all_units_.end(); )
	{
		BattleUnit* unit = it->get();
		if (!unit->IsAlive())
		{
			// 从attackers_或defenders_中移除对应的指针
			auto remove_from_vector = [unit](std::vector<BattleUnit*>& vec)
				{
					vec.erase(std::remove(vec.begin(), vec.end(), unit), vec.end());
				};

			remove_from_vector(attackers_);
			remove_from_vector(defenders_);

			// 移除视觉元素
			unit->RemoveSprite();

			// 从all_units_中移除
			it = all_units_.erase(it);
		}
		else
		{
			++it;
		}
	}
}