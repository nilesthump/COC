#include "BuildingInfoPanel.h"
#include "GoldMine.h"
#include "ElixirCollector.h"
#include "SecondScene.h" // 用于访问全局变量 g_goldCount 等

using namespace cocos2d;

BuildingInfoPanel* BuildingInfoPanel::create(Building* building, cocos2d::Sprite* background_sprite_) {
    BuildingInfoPanel* panel = new (std::nothrow) BuildingInfoPanel();
    if (panel && panel->init(building, background_sprite_)) {
        panel->autorelease();
        return panel;
    }
    CC_SAFE_DELETE(panel);
    return nullptr;
}

bool BuildingInfoPanel::init(Building* building, cocos2d::Sprite* background_sprite_) {
    if (!Node::init()) {
        return false;
    }
    _targetBuilding = building;
    temp = background_sprite_;

    if (!_targetBuilding) {

        return false; // 建筑为空则初始化失败
    }
    /*
    // 1. 半透明遮罩（点击关闭面板）
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto mask = LayerColor::create(Color4B(0, 0, 0, 128), visibleSize.width, visibleSize.height);
    this->addChild(mask);

    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [=](Touch*, Event*) {
        this->removeFromParent(); // 点击遮罩关闭
        return true;
        };
    touchListener->setSwallowTouches(true);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, mask);*/

    // 1. 面板主体
    auto panelBg = Sprite::create("btn_long.png"); // 面板背景图
    if (!panelBg) {
        return false;
    }
    panelBg->setPosition(building->getContentSize().width + panelBg->getContentSize().width / 2, building->getContentSize().height - panelBg->getContentSize().height / 2);
    building->addChild(panelBg);
    float bgWidth = panelBg->getContentSize().width;
    float bgHeight = panelBg->getContentSize().height;

    // 2. 标题（区分金矿/圣水收集器）
    std::string type = dynamic_cast<GoldMine*>(building) ? "GoldMine" : "ElixirCollector";
    _titleLabel = Label::createWithTTF(
        StringUtils::format("%s Lv.%d", type.c_str(), building->getLv()),
        "fonts/Marker Felt.ttf", 24
    );
    _titleLabel->setPosition(bgWidth / 2, bgHeight - 30);
    panelBg->addChild(_titleLabel);

    // 3. 血量信息
    _hpLabel = Label::createWithTTF(
        StringUtils::format("HP: %d", building->getHp()),
        "fonts/Marker Felt.ttf", 24
    );
    _hpLabel->setPosition(bgWidth / 2, bgHeight - 70);
    panelBg->addChild(_hpLabel);

    // 4. 生产速度信息
    _speedLabel = Label::createWithTTF(
        StringUtils::format("generateSpeed: %.1f/s", building->getSpeed()),
        "fonts/Marker Felt.ttf", 24
    );
    _speedLabel->setPosition(bgWidth / 2, bgHeight - 110);
    panelBg->addChild(_speedLabel);

    //5.网格坐标
    _positionLabel= Label::createWithTTF(
        StringUtils::format("(x,y):(%.1f,%.1f)", building->getXX(background_sprite_),building->getYY(background_sprite_)),
        "fonts/Marker Felt.ttf", 24
    );
    _positionLabel->setPosition(bgWidth / 2, bgHeight - 150);
    panelBg->addChild(_positionLabel);
    
    // 6. 资源数量显示
// 判断建筑类型并显示对应资源
    if (dynamic_cast<GoldMine*>(building)) {
        _resourceLabel = Label::createWithTTF(
            StringUtils::format("Gold: %d", building->getCurrentStock()), 
            "fonts/Marker Felt.ttf", 24
        );
    }
    else if (dynamic_cast<ElixirCollector*>(building)) {
        _resourceLabel = Label::createWithTTF(
            StringUtils::format("Elixir: %d", building->getCurrentStock()),
            "fonts/Marker Felt.ttf", 24
        );
    }
    _resourceLabel->setPosition(bgWidth / 2, bgHeight - 190); // 调整位置在坐标下方
    panelBg->addChild(_resourceLabel);

    // 7. 升级按钮
    _upgradeBtn = MenuItemImage::create(
        "btn_disabled.png",  // 正常状态图
        "btn_disabled.png", // 按下状态图
        CC_CALLBACK_1(BuildingInfoPanel::onUpgradeClicked, this)
    );
    //设置文字提示
    auto upGradeLabel = Label::createWithSystemFont("upGrade", "fonts/Marker Felt.ttf", 24);
    upGradeLabel->setColor(Color3B::WHITE);
    upGradeLabel->setPosition(Vec2(_upgradeBtn->getContentSize().width / 2,
        _upgradeBtn->getContentSize().height / 2));
    _upgradeBtn->addChild(upGradeLabel);
    //缩放和位置
    _upgradeBtn->setScale(0.75f);
    _upgradeBtn->setPosition(bgWidth / 2, 70);

    // 8. 收集按钮
    _collectBtn = MenuItemImage::create(
        "btn_disabled.png",  // 正常状态图
        "btn_disabled.png", // 按下状态图
        CC_CALLBACK_1(BuildingInfoPanel::onCollectClicked, this)
    );
    //设置文字提示
    auto collectLabel = Label::createWithSystemFont("collect", "fonts/Marker Felt.ttf", 24);
    collectLabel->setColor(Color3B::WHITE);
    collectLabel->setPosition(Vec2(_collectBtn->getContentSize().width / 2,
        _collectBtn->getContentSize().height / 2));
    _collectBtn->addChild(collectLabel);
    //缩放和位置
    _collectBtn->setScale(0.75f);
    _collectBtn->setPosition(bgWidth / 2, 120); // 位置在升级按钮上方

    // 菜单容器
    auto menu = Menu::create(_collectBtn,_upgradeBtn,nullptr);
    menu->setPosition(Vec2::ZERO);
    panelBg->addChild(menu);

    return true;
}

