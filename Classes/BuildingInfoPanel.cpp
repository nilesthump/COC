#include "BuildingInfoPanel.h"
#include "SecondScene.h" // 用于访问全局变量 g_goldCount 等

using namespace cocos2d;

//创建面板
BuildingInfoPanel* BuildingInfoPanel::create(Building* building, cocos2d::Sprite* background_sprite_) {
    BuildingInfoPanel* panel = new (std::nothrow) BuildingInfoPanel();
    if (panel && panel->init(building, background_sprite_)) {
        panel->autorelease();
        return panel;
    }
    CC_SAFE_DELETE(panel);
    return nullptr;
}

//面板初始化
bool BuildingInfoPanel::init(Building* building, cocos2d::Sprite* background_sprite_) {
    if (!Node::init()) {
        return false;
    }
    _targetBuilding = building;
    temp = background_sprite_;

    if (!_targetBuilding) {
        return false; // 建筑为空则初始化失败
    }

    //兵营的额外训练面板
    if (dynamic_cast<ArmyCamp*>(building))
    {
        armyExtraPanel = Node::create();
        armyExtraPanel->setPosition(Vec2::ZERO);
        this->addChild(armyExtraPanel, 100);

        auto armybg = Sprite::create("2.png");
        if (!armybg) {return false;}
        float bgWidth = armybg->getContentSize().width;
        float bgHeight = armybg->getContentSize().height;
        armybg->setPosition(Vec2(bgWidth / 2, bgHeight / 2));
        armyExtraPanel->addChild(armybg);

        // 添加士兵图像按钮115125
        barbarianBtn = MenuItemImage::create(
            "Barbarian.png",
            "Barbarian.png",
            [this](cocos2d::Ref* sender) {
                if (auto armyCamp = dynamic_cast<ArmyCamp*>(_targetBuilding)) {
                    // 检查是否还有容量
                    if (armyCamp->getCurrentStock()+ armyCamp->getArmySize(0) <= armyCamp->getMaxStock()) {
                        armyCamp->updateNum(0); // 野蛮人数量+1
                        this->updateInfo(_targetBuilding, temp); // 更新显示
                    }
                }
            }
        );
        barbarianBtn->setScale(0.3f);
        barbarianBtn->setPosition(100, barbarianBtn->getContentSize().height * 0.3); // 可根据需要调整位置     

        archerBtn = MenuItemImage::create(
            "Archer.png",  
            "Archer.png", 
            [this](cocos2d::Ref* sender) {
                if (auto armyCamp = dynamic_cast<ArmyCamp*>(_targetBuilding)) {
                    // 检查是否还有容量
                    if (armyCamp->getCurrentStock() + armyCamp->getArmySize(1) <= armyCamp->getMaxStock()) {
                        armyCamp->updateNum(1); 
                        this->updateInfo(_targetBuilding, temp); // 更新显示
                    }
                }
            }
        );
        archerBtn->setScale(0.3f);
        archerBtn->setPosition(250, archerBtn->getContentSize().height*0.32); // 可根据需要调整位置

        giantBtn = MenuItemImage::create(
            "Giant.png",
            "Giant.png",
            [this](cocos2d::Ref* sender) {
                if (auto armyCamp = dynamic_cast<ArmyCamp*>(_targetBuilding)) {
                    // 检查是否还有容量
                    if (armyCamp->getCurrentStock() + armyCamp->getArmySize(2) <= armyCamp->getMaxStock()) {
                        armyCamp->updateNum(2); 
                        this->updateInfo(_targetBuilding, temp); // 更新显示
                    }
                }
            }
        );
        giantBtn->setScale(0.3f);
        giantBtn->setPosition(400, giantBtn->getContentSize().height * 0.3); // 可根据需要调整位置

        goblinBtn = MenuItemImage::create(
            "Goblin.png",
            "Goblin.png",
            [this](cocos2d::Ref* sender) {
                if (auto armyCamp = dynamic_cast<ArmyCamp*>(_targetBuilding)) {
                    // 检查是否还有容量
                    if (armyCamp->getCurrentStock() + armyCamp->getArmySize(3) <= armyCamp->getMaxStock()) {
                        armyCamp->updateNum(3);
                        this->updateInfo(_targetBuilding, temp); // 更新显示
                    }
                }
            }
        );
        goblinBtn->setScale(0.3f);
        goblinBtn->setPosition(550, archerBtn->getContentSize().height * 0.3); // 可根据需要调整位置

        bomberBtn = MenuItemImage::create(
            "Bomber.png",
            "Bomber.png",
            [this](cocos2d::Ref* sender) {
                if (auto armyCamp = dynamic_cast<ArmyCamp*>(_targetBuilding)) {
                    // 检查是否还有容量
                    if (armyCamp->getCurrentStock() + armyCamp->getArmySize(4) <= armyCamp->getMaxStock()) {
                        armyCamp->updateNum(4); 
                        this->updateInfo(_targetBuilding, temp); // 更新显示
                    }
                }
            }
        );
        bomberBtn->setScale(0.3f);
        bomberBtn->setPosition(700, archerBtn->getContentSize().height * 0.32); // 可根据需要调整位置

        balloonBtn = MenuItemImage::create(
            "Balloon.png",
            "Balloon.png",
            [this](cocos2d::Ref* sender) {
                if (auto armyCamp = dynamic_cast<ArmyCamp*>(_targetBuilding)) {
                    // 检查是否还有容量
                    if (armyCamp->getCurrentStock() + armyCamp->getArmySize(5) <= armyCamp->getMaxStock()) {
                        armyCamp->updateNum(5);
                        this->updateInfo(_targetBuilding, temp); // 更新显示
                    }
                }
            }
        );
        balloonBtn->setScale(0.3f);
        balloonBtn->setPosition(850, archerBtn->getContentSize().height * 0.35); // 可根据需要调整位置

        // 创建菜单
        menu = cocos2d::Menu::create(barbarianBtn, archerBtn, giantBtn, goblinBtn, bomberBtn,balloonBtn, nullptr);
        menu->setPosition(0, 0); // 菜单位置相对于父节点（armyExtraPanel）
        armyExtraPanel->addChild(menu);
    }

 

    // 1. 面板主体
    auto panelBg = Sprite::create("3.png"); // 面板背景图
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
    else if (dynamic_cast<TownHall*>(building)) {
        type = "TownHall";
    }
    _titleLabel = Label::createWithTTF(
        StringUtils::format("%s Lv.%d", type.c_str(), building->getLv()),
        "fonts/Marker Felt.ttf", 24
    );
    _titleLabel->setPosition(bgWidth / 2, bgHeight - 30);
    _titleLabel->setColor(Color3B::BLACK);
    panelBg->addChild(_titleLabel);

    // 3. 血量信息
    _hpLabel = Label::createWithTTF(
        StringUtils::format("HP: %d", building->getHp()),
        "fonts/Marker Felt.ttf", 24
    );
    _hpLabel->setPosition(bgWidth / 2, bgHeight - 70);
    _hpLabel->setColor(Color3B::GREEN);
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
        _speedLabel->setColor(Color3B::YELLOW);
        panelBg->addChild(_speedLabel);

        _resourceLabel = Label::createWithTTF(
            StringUtils::format("Gold: %d", building->getCurrentStock()), 
            "fonts/Marker Felt.ttf", 24
        );  
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 190); // 调整位置在坐标下方
        _resourceLabel->setColor(Color3B::YELLOW);
        panelBg->addChild(_resourceLabel);

    }
    else if (dynamic_cast<ElixirCollector*>(building)) {
        _speedLabel = Label::createWithTTF(
            StringUtils::format("generateSpeed: %.1f/s", building->getSpeed()),
            "fonts/Marker Felt.ttf", 24
        );
        _speedLabel->setPosition(bgWidth / 2, bgHeight - 150);
        _speedLabel->setColor(Color3B::BLUE);
        panelBg->addChild(_speedLabel);

        _resourceLabel = Label::createWithTTF(
            StringUtils::format("Elixir: %d", building->getCurrentStock()),
            "fonts/Marker Felt.ttf", 24
        );
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 190); // 调整位置在坐标下方
        _resourceLabel->setColor(Color3B::BLUE);
        panelBg->addChild(_resourceLabel);
    }
    else if (dynamic_cast<GoldStorage*>(building)) {
        _resourceLabel = Label::createWithTTF(
            StringUtils::format("GoldVolum: %d", building->getMaxStock()),
            "fonts/Marker Felt.ttf", 24
        );
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 150); // 调整位置在坐标下方
        _resourceLabel->setColor(Color3B::YELLOW);
        panelBg->addChild(_resourceLabel);
    }
    else if (dynamic_cast<ElixirStorage*>(building)) {
        _resourceLabel = Label::createWithTTF(
            StringUtils::format("ElixirVolum: %d", building->getMaxStock()),
            "fonts/Marker Felt.ttf", 24
        );
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 150); // 调整位置在坐标下方
        _resourceLabel->setColor(Color3B::BLUE);
        panelBg->addChild(_resourceLabel);
    }
    else if (dynamic_cast<ArmyCamp*>(building)) {
        _resourceLabel = Label::createWithTTF(
            StringUtils::format("armyNum/maxNum: %d/%d", building->getCurrentStock(), building->getMaxStock()),
            "fonts/Marker Felt.ttf", 24
        );
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 150); // 调整位置在坐标下方
        _resourceLabel->setColor(Color3B::BLACK);
        panelBg->addChild(_resourceLabel);

        _barbarian = Label::createWithTTF(
            StringUtils::format("barbarianNum: %d", building->getArmy(0)),
            "fonts/Marker Felt.ttf", 24
        );
        _barbarian->setPosition(bgWidth / 2, bgHeight - 190); // 调整位置在坐标下方
        _barbarian->setColor(Color3B::BLACK);
        panelBg->addChild(_barbarian);

        _archer = Label::createWithTTF(
            StringUtils::format("archerNum: %d", building->getArmy(1)),
            "fonts/Marker Felt.ttf", 24
        );
        _archer->setPosition(bgWidth / 2, bgHeight - 230); // 调整位置在坐标下方
        _archer->setColor(Color3B::BLACK);
        panelBg->addChild(_archer);

        _giant = Label::createWithTTF(
            StringUtils::format("giantNum: %d", building->getArmy(2)),
            "fonts/Marker Felt.ttf", 24
        );
        _giant->setPosition(bgWidth / 2, bgHeight - 270); // 调整位置在坐标下方
        _giant->setColor(Color3B::BLACK);
        panelBg->addChild(_giant);

        _goblin = Label::createWithTTF(
            StringUtils::format("goblinNum: %d", building->getArmy(3)),
            "fonts/Marker Felt.ttf", 24
        );
        _goblin->setPosition(bgWidth / 2, bgHeight - 310); // 调整位置在坐标下方
        _goblin->setColor(Color3B::BLACK);
        panelBg->addChild(_goblin);

        _bomber = Label::createWithTTF(
            StringUtils::format("bomberNum: %d", building->getArmy(4)),
            "fonts/Marker Felt.ttf", 24
        );
        _bomber->setPosition(bgWidth / 2, bgHeight - 350); // 调整位置在坐标下方
        _bomber->setColor(Color3B::BLACK);
        panelBg->addChild(_bomber);

        _balloon = Label::createWithTTF(
            StringUtils::format("balloonNum: %d", building->getArmy(5)),
            "fonts/Marker Felt.ttf", 24
        );
        _balloon->setPosition(bgWidth / 2, bgHeight - 390); // 调整位置在坐标下方
        _balloon->setColor(Color3B::BLACK);
        panelBg->addChild(_balloon);
    }
    else if (dynamic_cast<Walls*>(building)) {}
    else if (dynamic_cast<TownHall*>(building)) {
        _resourceLabel = Label::createWithTTF(
            StringUtils::format("maxGoldNum: %d\nmaxElixirNum: %d", building->getMaxGoldNum(), building->getMaxElixirNum()),
            "fonts/Marker Felt.ttf", 24
        );
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 150); // 调整位置在坐标下方
        _resourceLabel->setColor(Color3B::BLACK);
        panelBg->addChild(_resourceLabel);
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
        upGradeLabel->setColor(Color3B::YELLOW);
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
    auto collectLabel = Label::createWithSystemFont("Collect", "fonts/Marker Felt.ttf", 24);
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

