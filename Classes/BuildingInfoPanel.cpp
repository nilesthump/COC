#include "BuildingInfoPanel.h"
#include "GoldMine.h"
#include "ElixirCollector.h"
#include "SecondScene.h" // 用于访问全局变量 g_goldCount 等

using namespace cocos2d;

BuildingInfoPanel* BuildingInfoPanel::create(Building* building) {
    BuildingInfoPanel* panel = new (std::nothrow) BuildingInfoPanel();
    if (panel && panel->init(building)) {
        panel->autorelease();
        return panel;
    }
    CC_SAFE_DELETE(panel);
    return nullptr;
}

bool BuildingInfoPanel::init(Building* building) {
    if (!Node::init()) {
        return false;
    }
    _targetBuilding = building;
    if (!_targetBuilding) {

        return false; // 建筑为空则初始化失败
    }

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
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, mask);

    // 2. 面板主体
    auto panelBg = Sprite::create("btn_long.png"); // 面板背景图
    if (!panelBg) {
        return false;
    }
    panelBg->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    this->addChild(panelBg);
    float bgWidth = panelBg->getContentSize().width;
    float bgHeight = panelBg->getContentSize().height;

    // 3. 标题（区分金矿/圣水收集器）
    std::string type = dynamic_cast<GoldMine*>(building) ? "金矿" : "圣水收集器";
    _titleLabel = Label::createWithTTF(
        StringUtils::format("%s Lv.%d", type.c_str(), building->getLv()),
        "fonts/Marker Felt.ttf", 24
    );
    _titleLabel->setPosition(bgWidth / 2, bgHeight - 30);
    panelBg->addChild(_titleLabel);

    // 4. 血量信息
    _hpLabel = Label::createWithTTF(
        StringUtils::format("HP: %d", building->getHp()),
        "fonts/Marker Felt.ttf", 20
    );
    _hpLabel->setPosition(bgWidth / 2, bgHeight - 70);
    panelBg->addChild(_hpLabel);

    // 5. 生产速度信息
    _speedLabel = Label::createWithTTF(
        StringUtils::format("generateSpeed: %.1f/s", building->getSpeed()),
        "fonts/Marker Felt.ttf", 20
    );
    _speedLabel->setPosition(bgWidth / 2, bgHeight - 110);
    panelBg->addChild(_speedLabel);

    // 6. 升级按钮
    _upgradeBtn = MenuItemImage::create(
        "btn_disabled.png",  // 正常状态图
        "btn_disabled.png", // 按下状态图
        CC_CALLBACK_1(BuildingInfoPanel::onUpgradeClicked, this)
    );
    _upgradeBtn->setPosition(bgWidth / 2, 70);

    // 菜单容器
    auto menu = Menu::create(_upgradeBtn,nullptr);
    menu->setPosition(Vec2::ZERO);
    panelBg->addChild(menu);

    return true;
}

void BuildingInfoPanel::updateInfo(Building* building) {
    if (!building) return;
    std::string type = dynamic_cast<GoldMine*>(building) ? "金矿" : "圣水收集器";
    _titleLabel->setString(StringUtils::format("%s Lv.%d", type.c_str(), building->getLv()));
    _hpLabel->setString(StringUtils::format("血量: %d", building->getHp()));
    _speedLabel->setString(StringUtils::format("生产速度: %.1f/s", building->getSpeed()));
}

void BuildingInfoPanel::onUpgradeClicked(Ref* sender) {
    if (!_targetBuilding) return;

    // 升级逻辑（示例：消耗金币，提升属性）
    
}
