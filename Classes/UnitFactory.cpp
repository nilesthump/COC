#include"UnitFactory.h"

BattleUnit* UnitFactory::CreateBarbarian(int level, cocos2d::Node* parent, cocos2d::Sprite* background)
{
    AttackerData data = AttackerData::CreateBarbarianData(level);
    BattleUnit* unit = new BattleUnit();
    unit->Init(data);
    unit->SetBehavior(std::make_unique<AttackerNormalBehavior>()); //先用通用行为
    unit->SetNavigation(std::make_unique <BarbarianNavigation>());

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

BattleUnit* UnitFactory::CreateArcher(int level, cocos2d::Node* parent, cocos2d::Sprite* background)
{
    AttackerData data = AttackerData::CreateArcherData(level);
    BattleUnit* unit = new BattleUnit();
    unit->Init(data);
    unit->SetBehavior(std::make_unique<AttackerNormalBehavior>()); //先用通用行为
    unit->SetNavigation(std::make_unique <ArcherNavigation>());

    unit->SetBackgroundSprite(background);
    // 设置精灵
    auto sprite = cocos2d::Sprite::create("ArcherLv1.png");
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

BattleUnit* UnitFactory::CreateCannon(int level, cocos2d::Node* parent, cocos2d::Sprite* background)
{
    DefenderData data = DefenderData::CreateCannonData(level);
    BattleUnit* unit = new BattleUnit();
    unit->Init(data);
    unit->SetBehavior(std::make_unique <DefenderNormalBehavior>());
    unit->SetNavigation(std::make_unique<CannonNavigation>()); // 使用加农炮导航（固定，无移动）

    unit->SetBackgroundSprite(background);
    int tile_w = data.tile_width;
    int tile_h = data.tile_height;

    //-----------------------------------------------调用BuildingComponent过程
    //建筑组件
    auto building = std::make_unique<BuildingComponent>(tile_w, tile_h);
    building->AttachTo(parent);

    //图片
    auto sprite = Sprite::create("CannonLv1.png");
    building->AttachSprite(sprite);

    //自动适配格子（关键）
    building->FitSpriteToFootprint();

    unit->SetBuildingComponent(std::move(building));
    //-------------------------------------------------

    // 设置血条
    unit->SetupHealthBar(parent);

    // 设置音效
    unit->SetAttackSound("sounds/cannon_attack.mp3");
    unit->SetDeathSound("sounds/cannon_death.mp3");

    return unit;
}

BattleUnit* UnitFactory::CreateArcherTower(int level, cocos2d::Node* parent, cocos2d::Sprite* background)
{
    DefenderData data = DefenderData::CreateArcherTowerData(level);
    BattleUnit* unit = new BattleUnit();
    unit->Init(data);
    unit->SetBehavior(std::make_unique <DefenderNormalBehavior>());
    unit->SetNavigation(std::make_unique<ArcherTowerNavigation>());

    unit->SetBackgroundSprite(background);
    int tile_w = data.tile_width;
    int tile_h = data.tile_height;

    //-----------------------------------------------调用BuildingComponent过程
    //建筑组件
    auto building = std::make_unique<BuildingComponent>(tile_w, tile_h);
    building->AttachTo(parent);

    //图片
    auto sprite = Sprite::create("ArcherTowerLv1.png");
    building->AttachSprite(sprite);

    //自动适配格子（关键）
    building->FitSpriteToFootprint();

    unit->SetBuildingComponent(std::move(building));
    //-------------------------------------------------

    // 设置血条
    unit->SetupHealthBar(parent);

    // 设置音效
    unit->SetAttackSound("sounds/cannon_attack.mp3");
    unit->SetDeathSound("sounds/cannon_death.mp3");

    return unit;
}