//需要更新的文字
void BuildingInfoPanel::updateInfo(Building* building, cocos2d::Sprite* background_sprite_) {
    if (!building) return;
    //私有标签
    std::string type;
    if (dynamic_cast<GoldMine*>(building)) {
        type = "GoldMine";
        _speedLabel->setString(StringUtils::format("generateSpeed: %.1f/s", building->getSpeed()));
        _resourceLabel->setString(StringUtils::format("Gold: %d", building->getCurrentStock()));
    }
    else if (dynamic_cast<ElixirCollector*>(building)) {
        type = "ElixirCollector";
        _speedLabel->setString(StringUtils::format("generateSpeed: %.1f/s", building->getSpeed()));
        _resourceLabel->setString(StringUtils::format("Elixir: %d", building->getCurrentStock()));
    }
    else if (dynamic_cast<GoldStorage*>(building)) {
        type = "GoldStorage";
        _resourceLabel->setString(StringUtils::format("GoldVolum: %d", building->getMaxStock()));
    }
    else if (dynamic_cast<ElixirStorage*>(building)) {
        type = "ElixirStorage";
        _resourceLabel->setString(StringUtils::format("ElixirVolum: %d", building->getMaxStock()));
    }
    else if (dynamic_cast<ArmyCamp*>(building)) {
        type = "ArmyCamp";
        _resourceLabel->setString(StringUtils::format("armyNum/maxNum: %d/%d", building->getCurrentStock(), building->getMaxStock()));
        _barbarian->setString(StringUtils::format("barbarianNum: %d", building->getArmy(0)));
        _archer->setString(StringUtils::format("archerNum: %d", building->getArmy(1)));
        _giant->setString(StringUtils::format("giantNum: %d", building->getArmy(2)));
        _goblin->setString(StringUtils::format("goblinNum: %d", building->getArmy(3)));
        _bomber->setString(StringUtils::format("bomberNum: %d", building->getArmy(4)));
        _balloon->setString(StringUtils::format("balloonNum: %d", building->getArmy(5)));
    }
    else if (dynamic_cast<Walls*>(building)) {
        type = "Walls";
    }
    else if (dynamic_cast<TownHall*>(building)) {
        type = "Townhall";
        _resourceLabel->setString(StringUtils::format("maxGoldNum: %d\nmaxElixirNum: %d", building->getMaxGoldNum(), building->getMaxElixirNum()));
    }
    //公共标签
    _titleLabel->setString(StringUtils::format("%s Lv.%d", type.c_str(), building->getLv()));
    _hpLabel->setString(StringUtils::format("HP: %d", building->getHp()));
    _positionLabel->setString(StringUtils::format("(x,y):(%.1f,%.1f)", building->getXX(), building->getYY()));
    
}

