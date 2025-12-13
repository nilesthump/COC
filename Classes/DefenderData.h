#ifndef _DEFENSE_DATA_H_
#define _DEFENSE_DATA_H_

#include "BaseUnitData.h"

struct DefenderData : public BaseUnitData
{
    //目前这里使用的是占地面积（比实际判定面积大）
    int tile_width;     //横向格子数
    int tile_height;    //纵向格子数

    DefenderData() : BaseUnitData(){}
    DefenderData(const DefenderData& other): BaseUnitData(other){}
    DefenderData& operator=(const DefenderData& other)
    {
        if (this != &other)
        {
            BaseUnitData::operator=(other);
        }
        return *this;
    }

    virtual bool IsDefender() const override { return true; }

    static DefenderData CreateCannonData(int level = 1);         //加农炮
    static DefenderData CreateArcherTowerData(int level = 1);    //箭塔

};

#endif // _DEFENSE_DATA_H_