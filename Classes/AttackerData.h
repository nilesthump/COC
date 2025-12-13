//12/4 今天我先实现野蛮人（近战），弓箭手（远程)，巨人，哥布林，炸弹人
// 扪心自问：
// 1.有地面、空中、近战、远程几种基础类别
// 2.每个单位有生命值，攻击伤害，伤害范围（单体、连锁、范围）
// 3.考虑攻击偏好
// 4.哪些属性是战斗相关哪些是资源相关
// Classes/Data/AttackerData.h
#ifndef _ATTACKER_DATA_H
#define _ATTACKER_DATA_H
#include<string>
#include<memory>
#include"BaseUnitData.h"
#include"BomberBehaviorData.h"

//? NOTE ：一些和UI相关，视觉音效等还没实现
//? NOTE ：角色不同等级对应的属性都不同(先用基础方式）
//? NOTE ：实验室等级和大本营等级的约束是不是可以不在这里体现，而是在主scene体现

//目标优先级枚举
enum class TargetPriority
{
	ANY,           // 任何目标
	DEFENSES,      // 防御建筑优先
	RESOURCES,     // 资源建筑优先
	WALLS,         // 城墙优先
	TROOPS         // 部队优先（以后再实现）
};

struct AttackerData : public BaseUnitData
{
	// std::string id;         // 已在基类
	// std::string name;       // 已在基类
	// int level;              // 已在基类
	// int health;             // 已在基类
	// int damage;             // 已在基类
	// double attack_interval; // 已在基类
	// double attack_distance; // 已在基类
	// AttackType attack_type; // 已在基类
	// UnitType unit_type;     // 已在基类
	// double move_speed;      // 已在基类
	
	TargetPriority preferred_target; // 优先目标
	int housing_space;             // 占用兵营人口数

	//Bomber特殊数据
	std::unique_ptr<BomberBehaviorData> bomber_data;

	//构造函数
	AttackerData() : BaseUnitData()
	{
		housing_space = 1;
	}

	//拷贝构造
	AttackerData(const AttackerData& other):BaseUnitData(other),preferred_target(other.preferred_target)
		,housing_space(other.housing_space)
	{
		if (other.bomber_data)
		{
			bomber_data = std::make_unique<BomberBehaviorData>(*other.bomber_data);
		}
	}

	//赋值构造
	AttackerData& operator=(const AttackerData& data)
	{
		if (this != &data)
		{
			BaseUnitData::operator=(data);
			preferred_target = data.preferred_target;
			housing_space = data.housing_space;

			if (data.bomber_data)
			{
				bomber_data = std::make_unique<BomberBehaviorData>(*data.bomber_data);
			}
			else
			{
				bomber_data.reset();
			}
		}
		return *this;
	}

	virtual bool IsAttacker() const override { return true; }

	static AttackerData CreateBarbarianData(int level = 1);	//野蛮人
	static AttackerData CreateArcherData(int level = 1);	//弓箭手
	static AttackerData CreateGiantData(int level = 1);		//巨人
	static AttackerData CreateGoblinData(int level = 1);	//哥布林
	static AttackerData CreateBomberData(int level = 1);	//炸弹人

};

#endif// AttackerData_H