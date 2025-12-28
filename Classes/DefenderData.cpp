//这里目前使用的是占地面积，大于判定面积，所以UI可以直接调用
//伤害均是每次伤害，不是每秒伤害
#include "DefenderData.h"
#include <vector>

//加农炮
DefenderData DefenderData::CreateCannonData(int level)
{
    DefenderData data;

    data.id = "cannon";
    data.name = "加农炮";
    data.level = level;
    data.building_type = BuildingType::CANNON;
    data.attack_interval = 0.8;     //0.8秒一发
    data.attack_distance = 9.0;     //9格射程
    data.tile_width = 3;
    data.tile_height = 3;
    data.search_range = 9.0;        //防御设施的和射程相等
    data.attack_type = AttackType::SINGLE_TARGET;
    data.attack_target_type = AttackTargetType::GROUND; 
    data.unit_target_type = UnitTargetType::GROUND;
    data.combat_type = CombatType::RANGED;
    data.resource_type = ResourceType::GOLD;  
    data.is_defender_building = true;
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

//弓箭塔
DefenderData DefenderData::CreateArcherTowerData(int level)
{
    DefenderData data;

    data.id = "archer_tower";
    data.name = "箭塔";
    data.level = level;
    data.building_type = BuildingType::ARCHER_TOWER;
    data.attack_interval = 0.5;     //0.5秒一发
    data.attack_distance = 10.0;
    data.tile_width = 3;
    data.tile_height = 3;
    data.search_range = 10.0;        //防御设施的和射程相等
    data.attack_type = AttackType::SINGLE_TARGET;
    data.unit_target_type = UnitTargetType::GROUND;
    data.attack_target_type = AttackTargetType::BOTH;
    data.combat_type = CombatType::RANGED;
    data.resource_type = ResourceType::GOLD;
    data.is_defender_building = true;


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

//迫击炮范围攻击
DefenderData DefenderData::CreateMortarData(int level)
{
    DefenderData data;

    data.id = "mortar";
    data.name = "迫击炮";
    data.level = level;
    data.building_type = BuildingType::MORTAR;
    data.attack_interval = 5;     
    data.attack_distance = 11;      //! 注意一下，迫击炮射程是4-11格，这里用11代表判定，到时候特殊处理
    data.tile_width = 3;
    data.tile_height = 3;
    data.search_range = 11;        //防御设施的和射程相等
    data.damage_radius = 1.5;
    data.attack_type = AttackType::AREA_DAMAGE;
    data.unit_target_type = UnitTargetType::GROUND;
    data.attack_target_type = AttackTargetType::GROUND;
    data.combat_type = CombatType::RANGED;
    data.resource_type = ResourceType::GOLD;
    data.is_defender_building = true;


    static const std::vector<std::pair<double, int>> klevel_stats = {
        {20, 400},
        {25, 450},
        {30, 500},
        {35, 550},
        {45, 600},
        {55, 650},
        {75, 700},
        {100, 800},
        {125, 950},
        {150, 1100},
        {175, 1300},
        {190, 1500}   //12
    };
    int idx = level - 1;
    if (idx >= 0 && idx < klevel_stats.size())
    {
        data.damage = klevel_stats[idx].first;
        data.health = klevel_stats[idx].second;
    }

    return data;
}

//城墙
DefenderData DefenderData::CreateWallData(int level)
{
    DefenderData data;
    data.id = "wall";
    data.name = "城墙";
    data.level = level;
    data.building_type = BuildingType::WALL;
    data.tile_width = 1;
    data.tile_height = 1;
    data.damage = 0; // 不反击
    static const std::vector<int> klevel_stats = {
        100,
        200,
        400,
        800,
        1200,
        1800,
        2400,
        3000,
        3500,
        4000,
        5000,
        7000        //12
    };
    int idx = level - 1;
    if (idx >= 0 && idx < klevel_stats.size())
    {
        data.health = klevel_stats[idx];
    }
    return data;
}

//金矿
//注意，这里不写生产数据，defenderdata只用于战斗，大本营场景拖拽和生产在另一套逻辑编写，需要表明
DefenderData DefenderData::CreateGoldMineData(int level)
{
    DefenderData data;
    data.id = "gold_mine";
    data.name = "金矿";
    data.level = level;
    data.building_type = BuildingType::GOLD_MINE;
    data.is_resource_building = true; // 关键标记
    data.tile_width = 3;
    data.tile_height = 3;
    data.damage = 0; // 不反击
    //! 这里的数据要内置，COC本来要求掠夺比例式50%，内置总数据*50%存入即可
    data.gold_reward = 1000 * level; // 假设摧毁给这么多钱   
    static const std::vector<int> klevel_stats = {
        75,
        150,
        300,
        400,
        500,
        550,
        600,
        660,
        720,
        780,
        860,
        960
       //先写12个
    };
    int idx = level - 1;
    if (idx >= 0 && idx < klevel_stats.size())
    {
        data.health = klevel_stats[idx];
    }
    return data;
}

//储金罐数据生命值和金矿设定一样
DefenderData DefenderData::CreateGoldStorageData(int level)
{
    DefenderData data;
    data.id = "gold_storage";
    data.name = "储金罐";
    data.level = level;
    data.building_type = BuildingType::GOLD_STORAGE;
    data.is_resource_building = true; // 关键标记
    data.tile_width = 3;
    data.tile_height = 3;
    data.damage = 0; // 不反击
    //! 这里的数据要内置，COC本来要求掠夺比例式50%，内置总数据*50%存入即可
    data.gold_reward = 1000 * level; // 假设摧毁给这么多钱   
    static const std::vector<int> klevel_stats = {
        75,
        150,
        300,
        400,
        500,
        550,
        600,
        660,
        720,
        780,
        860,
        960,
        //先写12个
    };
    int idx = level - 1;
    if (idx >= 0 && idx < klevel_stats.size())
    {
        data.health = klevel_stats[idx];
    }
    return data;
}

//圣水收集器
//注意，这里不写生产数据，defenderdata只用于战斗，大本营场景拖拽和生产在另一套逻辑编写，需要表明
DefenderData DefenderData::CreateElixirData(int level)
{
    DefenderData data;
    data.id = "elixir";
    data.name = "圣水收集器";
    data.level = level;
    data.building_type = BuildingType::ELIXIR;
    data.is_resource_building = true; // 关键标记
    data.tile_width = 3;
    data.tile_height = 3;
    data.damage = 0; // 不反击
    //! 这里的数据要内置，COC本来要求掠夺比例式50%，内置总数据*50%存入即可
    data.elixir_reward = 1000 * level; // 假设摧毁给这么多钱   
    static const std::vector<int> klevel_stats = {
        75,
        150,
        300,
        400,
        500,
        550,
        600,
        660,
        720,
        780,
        860,
        960,
        //先写12个
    };
    int idx = level - 1;
    if (idx >= 0 && idx < klevel_stats.size())
    {
        data.health = klevel_stats[idx];
    }
    return data;
}

//圣水储存器生命值一样
DefenderData DefenderData::CreateElixirStorageData(int level)
{
    DefenderData data;
    data.id = "elixir_storage";
    data.name = "圣水储存器";
    data.level = level;
    data.building_type = BuildingType::ELIXIR_STORAGE;
    data.is_resource_building = true; // 关键标记
    data.tile_width = 3;
    data.tile_height = 3;
    data.damage = 0; // 不反击
    //! 这里的数据要内置，COC本来要求掠夺比例式50%，内置总数据*50%存入即可
    data.elixir_reward = 1000 * level; // 假设摧毁给这么多钱   
    static const std::vector<int> klevel_stats = {
        75,
        150,
        300,
        400,
        500,
        550,
        600,
        660,
        720,
        780,
        860,
        960,
        //先写12个
    };
    int idx = level - 1;
    if (idx >= 0 && idx < klevel_stats.size())
    {
        data.health = klevel_stats[idx];
    }
    return data;
}

//大本营
DefenderData DefenderData::CreateTownHallData(int level)
{
    DefenderData data;
    data.id = "town_hall";
    data.name = "大本营";
    data.level = level;
    data.building_type = BuildingType::TOWN_HALL;
    data.tile_width = 4;
    data.tile_height = 4;
    data.damage = 0; // 不反击
    //! 这里的数据要内置，COC本来要求掠夺比例式50%，内置总数据*50%存入即可
    data.elixir_reward = 1000 * level; // 假设摧毁给这么多钱   
    data.gold_reward = 1000 * level; // 假设摧毁给这么多钱   
    static const std::vector<int> klevel_stats = {
        400,
        800,
        1600,
        2000,
        2400,
        2800,
        3300,
        3900,
        4600,
        5500,
        6800,
        7500,
        //先写12个
    };
    int idx = level - 1;
    if (idx >= 0 && idx < klevel_stats.size())
    {
        data.health = klevel_stats[idx];
    }
    return data;
}

//我们这里把建筑工人小屋当纯建筑了，不是资源、防御、城墙
DefenderData DefenderData::CreateBuildersHutData(int level)
{
    DefenderData data;
    data.id = "builders_hut";
    data.name = "建筑工人屋";
    data.level = level;
    data.building_type = BuildingType::BUILDERSHUT;
    data.tile_width = 3;
    data.tile_height = 3;
    data.damage = 0; // 不反击  
    //唯一没有参照COC官方的数据，我们修改建筑小屋生命值不要太大，和金矿一致
    static const std::vector<int> klevel_stats = {
        75,
        150,
        300,
        400,
        500,
        550,
        600,
        660,
        720,
        780,
        860,
        960,
        //先写12个
    };
    int idx = level - 1;
    if (idx >= 0 && idx < klevel_stats.size())
    {
        data.health = klevel_stats[idx];
    }
    return data;
}

//兵营
DefenderData DefenderData::CreateArmyCampData(int level)
{
    DefenderData data;
    data.id = "army_camp";
    data.name = "兵营";
    data.level = level;
    data.building_type = BuildingType::ARMYCAMP;
    data.tile_width = 4;
    data.tile_height = 4;
    data.damage = 0; // 不反击  
    //唯一没有参照COC官方的数据，我们修改建筑小屋生命值不要太大，和金矿一致
    static const std::vector<int> klevel_stats = {
        100,
        150,
        200,
        250,
        300,
        330,
        400,
        500,
        600,
        700,
        800,
        850,
        //先写12个
    };
    int idx = level - 1;
    if (idx >= 0 && idx < klevel_stats.size())
    {
        data.health = klevel_stats[idx];
    }
    return data;
}