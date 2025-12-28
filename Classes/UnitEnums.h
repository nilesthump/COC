//将所有的枚举放在一起，身份标识用于战斗中选择面板或者面对具体类型的部分
//其他是原本data的部分，统一抽出
#pragma once

//身份标识
enum class UnitType { BARBARIAN, ARCHER, GIANT, GOBLIN, BOMBER, BALLOON, NONE};
enum class BuildingType { NONE, TOWN_HALL, CANNON, ARCHER_TOWER,MORTAR, GOLD_MINE, GOLD_STORAGE, ELIXIR, ELIXIR_STORAGE, WALL,BUILDERSHUT,ARMYCAMP };

//战斗特性
enum class CombatType { MELEE, RANGED };		        //近战远程枚举
enum class UnitTargetType { GROUND, AIR };              //自身属性：地/空
enum class AttackTargetType { GROUND, AIR, BOTH };      //攻击目标：打地/打空/全能
enum class TargetPriority { ANY, DEFENSES, RESOURCES, WALLS };//目标优先级枚举
enum class AttackType { SINGLE_TARGET, AREA_DAMAGE, CHAIN };//攻击类型枚举（单体，范围，连锁）
enum class ResourceType { NONE, GOLD, ELIXIR, DARK_ELIXIR };//资源使用枚举

enum class BattleResult
{
	NONE,
	ALL_DESTROYED,   // 100% 拆完 (对应3星)
	UNITS_EXHAUSTED, // 没兵了 (对应0-2星)
	TIME_UP          // 时间到 (对应0-2星)
};

enum class BattleStar
{
	ZERO = 0,
	ONE_STAR = 1,   //50%毁坏/摧毁大本营
	TWO_STARS = 2,  //50%毁坏且摧毁大本营
	THREE_STARS = 3 //100%毁坏
};

struct LiveBattleScore
{
	int total_buildings;      // 初始建筑总数（不含墙）
	int destroyed_buildings;  // 已摧毁总数
	bool town_hall_destroyed;  // 大本营是否炸了
	float gold_accumulated;		//用于高精度存储
	float elixir_accumulated;	//用于高精度存储
	int gold_collected;       // 本场拿到的金币
	int elixir_collected;     // 本场拿到的圣水

	LiveBattleScore() : total_buildings(0), destroyed_buildings(0),
		town_hall_destroyed(false), gold_accumulated(0),
		elixir_accumulated(0), gold_collected(0), elixir_collected(0)
	{}

	float getPercent() const
	{
		return total_buildings > 0 ? (float)destroyed_buildings / total_buildings : 0;
	}
};

//每一个索引格子，格子！的状态
enum class GridStatus
{
	FREE = 0,      // 可通行，可放置
	BLOCKED = 1,   // 建筑占用，不可通行
	WALL = 2,      // 城墙，特殊阻挡
	GRASS = 3  // 边缘草坪，可通行，不可放置建筑
};