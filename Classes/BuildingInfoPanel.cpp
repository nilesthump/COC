#include "BuildingInfoPanel.h"
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

    //兵营的额外训练面板
    if (dynamic_cast<ArmyCamp*>(building))
    {
        armyExtraPanel = Node::create();
        armyExtraPanel->setPosition(Vec2::ZERO);     
        this->addChild(armyExtraPanel, 100);

        auto armybg = Sprite::create("btn_flat.png");
        if (!armybg) {
            return false;
        }
        float bgWidth = armybg->getContentSize().width;
        float bgHeight = armybg->getContentSize().height;
        armybg->setPosition(Vec2(bgWidth / 2, bgHeight / 2));
        armyExtraPanel->addChild(armybg,100);


    }

 

    // 1. 面板主体
    auto panelBg = Sprite::create("btn_long.png"); // 面板背景图
    if (!panelBg) {
        return false;
    }
    panelBg->setPosition(building->getContentSize().width + panelBg->getContentSize().width / 2, building->getContentSize().height - panelBg->getContentSize().height / 2);
    this->addChild(panelBg);
    float bgWidth = panelBg->getContentSize().width;
    float bgHeight = panelBg->getContentSize().height;

    // 2. 标题
    std::string type;
    if (dynamic_cast<GoldMine*>(building)) {
        type = "GoldMine";
    }
    else if (dynamic_cast<ElixirCollector*>(building)) {
        type = "ElixirCollector";
    }
    else if (dynamic_cast<GoldStorage*>(building)) {
        type = "GoldStorage";
    }
    else if (dynamic_cast<ElixirStorage*>(building)) {
        type = "ElixirStorage";
    }
    else if (dynamic_cast<ArmyCamp*>(building)) {
        type = "ArmyCamp";
    }
    else if (dynamic_cast<Walls*>(building)) {
        type = "Walls";
    }
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
    
    // 4. 资源信息显示
    // 判断建筑类型并显示对应资源，金矿和圣水收集器显示的是当前存贮的资源和生产速度，有收集按钮
    // 存钱罐和圣水瓶显示的是容量，无收集按钮
    //兵营待定，城墙不需要显示
    if (dynamic_cast<GoldMine*>(building)) {
        _speedLabel = Label::createWithTTF(
            StringUtils::format("generateSpeed: %.1f/s", building->getSpeed()),
            "fonts/Marker Felt.ttf", 24
        );
        _speedLabel->setPosition(bgWidth / 2, bgHeight - 110);
        panelBg->addChild(_speedLabel);

        _resourceLabel = Label::createWithTTF(
            StringUtils::format("Gold: %d", building->getCurrentStock()), 
            "fonts/Marker Felt.ttf", 24
        );  
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 150); // 调整位置在坐标下方
        panelBg->addChild(_resourceLabel);

    }
    else if (dynamic_cast<ElixirCollector*>(building)) {
        _speedLabel = Label::createWithTTF(
            StringUtils::format("generateSpeed: %.1f/s", building->getSpeed()),
            "fonts/Marker Felt.ttf", 24
        );
        _speedLabel->setPosition(bgWidth / 2, bgHeight - 110);
        panelBg->addChild(_speedLabel);

        _resourceLabel = Label::createWithTTF(
            StringUtils::format("Elixir: %d", building->getCurrentStock()),
            "fonts/Marker Felt.ttf", 24
        );
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 150); // 调整位置在坐标下方
        panelBg->addChild(_resourceLabel);
    }
    else if (dynamic_cast<GoldStorage*>(building)) {
        _resourceLabel = Label::createWithTTF(
            StringUtils::format("GoldVolum: %d", building->getMaxStock()),
            "fonts/Marker Felt.ttf", 24
        );
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 150); // 调整位置在坐标下方
        panelBg->addChild(_resourceLabel);
    }
    else if (dynamic_cast<ElixirStorage*>(building)) {
        _resourceLabel = Label::createWithTTF(
            StringUtils::format("ElixirVolum: %d", building->getMaxStock()),
            "fonts/Marker Felt.ttf", 24
        );
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 150); // 调整位置在坐标下方
        panelBg->addChild(_resourceLabel);
    }

    //5.网格坐标
    _positionLabel = Label::createWithTTF(
        StringUtils::format("(x,y):(%.1f,%.1f)", building->getXX(), building->getYY()),
        "fonts/Marker Felt.ttf", 24
    );
    _positionLabel->setPosition(bgWidth / 2, bgHeight - 190);
    panelBg->addChild(_positionLabel);

    // 6. 升级按钮,最高等级15
    _upgradeBtn = MenuItemImage::create(
        "btn_disabled.png",  // 正常状态图
        "btn_disabled.png", // 按下状态图
        CC_CALLBACK_1(BuildingInfoPanel::onUpgradeClicked, this)
     );
    //设置文字提示
    if (_targetBuilding->getLv() < 15) {
        auto upGradeLabel = Label::createWithSystemFont("upGrade", "fonts/Marker Felt.ttf", 24);
        upGradeLabel->setColor(Color3B::WHITE);
        upGradeLabel->setPosition(Vec2(_upgradeBtn->getContentSize().width / 2, _upgradeBtn->getContentSize().height / 2));
        _upgradeBtn->addChild(upGradeLabel);
    }
    else {
        auto upGradeLabel = Label::createWithSystemFont("maxGrade", "fonts/Marker Felt.ttf", 24);
        upGradeLabel->setColor(Color3B::RED);
        upGradeLabel->setPosition(Vec2(_upgradeBtn->getContentSize().width / 2, _upgradeBtn->getContentSize().height / 2));
        _upgradeBtn->addChild(upGradeLabel);
    }
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
    if (dynamic_cast<GoldMine*>(building)|| dynamic_cast<ElixirCollector*>(building)) {
        auto menu = Menu::create(_collectBtn, _upgradeBtn, nullptr);
        menu->setPosition(Vec2::ZERO);
        panelBg->addChild(menu);
    }
    else {
        auto menu = Menu::create(_upgradeBtn, nullptr);
        menu->setPosition(Vec2::ZERO);
        panelBg->addChild(menu);
    }
    return true;
}

