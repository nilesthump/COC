#include "BattleManager.h"
#include <vector>
#include <memory>

//添加战斗单位
void BattleManager::AddUnit(std::unique_ptr<BattleUnit> unit, bool is_attacker)
{
	if (is_attacker)
	{
		attackers_.push_back(unit.get());
	}
	else
	{
		defenders_.push_back(unit.get());
	}
	all_units_.push_back(std::move(unit));
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

    //更新所有单位
    for (auto& unit : all_units_)
    {
        if (unit->IsAlive())
        {
            std::vector<BattleUnit*> enemies = GetEnemiesFor(unit.get());
            unit->Update(deltaTime, enemies);
        }
    }

    //清理死亡单位（可选）
    //RemoveDeadUnits();

    // 检查战斗是否结束
    if (IsBattleOver())
    {
        EndBattle();
    }
}

//检查战斗是否结束
//! 这里还有个时间限制检查没加进去
bool BattleManager::IsBattleOver() const
{
    //检查是否还有存活的进攻方
    bool attackers_alive = false;
    for (auto attacker : attackers_)
    {
        if (attacker->IsAlive())
        {
            attackers_alive = true;
            break;
        }
    }

    //检查是否还有存活的防守方
    bool defenders_alive = false;
    for (auto defender : defenders_)
    {
        if (defender->IsAlive())
        {
            defenders_alive = true;
            break;
        }
    }

    return !attackers_alive || !defenders_alive;
}