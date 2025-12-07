#include "CharacerData.h"
#include "BomberBehaviorData.h"
#include<vector>
#include<tuple>

//! 其他细节请看社区
//! 还没有规定好格子和距离的关系
//野蛮人Barbarian
CharacterData CharacterData::CreateBarbarianData(int level = 1)
{
	CharacterData data;

	data.id = "barbarian";
	data.name = "野蛮人";
	data.combat_type = CombatType::MELEE;
	data.unit_type = UnitType::GROUND;
	data.attack_type = AttackType::SINGLE_TARGET;
	data.preferred_target = TargetPriority::ANY;
	data.resource_type = ResourceType::ELIXIR;

	data.housing_space = 1;
	data.attack_interval = 1.0;     // 1s
	data.attack_distance = 0.4;		// 0.4格
	data.move_speed = 2.0;			// 2格/s
	static const std::vector<std::pair<int, int>> level_stats = {
		{9, 45},
		{12, 54},
		{15, 65},
		{18, 85},
		{23, 105},
		{26, 125},
		{30, 160},
		{34, 205},
		{38, 230},
		{42, 250},
		{45, 270},
		{48, 290}   // 等级12
	};
	int idx = level - 1;
	if (idx >= 0 && idx < level_stats.size())
	{
		data.damage = level_stats[idx].first;
		data.health = level_stats[idx].second;
	}

	return data;
}

//弓箭手Archer
CharacterData CharacterData::CreateArcherData(int level = 1)
{
	CharacterData data;

	data.id = "archer";
	data.name = "弓箭手";
	data.combat_type = CombatType::RANGED;
	data.unit_type = UnitType::BOTH;
	data.attack_type = AttackType::SINGLE_TARGET;
	data.preferred_target = TargetPriority::ANY;
	data.resource_type = ResourceType::ELIXIR;

	data.housing_space = 1;
	data.attack_interval = 1.0;     // 1s
	data.attack_distance = 3.5;		// 3.5格
	data.move_speed = 3.0;			// 3格/s
	static const std::vector<std::pair<int, int>> klevel_stats = {
		{8, 22},
		{10, 26},
		{13, 29},
		{16, 33},
		{20, 40},
		{22, 44},
		{25, 48},
		{28, 52},
		{31, 56},
		{34, 60},
		{37, 64},
		{40, 68},
		{43, 72}    // 等级13
	};
	int idx = level - 1;
	if (idx >= 0 && idx < klevel_stats.size())
	{
		data.damage = klevel_stats[idx].first;
		data.health = klevel_stats[idx].second;
	}

	return data;
}

//巨人Giant
CharacterData CharacterData::CreateGiantData(int level = 1)
{
	CharacterData data;

	data.id = "giant";
	data.name = "巨人";
	data.combat_type = CombatType::MELEE;
	data.unit_type = UnitType::GROUND;
	data.attack_type = AttackType::SINGLE_TARGET;
	data.preferred_target = TargetPriority::DEFENSES;
	data.resource_type = ResourceType::ELIXIR;

	data.housing_space = 5;
	data.attack_interval = 2.0;     // 2s
	data.attack_distance = 1.0;		// 1格
	data.move_speed = 1.5;			// 1.5格/s
	static const std::vector<std::pair<int, int>> klevel_stats = {
		{24, 400},
		{30, 500},
		{40, 600},
		{48, 700},
		{62, 900},
		{86, 1100},
		{110, 1300},
		{124, 1500},
		{140, 1850},
		{156, 2000},
		{172, 2200},
		{188, 2400},
		{204, 2600}		// 等级13
	};
	int idx = level - 1;
	if (idx >= 0 && idx < klevel_stats.size())
	{
		data.damage = klevel_stats[idx].first;
		data.health = klevel_stats[idx].second;
	}

	return data;
}

//哥布林Goblin
CharacterData CharacterData::CreateGoblinData(int level = 1)
{
	CharacterData data;

	data.id = "goblin";
	data.name = "哥布林";
	data.combat_type = CombatType::MELEE;
	data.unit_type = UnitType::GROUND;
	data.attack_type = AttackType::SINGLE_TARGET;
	data.preferred_target = TargetPriority::RESOURCES;
	data.resource_type = ResourceType::ELIXIR;

	data.housing_space = 1;
	data.attack_interval = 1.0;     // 1s
	data.attack_distance = 0.4;		// 0.4格
	data.move_speed = 4.0;			// 4格/s
	static const std::vector<std::pair<int, int>> klevel_stats = {
		{11, 25},
		{14, 30},
		{19, 36},
		{24, 50},
		{32, 65},
		{42, 80},
		{52, 105},
		{62, 126},
		{72, 146},	// 等级9
	};
	int idx = level - 1;
	//! 哥布林对资源建筑类的每次伤害翻倍，这里需要一个接口判断攻击目标，然后翻倍
	//! 翻倍的攻击力可以写在攻击battle里
	//! ui展示的时候写在ui那里
	if (idx >= 0 && idx < klevel_stats.size())
	{
		data.damage = klevel_stats[idx].first;
		data.health = klevel_stats[idx].second;
	}

	return data;
}

//炸弹人Bomber
//! 炸弹人有特殊的寻路机制和爆炸伤害机制，参考社区
//! 架构还没有想清楚
CharacterData CharacterData::CreateBomberData(int level = 1)
{
	CharacterData data;

	data.id = "bomber";
	data.name = "炸弹人";
	data.combat_type = CombatType::MELEE;
	data.unit_type = UnitType::GROUND;
	data.attack_type = AttackType::SPLASH;
	data.preferred_target = TargetPriority::WALLS;
	data.resource_type = ResourceType::ELIXIR;

	data.housing_space = 2;
	data.attack_interval = 0.0;     //! 爆炸一次会杀死自己不需要间隔
	data.attack_distance = 0.5;		// 0.5格
	data.move_speed = 3.0;			// 3格/s
	//! 到达目标停顿1s
	//! 主动伤害爆炸半径 0.8格
	//! 死亡伤害爆炸半径 1.5格
	//! 下面表格是错的，没有区分主动和被动伤害以及这些逻辑应该写到哪里，明天见
	static const std::vector<std::tuple<int, int,int>> klevel_stats = {
		{10, 6, 20},
		{20, 9, 24},
		{25, 13, 29},
		{30, 16, 35},
		{43, 23, 53},
		{55, 30, 72},
		{66, 36, 82},
		{75, 42, 92},
		{86, 48, 112},
		{94, 54, 130},
		{102, 60, 140},
		{110, 66, 150},
		{118, 72, 160}		// 等级13(主动伤害、死亡伤害、生命值）
	};
	data.bomber_data = std::make_unique<BomberBehaviorData>();
	data.bomber_data->fuse_time = 1.0;
	data.bomber_data->active_radius = 0.8;
	data.bomber_data->death_radius = 1.5;
	data.bomber_data->wall_damage_multiplier = 40;
	
	int idx = level - 1;

	if (idx >= 0 && idx < klevel_stats.size())
	{
		data.bomber_data->active_damage = data.damage = std::get<0>(klevel_stats[idx]);
		data.bomber_data->death_damage = std::get<1>(klevel_stats[idx]);
		data.health = std::get<2>(klevel_stats[idx]);
	}
	return data;
}