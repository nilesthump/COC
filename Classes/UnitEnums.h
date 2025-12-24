//将所有的枚举放在一起，身份标识用于战斗中选择面板或者面对具体类型的部分
//其他是原本data的部分，同意抽出
#pragma once

// 身份标识
enum class UnitType { NONE, BARBARIAN, ARCHER, GIANT, GOBLIN, BOMBER };
enum class BuildingType { NONE, TOWN_HALL, CANNON, ARCHER_TOWER, GOLD_MINE, ELIXIR_STORAGE, WALL };

// 战斗特性
enum class CombatType { MELEE, RANGED };		//近战远程枚举
enum class UnitTargetType { GROUND, AIR };      // 自身属性：地/空
enum class AttackTargetType { GROUND, AIR, BOTH }; // 攻击能力：打地/打空/全能
enum class TargetPriority { ANY, DEFENSES, RESOURCES, WALLS };//目标优先级枚举
enum class AttackType { SINGLE_TARGET, AREA_DAMAGE, SPLASH, CHAIN };//攻击类型枚举（单体，范围，连锁）
enum class ResourceType { NONE, GOLD, ELIXIR, DARK_ELIXIR };//资源使用枚举

// 战斗结果
enum class BattleResult { NONE, ATTACKERS_WIN, DEFENDERS_WIN, TIME_UP };

//每一个索引格子，格子！的状态
enum class GridStatus
{
    FREE = 0,      // 可通行，可放置
    BLOCKED = 1,   // 建筑占用，不可通行
    WALL = 2,      // 城墙，特殊阻挡
    FORBIDDEN = 3  // 边缘草坪，可通行，不可放置建筑
};