void BuildingInfoPanel::updateInfo(Building* building, cocos2d::Sprite* background_sprite_) {
    if (!building) return;
    std::string type;
    if (dynamic_cast<GoldMine*>(building)) {
        type = "GoldMine";
        _speedLabel->setString(StringUtils::format("generateSpeed: %.1f/s", building->getSpeed()));
    }
    else if (dynamic_cast<ElixirCollector*>(building)) {
        type = "ElixirCollector";
        _speedLabel->setString(StringUtils::format("generateSpeed: %.1f/s", building->getSpeed()));
    }
    else if (dynamic_cast<GoldStorage*>(building)) {
        type = "GoldStorage";
    }
    else if (dynamic_cast<ElixirStorage*>(building)) {
        type = "ElixirStorage";
    }
    else if (dynamic_cast<ArmyCamp*>(building)) {
        type = "ArmyCamp";
    }
    else if (dynamic_cast<Walls*>(building)) {
        type = "Walls";
    }
    _titleLabel->setString(StringUtils::format("%s Lv.%d", type.c_str(), building->getLv()));
    _hpLabel->setString(StringUtils::format("HP: %d", building->getHp()));
    _positionLabel->setString(StringUtils::format("(x,y):(%.1f,%.1f)", building->getXX(), building->getYY()));
    // 添加资源数量更新
    if (dynamic_cast<GoldMine*>(building)) {
        _speedLabel ->setString(StringUtils::format("gengrateSpeed: %.1f/s", building->getSpeed()));
        _resourceLabel->setString(StringUtils::format("Gold: %d", building->getCurrentStock()));
    }
    else if (dynamic_cast<ElixirCollector*>(building)) {
        _speedLabel->setString(StringUtils::format("gengrateSpeed: %.1f/s", building->getSpeed()));
        _resourceLabel->setString(StringUtils::format("Elixir: %d", building->getCurrentStock()));
    }
    else if (dynamic_cast<GoldStorage*>(building)) {
        _resourceLabel->setString(StringUtils::format("GoldVolum: %d", building->getMaxStock()));
    }
    else if (dynamic_cast<ElixirStorage*>(building)) {
        _resourceLabel->setString(StringUtils::format("ElixirVolum: %d", building->getMaxStock()));
    }

}

void BuildingInfoPanel::onUpgradeClicked(Ref* sender) {
    if (!_targetBuilding) return;

    if (_targetBuilding->getLv() < 15) {
        // 升级所需资源（可根据建筑类型和当前等级调整）
        int requiredGold = 1;//100 * _targetBuilding->getLv();    // 每级所需金币为当前等级*100
        int requiredElixir = 1;//50 * _targetBuilding->getLv();   // 每级所需圣水为当前等级*50

        // 检查是否有足够资源
        if (g_goldCount < requiredGold || g_elixirCount < requiredElixir) {
            // 资源不足，播放失败动画
            _targetBuilding->playFailBlink();
            _targetBuilding->setColor(Color3B::WHITE);
            return;
        }

        // 消耗升级资源
        g_goldCount -= requiredGold;
        g_elixirCount -= requiredElixir;

        _targetBuilding->updateLv(); // 等级提升
        _targetBuilding->updateHp();  // 生命值提升
        _targetBuilding->updateSpeed();  // 生成速度提升
        _targetBuilding->updateSize();  // 最大容量提升

        //根据新等级切换建筑外观
        std::string newTexture;
        if (dynamic_cast<GoldMine*>(_targetBuilding)) {
            // 金矿纹理命名规则："goldMineLv2.png"、"goldMineLv3.png"等
            newTexture = StringUtils::format("GoldMineLv%d.png", _targetBuilding->getLv());
        }
        else if (dynamic_cast<ElixirCollector*>(_targetBuilding)) {
            // 圣水收集器纹理命名规则："elixirCollectorLv2.png"等
            newTexture = StringUtils::format("ElixirCollectorLv%d.png", _targetBuilding->getLv());
        }
        else if (dynamic_cast<GoldStorage*>(_targetBuilding)) {
            newTexture = StringUtils::format("GoldStorageLv%d.png", _targetBuilding->getLv());
        }
        else if (dynamic_cast<ElixirStorage*>(_targetBuilding)) {
            newTexture = StringUtils::format("ElixirStorageLv%d.png", _targetBuilding->getLv());
        }
        else if (dynamic_cast<ArmyCamp*>(_targetBuilding)) {
            newTexture = StringUtils::format("ArmyCampLv%d.png", _targetBuilding->getLv());
        }
        else if (dynamic_cast<Walls*>(_targetBuilding)) {
            newTexture = StringUtils::format("WallsLv%d.png", _targetBuilding->getLv());
        }
        // 调用 更新纹理
        _targetBuilding->updateTexture(newTexture);
        // 播放升级成功动画
        _targetBuilding->playSuccessBlink();
    }
    // 更新信息面板显示
    updateInfo(_targetBuilding, temp);
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
