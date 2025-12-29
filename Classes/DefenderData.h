//在这里我们明确区分了“防御设施”和“资源/普通设施”，提供通用数据。为战斗做准备
#ifndef _DEFENSE_DATA_H_
#define _DEFENSE_DATA_H_
#include "BaseUnitData.h"

struct DefenderData : public BaseUnitData
{
	BuildingType building_type;
	ResourceType resource_type;
	//目前这里使用的是占地面积（比实际判定面积大）
	int tile_width;     //横向格子数
	int tile_height;    //纵向格子数
	//资源产出属性
	bool is_resource_building; // 是否是资源建筑
	bool is_defender_building; // 是否是除了墙以外的防御性建筑
	int gold_reward;               // 摧毁掉落金币
	int elixir_reward;             // 摧毁掉落圣水

	DefenderData() : BaseUnitData()
	{
		building_type = BuildingType::NONE;
		resource_type = ResourceType::NONE;
		tile_width = 1;
		tile_height = 1;
		is_resource_building = false;
		is_defender_building = false;
		gold_reward = 0;
		elixir_reward = 0;
	}

	//因为没有类似炸弹人的unique_ptr，其实可以默认，要不然新添加变量总忘！！12/26问题
	//改成了默认
	DefenderData(const DefenderData& other) = default;
	DefenderData& operator=(const DefenderData& other) = default;

	static DefenderData CreateCannonData(int level = 1);         //加农炮
	static DefenderData CreateArcherTowerData(int level = 1);    //箭塔
	static DefenderData CreateMortarData(int level = 1);         //迫击炮
	static DefenderData CreateWallData(int level = 1);          //城墙
	static DefenderData CreateGoldMineData(int level = 1);      //金矿
	static DefenderData CreateGoldStorageData(int level = 1);    //储金罐
	static DefenderData CreateElixirData(int level = 1);        //圣水收集器
	static DefenderData CreateElixirStorageData(int level = 1); //圣水存储器
	static DefenderData CreateTownHallData(int level = 1);      //大本营
	static DefenderData CreateBuildersHutData(int level = 1);      //大建筑工人小屋
	static DefenderData CreateArmyCampData(int level = 1);      //兵营
};

#endif // _DEFENSE_DATA_H_