void BuildingInfoPanel::updateInfo(Building* building, cocos2d::Sprite* background_sprite_) {
    if (!building) return;
    std::string type = dynamic_cast<GoldMine*>(building) ? "GoldMine" : "ElixirCollector";
    _titleLabel->setString(StringUtils::format("%s Lv.%d", type.c_str(), building->getLv()));
    _hpLabel->setString(StringUtils::format("HP: %d", building->getHp()));
    _speedLabel->setString(StringUtils::format("generateSpeed: %.1f/s", building->getSpeed()));
    _positionLabel->setString(StringUtils::format("(x,y):(%.1f,%.1f)", building->getXX(background_sprite_), building->getYY(background_sprite_)));
    // 添加资源数量更新
    if (dynamic_cast<GoldMine*>(building)) {
        _resourceLabel->setString(StringUtils::format("Gold: %d", building->getCurrentStock()));
    }
    else if (dynamic_cast<ElixirCollector*>(building)) {
        _resourceLabel->setString(StringUtils::format("Elixir: %d", building->getCurrentStock()));
    }
}

void BuildingInfoPanel::onUpgradeClicked(Ref* sender) {
    if (!_targetBuilding) return;

    // 升级逻辑（示例：消耗金币，提升属性）
    
}
void BuildingInfoPanel::onCollectClicked(Ref* sender) {
    if (!_targetBuilding) return;

    int collected = 0;
    // 判断建筑类型并执行对应收集逻辑
    if (auto goldMine = dynamic_cast<GoldMine*>(_targetBuilding)) {
        collected = goldMine->collectStock();
        if (collected > 0) {
            g_goldCount += collected;
        }
    }
    else if (auto elixirCollector = dynamic_cast<ElixirCollector*>(_targetBuilding)) {
        collected = elixirCollector->collectStock();
        if (collected > 0) {
            g_elixirCount += collected;
        }
    }
    // 更新面板显示的资源数量
    if (collected > 0) {
        updateInfo(_targetBuilding,temp); // 刷新信息显示
    }
}
