//AttackerData和DefenderData都继承于它
#ifndef _BASE_UNIT_DATA_
#define _BASE_UNIT_DATA_
#include "UnitEnums.h"
#include<string>

struct BaseUnitData
{
    //标识信息
    std::string id;             //内部标识
    std::string name;           //显示名称
    int level=1;                  //等级

    //战斗属性
    float health;                 //生命值
    float damage;                 //攻击力
    float attack_interval;     //攻击间隔（秒）
    float attack_distance;     //攻击距离（格）

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
        move_speed(0.0)
    {}

    virtual ~BaseUnitData() = default;

    BaseUnitData(const BaseUnitData& other) = default;  
    BaseUnitData& operator=(const BaseUnitData& other) = default; 

    //获取展示名称
    std::string GetDisplayName() const
    {
        return name + " Lv." + std::to_string(level);
    }

    virtual bool IsAttacker() const { return false; }
    virtual bool IsDefender() const { return false; }
    // 进攻方属性默认值
    virtual TargetPriority GetPreferredTarget() const { return TargetPriority::ANY; }
    virtual int GetHousingSpace() const { return 0; }

    // 防守方属性默认值
    virtual int GetTileWidth() const { return 1; }
    virtual int GetTileHeight() const { return 1; }
    virtual bool IsResourceBuilding() const { return false; }
};
#endif //_BASE_UNIT_DATA_

