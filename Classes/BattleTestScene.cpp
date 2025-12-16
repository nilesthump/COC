#include "BattleTestScene.h"
#include "BattleManager.h"
#include "BattleUnit.h"
#include "AttackerData.h"
#include "DefenderData.h"
#include "AttackerNormalBehavior.h"
#include "DefenderNormalBehavior.h"
#include "BarbarianNavigation.h"
#include "cocos2d.h"
#include <memory>

USING_NS_CC;

Scene *BattleTestScene::createScene()
{
    auto scene = Scene::create();
    auto layer = BattleTestScene::create();
    scene->addChild(layer);
    return scene;
}

bool BattleTestScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

    // 初始化战斗管理器
    battleMgr_ = std::make_unique<BattleManager>();
    //battleMgr_->SetBattleActive(true);

    // 创建战场背景
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto background = LayerColor::create(Color4B(50, 100, 50, 255), visibleSize.width, visibleSize.height);
    this->addChild(background);

    // 创建Barbarian(攻击者)
    auto barbarian = std::make_unique<BattleUnit>();
    auto barbarianData = AttackerData::CreateBarbarianData(1);
    barbarian->Init(barbarianData);
    barbarian->SetPosition(100, 200);
    barbarian->SetBehavior(new AttackerNormalBehavior());
    barbarian->SetNavigation(new BarbarianNavigation());

    // 创建Cannon(防御者)
    auto cannon = std::make_unique<BattleUnit>();
    auto cannonData = DefenderData::CreateCannonData(1);
    cannon->Init(cannonData);
    cannon->SetPosition(500, 200);
    cannon->SetBehavior(new DefenderNormalBehavior());
   // cannon->SetNavigation(new CannonNavigation());

    // 添加到战斗管理器
    battleMgr_->AddUnit(std::move(barbarian), true);
    battleMgr_->AddUnit(std::move(cannon), false);

    // 启动战斗循环
    this->scheduleUpdate();

    return true;
}

void BattleTestScene::update(float delta)
{
    // 更新战斗管理器
    if (battleMgr_)
    {
        battleMgr_->Update(delta);
    }
}
