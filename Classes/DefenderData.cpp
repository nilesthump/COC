//这里目前使用的是占地面积，大于判定面积，所以UI可以直接调用
//伤害均是每次伤害，不是每秒伤害
#include "DefenderData.h"
#include <vector>
DefenderData DefenderData::CreateCannonData(int level)
{
    DefenderData data;

    data.id = "cannon";
    data.name = "加农炮";
    data.level = level;
    data.attack_interval = 0.8;     //0.8秒一发
    data.attack_distance = 9.0;     //9格射程
    data.tile_width = 3;
    data.tile_height = 3;
    data.attack_type = AttackType::SINGLE_TARGET;
    data.unit_type = UnitType::GROUND; 
    data.combat_type = CombatType::RANGED;
    data.resource_type = ResourceType::GOLD;  
    static const std::vector<std::pair<double, int>> klevel_stats = {
        {5.6, 300},
        {8, 360},
        {10.4, 420},
        {13.6, 500},
        {18.4, 600},
        {24, 660},
        {32, 730},
        {38.4, 800},
        {44.8, 880},
        {51.2, 960},
        {59.2, 1060},
        {68, 1160}   // 等级12(共21）
    };
    int idx = level - 1;
    if (idx >= 0 && idx < klevel_stats.size())
    {
        data.damage = klevel_stats[idx].first;
        data.health = klevel_stats[idx].second;
    }

    return data;
}

DefenderData DefenderData::CreateArcherTowerData(int level)
{
    DefenderData data;

    data.id = "archer_tower";
    data.name = "箭塔";
    data.level = level;
    data.attack_interval = 0.5;     //0.5秒一发
    data.attack_distance = 10.0;
    data.tile_width = 3;
    data.tile_height = 3;
    data.attack_type = AttackType::SINGLE_TARGET;
    data.unit_type = UnitType::BOTH;
    data.combat_type = CombatType::RANGED;
    data.resource_type = ResourceType::GOLD;

    static const std::vector<std::pair<double, int>> klevel_stats = {
        {5.5, 380},
        {7.5, 420},
        {9.5, 460},
        {12.5, 500},
        {15, 540},
        {17.5, 580},
        {21, 630},
        {24, 690},
        {28, 750},
        {31.5, 810},
        {35, 890},
        {37, 970}   // 等级12(共21）
    };
    int idx = level - 1;
    if (idx >= 0 && idx < klevel_stats.size())
    {
        data.damage = klevel_stats[idx].first;
        data.health = klevel_stats[idx].second;
    }

    return data;
}