//AttackerData和DefenderData都继承于它
#ifndef _BASE_UNIT_DATA_
#define _BASE_UNIT_DATA_
#include "UnitEnums.h"
#include<string>

struct BaseUnitData
{
    //标识信息
    std::string id;             //id
    std::string name;           //显示名称
    int level;                  //等级
   
    //战斗属性
    float health;                 //生命值
    float damage;                 //攻击力
    float attack_interval;     //攻击间隔（秒）
    float attack_distance;     //攻击距离（格）
    float search_range;         //搜索范围，攻击者的自定义，防御的就是他的射程
    float damage_radius;		//对于范围伤害有伤害半径，其实也有主动伤害和被动伤害

    //攻击特性
    CombatType combat_type = CombatType::MELEE;
    AttackTargetType attack_target_type = AttackTargetType::GROUND;
    UnitTargetType unit_target_type = UnitTargetType::GROUND;
    AttackType attack_type = AttackType::SINGLE_TARGET;

    //移动属性（防御建筑为0）
    float move_speed;          //移动速度（格/秒）

    // 构造函数
    BaseUnitData()
        : level(1),
        health(0),
        damage(0),
        attack_interval(1.0),
        attack_distance(0.0),
        move_speed(0.0),
        search_range(0.0),
        damage_radius(0.0)
    {}

    virtual ~BaseUnitData() = default;

    BaseUnitData(const BaseUnitData& other) = default;  
    BaseUnitData& operator=(const BaseUnitData& other) = default; 

    //获取展示名称
    std::string GetDisplayName() const
    {
        return name + " Lv." + std::to_string(level);
    }

    virtual int GetHousingSpace() const { return 0; }
};
#endif //_BASE_UNIT_DATA_

