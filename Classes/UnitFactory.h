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
#include "AttackerNormalBehavior.h"
#include "DefenderNormalBehavior.h"
#include "BarbarianNavigation.h"
#include "CannonNavigation.h"
#include "AttackerData.h"
#include "DefenderData.h"
#include "cocos2d.h" 

namespace cocos2d
{
    class Node;
}

class UnitFactory
{
public:
    //创建野蛮人单位
    static BattleUnit* CreateBarbarian(int level = 1)
    {
        AttackerData data = AttackerData::CreateBarbarianData(level);
        BattleUnit* unit = new BattleUnit();
        unit->Init(data);
        unit->SetBehavior(std::make_unique<AttackerNormalBehavior>()); //先用通用行为
        unit->SetNavigation(std::make_unique <BarbarianNavigation>());
        return unit;
    }

    // 创建加农炮单位
    static BattleUnit* CreateCannon(int level = 1)
    {
        DefenderData data = DefenderData::CreateCannonData(level);
        BattleUnit* unit = new BattleUnit();
        unit->Init(data);
        unit->SetBehavior(std::make_unique <DefenderNormalBehavior>());
        unit->SetNavigation(std::make_unique<CannonNavigation>()); // 使用加农炮导航（固定，无移动）
        return unit;
    }

    // 创建带有视觉效果的完整单位（用于场景直接使用）
    static BattleUnit* CreateCompleteBarbarian(int level, cocos2d::Node* parent, cocos2d::Sprite* background)
    {
        BattleUnit* unit = CreateBarbarian(level);

        unit->SetBackgroundSprite(background);
        // 设置精灵
        auto sprite = cocos2d::Sprite::create("BarbarianLv1.png");
        if (sprite)
        {
            sprite->setScale(0.5f);
            unit->SetSprite(sprite, parent); //这里会调用UpdateSpritePosition
        }

        // 设置血条
        unit->SetupHealthBar(parent);

        // 设置音效
        unit->SetAttackSound("sounds/barbarian_attack.mp3");
        unit->SetDeathSound("sounds/barbarian_death.mp3");

        return unit;
    }

    static BattleUnit* CreateCompleteCannon(int level, cocos2d::Node* parent, cocos2d::Sprite* background)
    {
        BattleUnit* unit = CreateCannon(level);

        unit->SetBackgroundSprite(background);
        // 设置精灵
        auto sprite = cocos2d::Sprite::create("CannonLv1.png");
        if (sprite)
        {
            sprite->setScale(0.3f);
            unit->SetSprite(sprite, parent);
        }

        // 设置血条
        unit->SetupHealthBar(parent);

        // 设置音效
        unit->SetAttackSound("sounds/cannon_attack.mp3");
        unit->SetDeathSound("sounds/cannon_death.mp3");

        return unit;
    }
};

#endif