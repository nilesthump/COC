/*
 * UnitFactory.h - 单位工厂类
 *
 * 核心职责：创建和配置战斗单位的工厂类
 * 设计模式：抽象工厂模式 + 建造者模式
 *
 * 设计原则：
 * 1. 单一职责原则：工厂只负责创建，不负责业务逻辑
 * 2. 开闭原则：新增单位类型无需修改现有代码
 * 3. 依赖倒置原则：高层模块不依赖低层模块，都依赖抽象
 *
 * 核心功能：
 * 1. 创建基础单位（仅数据层）
 * 2. 创建完整单位（带视觉和音效，background是为了和地图坐标匹配）
 *
 * 使用方式：
 * 1. 简单创建：UnitFactory::CreateBarbarian(1)
 * 2. 完整创建：UnitFactory::CreateCompleteBarbarian(1, parentNode,background)
 */

//12/16 下一步把别的角色工厂也添加，然后试着提取共有部分批量创建
//（但是初始位置还是得从scene读取，这里对角色自己的工厂而已）
#ifndef UNITFACTORY_H
#define UNITFACTORY_H
#include "BattleUnit.h"
#include "AttackerData.h"
#include "DefenderData.h"
#include "AttackerNavigation.h"
#include "DefenderNavigation.h"
#include "AttackerNormalBehavior.h"
#include "DefenderNormalBehavior.h"
#include "StaticBuildingBehavior.h"
#include "cocos2d.h" 

namespace cocos2d
{
    class Node;
}

class UnitFactory
{
public:
    // 创建带有视觉效果的完整单位（用于场景直接使用）
    static BattleUnit* CreateBarbarian(int level = 1, cocos2d::Node* parent = nullptr, cocos2d::Sprite* background = nullptr);
    static BattleUnit* CreateArcher(int level = 1, cocos2d::Node* parent = nullptr, cocos2d::Sprite* background = nullptr);
    static BattleUnit* CreateCannon(int level = 1, cocos2d::Node* parent = nullptr, cocos2d::Sprite* background = nullptr);
    static BattleUnit* CreateArcherTower(int level = 1, cocos2d::Node* parent = nullptr, cocos2d::Sprite* background = nullptr);
    static BattleUnit* CreateGoldMine(int level = 1, cocos2d::Node* parent = nullptr, cocos2d::Sprite* background = nullptr);

};

#endif