/*
 * UnitFactory.h - 单位工厂类
 *
 * 核心职责：创建和配置战斗单位的工厂类
 * 设计模式：工厂模式 + 简单工厂
 *
 * 为什么需要工厂：
 * 1. 封装对象创建逻辑，避免在业务代码中直接new
 * 2. 统一单位创建接口，便于管理
 * 3. 隔离具体实现，便于扩展新单位
 * 4. 集中管理组件依赖关系
 *
 * 创建流程：
 * 1. 获取静态数据（AttackerData/DefenderData）
 * 2. 创建BattleUnit空壳
 * 3. 根据角色类型注入对应组件
 * 4. 返回配置完成的战斗单位
 */
#ifndef UNITFACTORY_H
#define UNITFACTORY_H
#include "BattleUnit.h"
#include "AttackerNormalBehavior.h"
#include "DefenderNormalBehavior.h"
#include "BarbarianNavigation.h"
#include "AttackerData.h"
#include "DefenderData.h"

class UnitFactory
{
public:
    //创建进攻单位
    static BattleUnit* CreateBarbarian(int level = 1)
    {
        AttackerData data = AttackerData::CreateBarbarianData(level);
        BattleUnit* unit = new BattleUnit();
        unit->Init(data);
        unit->SetBehavior(new AttackerNormalBehavior()); //先用通用行为
        unit->SetNavigation(new BarbarianNavigation());
        return unit;
    }

    static BattleUnit* CreateArcher(int level = 1)
    {
        AttackerData data = AttackerData::CreateArcherData(level);
        BattleUnit* unit = new BattleUnit();
        unit->Init(data);
        unit->SetBehavior(new AttackerNormalBehavior());
        unit->SetNavigation(new BarbarianNavigation()); // 先用野蛮人导航
        return unit;
    }

    // 创建防御建筑
    static BattleUnit* CreateArcherTower(int level = 1)
    {
        DefenderData data = DefenderData::CreateArcherTowerData(level);
        BattleUnit* unit = new BattleUnit();
        unit->Init(data);
        unit->SetBehavior(new DefenderNormalBehavior());
        unit->SetNavigation(nullptr); // 防御建筑无导航
        return unit;
    }
};

#endif