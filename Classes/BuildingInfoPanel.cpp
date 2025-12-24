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
        armyExtraPanel->addChild(armybg);

 
        
        // 添加士兵图像按钮
        barbarianBtn = MenuItemImage::create(
            "Barbarian.png",
            "Barbarian.png",
            [](cocos2d::Ref* sender) {
                // 暂时为空，后续实现点击功能
            }
        );
        barbarianBtn->setScale(0.3f);
        barbarianBtn->setPosition(barbarianBtn->getContentSize().width * 0.25, barbarianBtn->getContentSize().height * 0.3); // 可根据需要调整位置     

        archerBtn = MenuItemImage::create(
            "Archer.png",  
            "Archer.png", 
            [](cocos2d::Ref* sender) {
                // 暂时为空，后续实现点击功能
            }
        );
        archerBtn->setScale(0.3f);
        archerBtn->setPosition(archerBtn->getContentSize().width*0.55, archerBtn->getContentSize().height*0.32); // 可根据需要调整位置

        giantBtn = MenuItemImage::create(
            "Giant.png",
            "Giant.png",
            [](cocos2d::Ref* sender) {
                // 暂时为空，后续实现点击功能
            }
        );
        giantBtn->setScale(0.3f);
        giantBtn->setPosition(giantBtn->getContentSize().width * 0.95, giantBtn->getContentSize().height * 0.3); // 可根据需要调整位置

        goblinBtn = MenuItemImage::create(
            "Goblin.png",
            "Goblin.png",
            [](cocos2d::Ref* sender) {
                // 暂时为空，后续实现点击功能
            }
        );
        goblinBtn->setScale(0.3f);
        goblinBtn->setPosition(archerBtn->getContentSize().width * 1.3, archerBtn->getContentSize().height * 0.3); // 可根据需要调整位置

        bomberBtn = MenuItemImage::create(
            "Bomber.png",
            "Bomber.png",
            [](cocos2d::Ref* sender) {
                // 暂时为空，后续实现点击功能
            }
        );
        bomberBtn->setScale(0.3f);
        bomberBtn->setPosition(archerBtn->getContentSize().width * 1.7, archerBtn->getContentSize().height * 0.3); // 可根据需要调整位置

        balloonBtn = MenuItemImage::create(
            "Bomber.png",
            "Bomber.png",
            [](cocos2d::Ref* sender) {
                // 暂时为空，后续实现点击功能
            }
        );
        balloonBtn->setScale(0.3f);
        balloonBtn->setPosition(archerBtn->getContentSize().width * 2.1, archerBtn->getContentSize().height * 0.3); // 可根据需要调整位置

        // 创建菜单
        menu = cocos2d::Menu::create(barbarianBtn, archerBtn, giantBtn, goblinBtn, bomberBtn,balloonBtn, nullptr);
        menu->setPosition(0, 0); // 菜单位置相对于父节点（armyExtraPanel）
        armyExtraPanel->addChild(menu);
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
    
    //4.网格坐标
    _positionLabel = Label::createWithTTF(
        StringUtils::format("(x,y):(%.1f,%.1f)", building->getXX(), building->getYY()),
        "fonts/Marker Felt.ttf", 24
    );
    _positionLabel->setPosition(bgWidth / 2, bgHeight - 110);
    panelBg->addChild(_positionLabel);


    // 5. 资源信息显示
    // 判断建筑类型并显示对应资源，金矿和圣水收集器显示的是当前存贮的资源和生产速度，有收集按钮
    // 存钱罐和圣水瓶显示的是容量，无收集按钮
    //兵营待定，城墙不需要显示
    if (dynamic_cast<GoldMine*>(building)) {
        _speedLabel = Label::createWithTTF(
            StringUtils::format("generateSpeed: %.1f/s", building->getSpeed()),
            "fonts/Marker Felt.ttf", 24
        );
        _speedLabel->setPosition(bgWidth / 2, bgHeight - 150);
        panelBg->addChild(_speedLabel);

        _resourceLabel = Label::createWithTTF(
            StringUtils::format("Gold: %d", building->getCurrentStock()), 
            "fonts/Marker Felt.ttf", 24
        );  
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 190); // 调整位置在坐标下方
        panelBg->addChild(_resourceLabel);

    }
    else if (dynamic_cast<ElixirCollector*>(building)) {
        _speedLabel = Label::createWithTTF(
            StringUtils::format("generateSpeed: %.1f/s", building->getSpeed()),
            "fonts/Marker Felt.ttf", 24
        );
        _speedLabel->setPosition(bgWidth / 2, bgHeight - 150);
        panelBg->addChild(_speedLabel);

        _resourceLabel = Label::createWithTTF(
            StringUtils::format("Elixir: %d", building->getCurrentStock()),
            "fonts/Marker Felt.ttf", 24
        );
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 190); // 调整位置在坐标下方
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
    else if (dynamic_cast<ArmyCamp*>(building)) {
        _resourceLabel = Label::createWithTTF(
            StringUtils::format("armyNum/maxNum: %d/%d", building->getCurrentStock(), building->getMaxStock()),
            "fonts/Marker Felt.ttf", 24
        );
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 150); // 调整位置在坐标下方
        panelBg->addChild(_resourceLabel);

        _barbarian = Label::createWithTTF(
            StringUtils::format("barbarianNum: %d", building->getNum(0)),
            "fonts/Marker Felt.ttf", 24
        );
        _barbarian->setPosition(bgWidth / 2, bgHeight - 190); // 调整位置在坐标下方
        panelBg->addChild(_barbarian);

        _archer = Label::createWithTTF(
            StringUtils::format("archerNum: %d", building->getNum(1)),
            "fonts/Marker Felt.ttf", 24
        );
        _archer->setPosition(bgWidth / 2, bgHeight - 230); // 调整位置在坐标下方
        panelBg->addChild(_archer);

        _giant = Label::createWithTTF(
            StringUtils::format("giantNum: %d", building->getNum(2)),
            "fonts/Marker Felt.ttf", 24
        );
        _giant->setPosition(bgWidth / 2, bgHeight - 270); // 调整位置在坐标下方
        panelBg->addChild(_giant);

        _goblin = Label::createWithTTF(
            StringUtils::format("goblinNum: %d", building->getNum(3)),
            "fonts/Marker Felt.ttf", 24
        );
        _goblin->setPosition(bgWidth / 2, bgHeight - 310); // 调整位置在坐标下方
        panelBg->addChild(_goblin);

        _bomber = Label::createWithTTF(
            StringUtils::format("bomberNum: %d", building->getNum(4)),
            "fonts/Marker Felt.ttf", 24
        );
        _bomber->setPosition(bgWidth / 2, bgHeight - 350); // 调整位置在坐标下方
        panelBg->addChild(_bomber);
    }

    
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
    else if (dynamic_cast<ArmyCamp*>(building)) {
        _resourceLabel->setString(
            StringUtils::format("armyNum/maxNum: %d/%d", building->getCurrentStock(), building->getMaxStock()));
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
