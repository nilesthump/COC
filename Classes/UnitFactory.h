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
//12/27 重构，漂亮了
#ifndef UNITFACTORY_H
#define UNITFACTORY_H
#include <string>
#include <algorithm>
#include "BattleUnit.h"
#include "AttackerData.h"
#include "DefenderData.h"
#include "AttackerNavigation.h"
#include "DefenderNavigation.h"
#include "BomberNavigation.h"
#include "AttackerNormalBehavior.h"
#include "DefenderNormalBehavior.h"
#include "StaticBuildingBehavior.h"
#include "BomberBehavior.h"
#include "cocos2d.h" 

namespace cocos2d
{
    class Node;
}

class UnitFactory
{
private:
    //专门负责处理防守建筑通用逻辑：创建、初始化Data、绑定Component、设置HP条、设置Behavior
    static BattleUnit* CreateBaseBuilding(DefenderData data,
        const std::string& spritePath,
        cocos2d::Node* parent,
        cocos2d::Sprite* background,
        bool canAttack = false);

    //专门负责处理小兵通用逻辑
    static BattleUnit* CreateBaseAttacker(AttackerData data,
        const std::string& spritePath,
        cocos2d::Node* parent,
        cocos2d::Sprite* background);
public:
    //根据等级传图片，最大为9
    static std::string makeSpritePath(const std::string& prefix, int level);

    //根据UnitType创建进攻方
    static BattleUnit* CreateAttackerByType(UnitType type, int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    //根据 BuildingType创建防守方
    static BattleUnit* CreateBuildingByType(BuildingType type, int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    //创建带有视觉效果的完整单位
    static BattleUnit* CreateBarbarian(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    static BattleUnit* CreateArcher(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    static BattleUnit* CreateGiant(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    static BattleUnit* CreateGoblin(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    static BattleUnit* CreateBomber(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    static BattleUnit* CreateBalloon(int level, cocos2d::Node* parent, cocos2d::Sprite* background);

    static BattleUnit* CreateCannon(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    static BattleUnit* CreateArcherTower(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    static BattleUnit* CreateMortar(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    static BattleUnit* CreateWall(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    static BattleUnit* CreateGoldMine(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    static BattleUnit* CreateGoldStorage(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    static BattleUnit* CreateElixir(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    static BattleUnit* CreateElixirStorage(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    static BattleUnit* CreateTownHall(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    static BattleUnit* CreateBuildersHut(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    static BattleUnit* CreateArmyCamp(int level, cocos2d::Node* parent, cocos2d::Sprite* background);
    
};

#endif