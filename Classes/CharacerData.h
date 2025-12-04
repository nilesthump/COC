//12/4 今天我先实现野蛮人（近战），弓箭手（远程)，巨人，哥布林，炸弹人
// 扪心自问：
// 1.有地面、空中、近战、远程几种基础类别
// 2.每个单位有生命值，攻击伤害，伤害范围（单体、连锁、范围）
// 3.考虑攻击偏好
// 4.哪些属性是战斗相关哪些是资源相关
// Classes/Data/CharacterData.h
#ifndef CHARACTERDATA_H
#define CHARACTERDATA_H
#include<string>
//? NOTE ：一些和UI相关，视觉音效等还没实现
//? NOTE ：角色不同等级对应的属性都不同
//? NOTE ：实验室等级和大本营等级的约束是不是可以不在这里体现，而是在主scene体现

//近战远程枚举
enum class CombatType
{
	MELEE,      //近战
	RANGED      //远程
};

//单位类型枚举
enum class UnitType
{
	GROUND,		    // 只攻击地面
	AIR,			// 只攻击空中
	BOTH			// 都攻击
};

//攻击类型枚举（单体，范围，连锁）
enum class AttackType
{
	SINGLE_TARGET,  // 单体攻击
	SPLASH,         // 范围溅射
	CHAIN           // 连锁攻击
};

//目标优先级枚举
enum class TargetPriority
{
	ANY,           // 任何目标
	DEFENSES,      // 防御建筑优先
	RESOURCES,     // 资源建筑优先
	WALLS,         // 城墙优先
	TROOPS         // 部队优先（以后再实现）
};

//资源使用枚举
enum class ResourceType
{
	ELIXIR,         //圣水
	DARK_ELIXIR,    //暗黑重油
	GOLD            //金币
};

struct CharacterData
{
	//标识
	std::string id_;				//内部标识	
	std::string name_;				//显示名称
	CombatType combat_type_;		//近战远程
	UnitType unit_type_;			//单位类型
	ResourceType resource_type_;	//资源使用类型

	//战斗
	int health_;                 // 生命值
	int damage_;                 // 攻击力
	double attack_speed_;        // 攻击速度（攻击间隔秒数）
	double attack_range_;        // 攻击范围
	double move_speed_;          // 移动速度
	AttackType attack_type_;     // 攻击类型

	//目标选择
	TargetPriority preferred_target_;  // 优先目标

	//训练
	int housing_space_;			//占用兵营人口数

	// 构造函数
	CharacterData()
		: health_(0), damage_(0), attack_speed_(1.0),
		attack_range_(0.0), move_speed_(0.0),
		training_cost_(0), training_time_(0)
	{}

	static CharacterData CreateBarbarianData();	//野蛮人
	static CharacterData CreateArcherData();	//弓箭手
	static CharacterData CreateGiantData();		//巨人
	static CharacterData CreateGoblinData();	//哥布林
	static CharacterData CreateBomberData();	//炸弹人

};

#endif// CHARACTERDATA_H