void BuildingInfoPanel::onUpgradeClicked(Ref* sender) {
    if (!_targetBuilding) return;

    //非大本营
    if (_targetBuilding->getLv() < maxLevel&&!dynamic_cast<TownHall*>(_targetBuilding)) {
        // 升级所需资源
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

        _targetBuilding->update();//建筑物升级
        _targetBuilding->playSuccessBlink();// 播放升级成功动画

    }
    //大本营
    else if (dynamic_cast<TownHall*>(_targetBuilding) && _targetBuilding->getLv() < 15) {
        // 升级所需资源
        int requiredGold = 100;//100 * _targetBuilding->getLv();    // 每级所需金币为当前等级*100
        int requiredElixir = 100;//50 * _targetBuilding->getLv();   // 每级所需圣水为当前等级*50

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

        _targetBuilding->update();//建筑物升级
        
        //更新全局变量
        maxGoldVolum = _targetBuilding->getMaxGoldNum();
        maxElixirVolum = _targetBuilding->getMaxElixirNum();
        maxLevel = _targetBuilding->getLv();

        _targetBuilding->playSuccessBlink();// 播放升级成功动画
    }
    // 更新信息面板显示
    updateInfo(_targetBuilding, temp);
}

void BuildingInfoPanel::onCollectClicked(Ref* sender) {
    if (!_targetBuilding) return;

    int collected = 0;
    // 判断建筑类型并执行对应收集逻辑
    if (auto goldMine = dynamic_cast<GoldMine*>(_targetBuilding)) {
        collected = goldMine->getCurrentStock();
        if (collected > 0&&collected+ g_goldCount<=maxGoldVolum) {
            g_goldCount += collected;
        }
        else if (collected > 0 && collected + g_goldCount > maxGoldVolum) {
            g_goldCount = maxGoldVolum;
        }
    }
    else if (auto elixirCollector = dynamic_cast<ElixirCollector*>(_targetBuilding)) {
        collected = elixirCollector->getCurrentStock();
        if (collected > 0 && collected + g_goldCount <= maxElixirVolum) {
            g_elixirCount += collected;
        }
        else if (collected > 0 && collected + g_goldCount > maxElixirVolum) {
            g_elixirCount = maxElixirVolum;
        }
    }
    // 更新面板显示的资源数量
    if (collected > 0) {
        updateInfo(_targetBuilding,temp); // 刷新信息显示
    }
}
