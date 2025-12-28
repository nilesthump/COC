#ifndef BOMBERBEHAVIORDATA_H
#define BOMBERBEHAVIORDATA_H

struct BomberBehaviorData
{
    double fuse_time = 0.5;          // 到达目标后的引爆时间
    double active_radius = 0.8;      // 主动爆炸半径
    double death_radius = 1.5;       // 死亡爆炸半径
    int wall_damage_multiplier = 40; // 对城墙的倍伤
    int active_damage = 0;           // 主动爆炸伤害
    int death_damage = 0;            // 死亡爆炸伤害
};

#endif
