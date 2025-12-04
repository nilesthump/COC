#include "CharacerData.h"

//! 还没有规定好格子和距离的关系
CharacterData CharacterData::CreateBarbarianData()
{
    //! 这是一级的时候
    CharacterData data;

    data.id_ = "barbarian";
    data.name_ = "野蛮人";
    data.combat_type_ = CombatType::MELEE;
    data.unit_type_ = UnitType::GROUND;
    data.resource_type_ = ResourceType::ELIXIR;

    data.health_ = 45;
    data.damage_ = 9;
    data.attack_speed_ = 1.0;  //1秒/次
    data.attack_range_ = 0.4;  //0.4格
    data.move_speed_ = 2.0;    //2格/秒
    data.attack_type_ = AttackType::SINGLE_TARGET;

    data.preferred_target_ = TargetPriority::ANY;

    data.housing_space_ = 1;    //占用1人口

    return data;
}
