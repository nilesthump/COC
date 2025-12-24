//在这里我们明确区分了“防御设施”和“资源/普通设施”，提供通用数据。为战斗做准备
#ifndef _DEFENSE_DATA_H_
#define _DEFENSE_DATA_H_
#include "BaseUnitData.h"

struct DefenderData : public BaseUnitData
{
    BuildingType building_type = BuildingType::NONE;
    ResourceType resource_type;
    //目前这里使用的是占地面积（比实际判定面积大）
    int tile_width=1;     //横向格子数
    int tile_height=1;    //纵向格子数
    //资源产出属性
    bool is_resource_building = false; // 是否是资源建筑
    int gold_reward = 0;               // 摧毁掉落金币
    int elixir_reward = 0;             // 摧毁掉落圣水

    DefenderData() : BaseUnitData(){}
    DefenderData(const DefenderData& other)
        : BaseUnitData(other),
        tile_width(other.tile_width),
        tile_height(other.tile_height)
    {}
    DefenderData& operator=(const DefenderData& other)
    {
        if (this != &other)
        {
            BaseUnitData::operator=(other);
            tile_width = other.tile_width;     
            tile_height = other.tile_height;    
        }
        return *this;
    }

    virtual bool IsDefender() const override { return true; }
    int GetTileWidth() const override { return tile_width; }
    int GetTileHeight() const override { return tile_height; }
    bool IsResourceBuilding() const override { return is_resource_building; }

    static DefenderData CreateCannonData(int level = 1);         //加农炮
    static DefenderData CreateArcherTowerData(int level = 1);    //箭塔
    static DefenderData CreateGoldMineData(int level = 1); // 新增
    //static DefenderData CreateElixirData(int level = 1); // 新增
    //static DefenderData CreateTownHallData(int level = 1); // 新增（大本营也算资源建筑，因为给钱多）
};

#endif // _DEFENSE_DATA_H_