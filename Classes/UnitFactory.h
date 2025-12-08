//仅用来创建角色
#ifndef UNITFACTORY_H
#define UNITFACTORY_H
#include"BattleUnit.h"
#include "NormalBehavior.h"
#include "DefenseBehavior.h"
#include "BarbarianNavigation.h"
#include "CharacterData.h"
#include "DefenseData.h"

class UnitFactory
{
public:
    static BattleUnit* CreateBarbarian(int level = 1)
    {
        CharacterData data = CharacterData::CreateBarbarianData(level);
        BattleUnit* unit = new BattleUnit();
        unit->Init(data);
        unit->SetRule(new BarbarianRule());
        unit->SetWay(new BarbarianWay());
        return unit;
    }
    
    static BattleUnit* CreateArcher(int level = 1)
    {
        CharacterData data = CharacterData::CreateArcherData(level);
        BattleUnit* unit = new BattleUnit();
        unit->Init(data);
        unit->SetRule(new ArcherRule());
        unit->SetWay(new ArcherWay());
        return unit;
    }
    // ... 其他角色
};

#endif