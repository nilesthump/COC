#ifndef _DEFENSE_DATA_H_
#define _DEFENSE_DATA_H_
#include "CharacterData.h"

// 防御建筑数据
struct DefenseData : public CharacterData
{
    bool can_target_air;
    bool can_target_ground;

    static DefenseData CreateArcherTowerData(int level = 1)
    {
        DefenseData data;
        data.id = "archer_tower";
        data.name = "箭塔";
        data.combat_type = CombatType::RANGED;
        data.unit_type = UnitType::BOTH; // 攻击地面和空中
        data.attack_type = AttackType::SINGLE_TARGET;
        data.preferred_target = TargetPriority::ANY;

        // 基础属性（简化版，实际需要完整数据）
        data.health = 500;
        data.damage = 25;
        data.attack_interval = 1.25f;
        data.attack_distance = 10.0f;
        data.move_speed = 0.0f; // 防御建筑不能移动

        data.can_target_air = true;
        data.can_target_ground = true;

        return data;
    }
};


#endif