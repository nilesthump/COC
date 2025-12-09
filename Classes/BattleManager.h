//BattleManager的作用是战斗管理，组织各种battleunit
#ifndef BATTLEMANAGER_H
#define BATTLEMANAGER_H
#include "BattleUnit.h"
#include <vector>
#include <memory>

class BattleAgent
{
private:
	std::vector<std::unique_ptr<BattleUnit>> all_units_;

public:
	void Update(double delta_time)
	{
		// 1. 更新所有单位
		for (auto& unit : all_units_)
		{
			if (unit->IsAlive())//! 还有一个问题就是battleunit里面的一些函数其实是unitstate里面带有的，所以最好的方式还是传递给battleunit还是继承
			{
				// 获取该单位的敌人列表
				auto enemies = GetEnemiesFor(unit.get());
				unit->Update(delta_time, enemies);
			}
		}
		// 2. 清理死亡单位
		RemoveDeadUnits();

		// 3. 检查战斗结束
		if (IsBattleOver())
		{
			EndBattle();
		}
	}

	// 创建单位
	void CreateUnit(const CharacterData& data, bool isAttacker)
	{
		auto unit = std::make_unique<BattleUnit>();
		unit->Init(data);

		// 根据data.id设置对应的rule和way
		if (data.id == "barbarian")
		{
			unit->SetRule(new BarbarianRule());
			unit->SetWay(new BarbarianWay());
		}
		// ... 其他角色

		allUnits.push_back(std::move(unit));
	}

};
#endif 

