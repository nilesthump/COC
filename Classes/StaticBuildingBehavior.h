// StaticBuildingBehavior.h
#ifndef STATIC_BUILDING_BEHAVIOR_H
#define STATIC_BUILDING_BEHAVIOR_H
#include "UnitEnums.h"
#include "UnitBehavior.h"

class StaticBuildingBehavior : public UnitBehavior
{
public:
    // 必须要实现的纯虚函数
    virtual void OnUpdate(BattleUnit* unit, float deltaTime) override;
    virtual void OnDeath(BattleUnit* unit) override;
    virtual void OnDamageTaken(BattleUnit* unit, float damage, BattleUnit* source) override;
    void showLootAnimation(BattleUnit* unit, ResourceType type);
    virtual std::string GetBehaviorType() const override { return "StaticBuilding"; }
};

#endif