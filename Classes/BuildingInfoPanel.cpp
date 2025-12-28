#include "BuildingInfoPanel.h"
#include "AttackerData.h"
#include "SecondScene.h" // 用于访问全局变量 g_goldCount 等
#include "WebSocketManager.h"
#include "SessionManager.h"

using namespace cocos2d;

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

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
        armybg->setPosition(Vec2(-bgWidth *0.55 , -bgHeight *0.22));
        armyExtraPanel->addChild(armybg);

        // 添加士兵图像按钮115125
        barbarianBtn = MenuItemImage::create(
            "Barbarian.png",
            "Barbarian.png",
            [this](cocos2d::Ref* sender) {
                if (auto armyCamp = dynamic_cast<ArmyCamp*>(_targetBuilding)) {
                    // 检查是否还有容量
                    if (armyCamp->getCurrentStock()+ armyCamp->getArmySize(0) <= armyCamp->getMaxStock()) {
                        this->playBlinkAnimation(barbarianBtn);
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
                        this->playBlinkAnimation(archerBtn);
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
                        this->playBlinkAnimation(giantBtn);
                        armyCamp->updateNum(2); 
                        this->updateInfo(_targetBuilding, temp); // 更新显示
                    }
                }
            }
        );
        giantBtn->setScale(0.3f);
        giantBtn->setPosition(400, archerBtn->getContentSize().height * 0.32); // 可根据需要调整位置

        goblinBtn = MenuItemImage::create(
            "Goblin.png",
            "Goblin.png",
            [this](cocos2d::Ref* sender) {
                if (auto armyCamp = dynamic_cast<ArmyCamp*>(_targetBuilding)) {
                    // 检查是否还有容量
                    if (armyCamp->getCurrentStock() + armyCamp->getArmySize(3) <= armyCamp->getMaxStock()) {
                        this->playBlinkAnimation(goblinBtn);
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
                        this->playBlinkAnimation(bomberBtn);
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
                        this->playBlinkAnimation(balloonBtn);
                        armyCamp->updateNum(5);
                        this->updateInfo(_targetBuilding, temp); // 更新显示
                    }
                }
            }
        );
        balloonBtn->setScale(0.3f);
        balloonBtn->setPosition(880, archerBtn->getContentSize().height * 0.35); // 可根据需要调整位置

        // 添加详情按钮
        soldierInfoBtn = MenuItemImage::create(
            "0.png",
            "0.png",
            [this](Ref* sender) {
                showSoldierInfo(_targetBuilding->getLv());
            }
        );
        soldierInfoBtn->setScale(0.5f);
        soldierInfoBtn->setPosition(bgWidth / 2, barbarianBtn->getPositionY() - 150);

        auto soldierInfoLabel = Label::createWithSystemFont("Soldiers' Information", "fonts/Marker Felt.ttf", 60);
        soldierInfoLabel->setColor(Color3B::MAGENTA);
        soldierInfoLabel->setPosition(Vec2(soldierInfoBtn->getContentSize().width / 2, soldierInfoBtn->getContentSize().height / 2));
        soldierInfoBtn->addChild(soldierInfoLabel);

        // 创建菜单
        menu = cocos2d::Menu::create(barbarianBtn, archerBtn, giantBtn, goblinBtn, bomberBtn, balloonBtn, soldierInfoBtn, nullptr);
        menu->setPosition(0, 0); // 菜单位置相对于父节点（armyExtraPanel）
        armybg->addChild(menu);
    }

    // 1. 面板主体
    auto panelBg = Sprite::create("3.png"); // 面板背景图
    if (!panelBg) {
        return false;
    }
    panelBg->setPosition(building->getContentSize().width + panelBg->getContentSize().width * 0.75, building->getContentSize().height - panelBg->getContentSize().height / 3);
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
    else {
        type = "BuilderHut";
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

    // 4. 资源信息显示
    // 判断建筑类型并显示对应资源，金矿和圣水收集器显示的是当前存贮的资源和生产速度，有收集按钮
    // 存钱罐和圣水瓶显示的是容量/上限，有收集按钮
    //兵营显示各个士兵的信息，城墙、建筑小屋不需要显示
    if (dynamic_cast<GoldMine*>(building)) {
        _speedLabel = Label::createWithTTF(
            StringUtils::format("generateSpeed: %d/s", building->getSpeed()),
            "fonts/Marker Felt.ttf", 24
        );
        _speedLabel->setPosition(bgWidth / 2, bgHeight - 150);
        _speedLabel->setColor(Color3B::YELLOW);
        panelBg->addChild(_speedLabel);

        _resourceLabel = Label::createWithTTF(
            StringUtils::format("Gold: %d\nVolum: %d", building->getCurrentStock(), building->getMaxStock()),
            "fonts/Marker Felt.ttf", 24
        );  
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 210); // 调整位置在坐标下方
        _resourceLabel->setColor(Color3B::YELLOW);
        panelBg->addChild(_resourceLabel);

    }
    else if (dynamic_cast<ElixirCollector*>(building)) {
        _speedLabel = Label::createWithTTF(
            StringUtils::format("generateSpeed: %d/s", building->getSpeed()),
            "fonts/Marker Felt.ttf", 24
        );
        _speedLabel->setPosition(bgWidth / 2, bgHeight - 150);
        _speedLabel->setColor(Color3B::BLUE);
        panelBg->addChild(_speedLabel);

        _resourceLabel = Label::createWithTTF(
            StringUtils::format("Elixir: %d\nVolum: %d", building->getCurrentStock(), building->getMaxStock()),
            "fonts/Marker Felt.ttf", 24
        );
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 210); // 调整位置在坐标下方
        _resourceLabel->setColor(Color3B::BLUE);
        panelBg->addChild(_resourceLabel);
    }
    else if (dynamic_cast<GoldStorage*>(building)) {
        _resourceLabel = Label::createWithTTF(
            StringUtils::format("Gold: %d\nVolum: %d", building->getCurrentStock(), building->getMaxStock()),
            "fonts/Marker Felt.ttf", 24
        );
        _resourceLabel->setPosition(bgWidth / 2, bgHeight - 150); // 调整位置在坐标下方
        _resourceLabel->setColor(Color3B::YELLOW);
        panelBg->addChild(_resourceLabel);
    }
    else if (dynamic_cast<ElixirStorage*>(building)) {
        _resourceLabel = Label::createWithTTF(
            StringUtils::format("Elixir: %d\nVolum: %d", building->getCurrentStock(), building->getMaxStock()),
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
    else if(dynamic_cast<BuilderHut*>(building)){}
    
    // 5. 升级按钮,最高等级15
    _upgradeBtn = MenuItemImage::create(
        "5.png",  // 正常状态图
        "5.png", // 按下状态图
        CC_CALLBACK_1(BuildingInfoPanel::onUpgradeClicked, this)
     );
    //设置文字提示
    if (!_targetBuilding->getIsUpgrade()) {
        if (_targetBuilding->getLv() < 15) {
            auto upGradeLabel = Label::createWithSystemFont("UpGrade", "fonts/Marker Felt.ttf", 24);
            upGradeLabel->setColor(Color3B::MAGENTA);
            upGradeLabel->setPosition(Vec2(_upgradeBtn->getContentSize().width / 2, _upgradeBtn->getContentSize().height / 2));
            _upgradeBtn->addChild(upGradeLabel);
        }
        else {
            auto upGradeLabel = Label::createWithSystemFont("MaxGrade", "fonts/Marker Felt.ttf", 24);
            upGradeLabel->setColor(Color3B::RED);
            upGradeLabel->setPosition(Vec2(_upgradeBtn->getContentSize().width / 2, _upgradeBtn->getContentSize().height / 2));
            _upgradeBtn->addChild(upGradeLabel);
        }
    }
    else {
        auto upGradeLabel = Label::createWithSystemFont(StringUtils::format("please wait %d s!", _targetBuilding->getRemainTime()),"fonts/Marker Felt.ttf", 24);
        upGradeLabel->setColor(Color3B::RED);
        upGradeLabel->setPosition(Vec2(_upgradeBtn->getContentSize().width / 2, _upgradeBtn->getContentSize().height / 2));
        _upgradeBtn->addChild(upGradeLabel);
    }
    //缩放和位置
    _upgradeBtn->setScale(0.75f);
    _upgradeBtn->setPosition(bgWidth / 2, 70);
    
    //6. 加速按钮
    if (_targetBuilding->getIsUpgrade()) {
        _speedUpBtn = MenuItemImage::create(
            "5.png",  // 正常状态图
            "5.png", // 按下状态图
            CC_CALLBACK_1(BuildingInfoPanel::speedUpgradeClicked, this)
        );
        auto speedUpLabel = Label::createWithSystemFont("SpeedUp", "fonts/Marker Felt.ttf", 24);
        speedUpLabel->setColor(Color3B::MAGENTA);
        speedUpLabel->setPosition(Vec2(_upgradeBtn->getContentSize().width / 2, _upgradeBtn->getContentSize().height / 2));
        _speedUpBtn->addChild(speedUpLabel);
        //缩放和位置
        _speedUpBtn->setScale(0.75f);
        _speedUpBtn->setPosition(bgWidth / 2, 170); // 位置在升级按钮上方
    }

    // 7. 收集按钮
    _collectBtn = MenuItemImage::create(
        "5.png",  // 正常状态图
        "5.png", // 按下状态图
        CC_CALLBACK_1(BuildingInfoPanel::onCollectClicked, this)
    );
    //设置文字提示
    auto collectLabel = Label::createWithSystemFont("Collect", "fonts/Marker Felt.ttf", 24);
    collectLabel->setColor(Color3B::YELLOW);
    collectLabel->setPosition(Vec2(_collectBtn->getContentSize().width / 2,
        _collectBtn->getContentSize().height / 2));
    _collectBtn->addChild(collectLabel);
    //缩放和位置
    _collectBtn->setScale(0.75f);
    _collectBtn->setPosition(bgWidth / 2, 120); // 位置在升级按钮上方

    // 菜单容器
    if (dynamic_cast<GoldMine*>(building)|| dynamic_cast<ElixirCollector*>(building)||
        dynamic_cast<GoldStorage*>(building) || dynamic_cast<ElixirStorage*>(building)) {
        auto menu = Menu::create(_collectBtn, _upgradeBtn, _speedUpBtn, nullptr);
        menu->setPosition(Vec2::ZERO);
        panelBg->addChild(menu);
    }
    else {
        auto menu = Menu::create(_upgradeBtn, _speedUpBtn, nullptr);
        menu->setPosition(Vec2::ZERO);
        panelBg->addChild(menu);
    }
    return true;
}

//更新文字
void BuildingInfoPanel::updateInfo(Building* building, cocos2d::Sprite* background_sprite_) {
    if (!building) return;
    //私有标签
    std::string type;
    if (dynamic_cast<GoldMine*>(building)) {
        type = "GoldMine";
        _speedLabel->setString(StringUtils::format("generateSpeed: %d/s", building->getSpeed()));
        _resourceLabel->setString(StringUtils::format("Gold: %d\nVolum: %d", building->getCurrentStock(), building->getMaxStock()));
    }
    else if (dynamic_cast<ElixirCollector*>(building)) {
        type = "ElixirCollector";
        _speedLabel->setString(StringUtils::format("generateSpeed: %d/s", building->getSpeed()));
        _resourceLabel->setString(StringUtils::format("Elixir: %d\nVolum: %d", building->getCurrentStock(), building->getMaxStock()));
    }
    else if (dynamic_cast<GoldStorage*>(building)) {
        type = "GoldStorage";
        _resourceLabel->setString(StringUtils::format("Gold: %d\nVolum: %d", building->getCurrentStock(), building->getMaxStock()));
    }
    else if (dynamic_cast<ElixirStorage*>(building)) {
        type = "ElixirStorage";
        _resourceLabel->setString(StringUtils::format("Elixir: %d\nVolum: %d", building->getCurrentStock(), building->getMaxStock()));
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
    else if (dynamic_cast<BuilderHut*>(building)) {
        type = "BuilderHut";
    }
    //公共标签
    _titleLabel->setString(StringUtils::format("%s Lv.%d", type.c_str(), building->getLv()));
    _hpLabel->setString(StringUtils::format("HP: %d", building->getHp()));
<<<<<<< HEAD
    _positionLabel->setString(StringUtils::format("(x,y):(%.1f,%.1f)", building->getX(), building->getY()));
  
    refreshUpgradeButton(building);

    startUpgradeCountdownScheduler();

    return;
}

void BuildingInfoPanel::refreshUpgradeButton(Building* building) {
    if (!building || !_upgradeBtn) return;

    _upgradeBtn->removeAllChildren();

    if (!building->getIsUpgrade()) {
        if (building->getLv() < 15) {
            auto upGradeLabel = Label::createWithSystemFont("UpGrade", "fonts/Marker Felt.ttf", 24);
            upGradeLabel->setColor(Color3B::MAGENTA);
            upGradeLabel->setPosition(Vec2(_upgradeBtn->getContentSize().width / 2, _upgradeBtn->getContentSize().height / 2));
            _upgradeBtn->addChild(upGradeLabel);
        }
        else {
            auto upGradeLabel = Label::createWithSystemFont("MaxGrade", "fonts/Marker Felt.ttf", 24);
            upGradeLabel->setColor(Color3B::RED);
            upGradeLabel->setPosition(Vec2(_upgradeBtn->getContentSize().width / 2, _upgradeBtn->getContentSize().height / 2));
            _upgradeBtn->addChild(upGradeLabel);
        }
    }
    else {
        int remainingTime = building->getRemainTime();
        auto upGradeLabel = Label::createWithSystemFont(StringUtils::format("please wait %d s!", remainingTime), "fonts/Marker Felt.ttf", 24);
        upGradeLabel->setColor(Color3B::RED);
        upGradeLabel->setPosition(Vec2(_upgradeBtn->getContentSize().width / 2, _upgradeBtn->getContentSize().height / 2));
        _upgradeBtn->addChild(upGradeLabel);
    }
}

void BuildingInfoPanel::updateUpgradeCountdown() {
    if (!_targetBuilding) return;

    if (_targetBuilding->getIsUpgrade()) {
        refreshUpgradeButton(_targetBuilding);
    }
}

void BuildingInfoPanel::startUpgradeCountdownScheduler() {
    Director::getInstance()->getScheduler()->schedule(
        CC_CALLBACK_0(BuildingInfoPanel::updateUpgradeCountdown, this),
        this,
        1.0f,
        false,
        "upgradeCountdown"
    );
}

void BuildingInfoPanel::stopUpgradeCountdownScheduler() {
    Director::getInstance()->getScheduler()->unschedule("upgradeCountdown", this);
=======
    
>>>>>>> 663d826ac427855dd4ac5e32c99aac542ee983a2
}

//升级
void BuildingInfoPanel::onUpgradeClicked(Ref* sender) {
    if (!_targetBuilding) return;

    int count = 0;
    for (auto building :SecondScene::placedBuildings) {
        if (building->getIsUpgrade()) {
            count++;
        }
    }
    //canUpgrade函数判断了资源是否足够，等级是否超上限，而count则是同时可以升级的建筑数量
    if (count < hutNum && _targetBuilding->canUpgrade()) {
        g_goldCount -= _targetBuilding->getUpgradeGoldCost();
        g_elixirCount-= _targetBuilding->getUpgradeElixirCost();

        _targetBuilding->playSuccessBlink();// 播放开始升级动画
        _targetBuilding->startUpgrade();

        // 发送升级开始事件到服务器
        WebSocketManager::getInstance()->sendUpgradeStart(
            _targetBuilding->getBuildingType(),
            _targetBuilding->getX(),
            _targetBuilding->getY(),
            _targetBuilding->getLv() + 1,
            _targetBuilding->getUpgradeDuration()
        );
    }
    updateInfo(_targetBuilding, temp);
}

//一键完成加速
void BuildingInfoPanel::speedUpgradeClicked(cocos2d::Ref* sender) {
    if (g_gemCount > 0&& _targetBuilding->getIsUpgrade()) {
        g_gemCount--;
        _targetBuilding->finishUpgrade();
    }
}

//收集
void BuildingInfoPanel::onCollectClicked(Ref* sender) {
    CCLOG("BuildingInfoPanel: onCollectClicked called");
    if (!_targetBuilding) {
        CCLOG("BuildingInfoPanel: _targetBuilding is null!");
        return;
    }

    // 检查建筑对象是否有效 (0xDDDDDDDD 表示已释放)
    auto checkPtr = reinterpret_cast<uintptr_t>(_targetBuilding);
    if (checkPtr == 0xDDDDDDDD || checkPtr == 0xCCCCCCCC || checkPtr < 0x10000) {
        CCLOG("BuildingInfoPanel: _targetBuilding is invalid pointer: %p", _targetBuilding);
        return;
    }

    // 检查标签是否有效
    if (!_resourceLabel || !_speedLabel) {
        CCLOG("BuildingInfoPanel: UI labels are null!");
        return;
    }

    int collected = 0;
    bool resourceUpdated = false;
    // 判断建筑类型并执行对应收集逻辑
    if (dynamic_cast<GoldMine*>(_targetBuilding)) {
        collected = _targetBuilding->getCurrentStock();
        CCLOG("BuildingInfoPanel: GoldMine current stock: %d, g_goldCount: %d, max: %d",
            collected, g_goldCount, maxGoldVolum);
        if (collected > 0 && collected + g_goldCount <= maxGoldVolum) {
            g_goldCount += collected;
            resourceUpdated = true;
            collected = 0;
            CCLOG("BuildingInfoPanel: Gold collected, new g_goldCount: %d", g_goldCount);
        }
        else if (collected > 0 && collected + g_goldCount > maxGoldVolum) {
            int spaceAvailable = maxGoldVolum - g_goldCount;
            if (spaceAvailable > 0) {
                g_goldCount = maxGoldVolum;
                collected -= spaceAvailable;
                resourceUpdated = true;
            }
        }
    }
    else if (dynamic_cast<ElixirCollector*>(_targetBuilding)) {
        collected = _targetBuilding->getCurrentStock();
        CCLOG("BuildingInfoPanel: ElixirCollector current stock: %d, g_elixirCount: %d, max: %d",
            collected, g_elixirCount, maxElixirVolum);
        if (collected > 0 && collected + g_elixirCount <= maxElixirVolum) {
            g_elixirCount += collected;
            resourceUpdated = true;
            collected = 0;
        }
        else if (collected > 0 && collected + g_elixirCount > maxElixirVolum) {
            int spaceAvailable = maxElixirVolum - g_elixirCount;
            if (spaceAvailable > 0) {
                g_elixirCount = maxElixirVolum;
                collected -= spaceAvailable;
                resourceUpdated = true;
            }
        }
    }
    else if (dynamic_cast<GoldStorage*>(_targetBuilding)) {
        collected = _targetBuilding->getCurrentStock();
        if (collected > 0 && collected + g_goldCount <= maxGoldVolum) {
            g_goldCount += collected;
            resourceUpdated = true;
            collected = 0;
        }
        else if (collected > 0 && collected + g_goldCount > maxGoldVolum) {
            int spaceAvailable = maxGoldVolum - g_goldCount;
            if (spaceAvailable > 0) {
                g_goldCount = maxGoldVolum;
                collected -= spaceAvailable;
                resourceUpdated = true;
            }
        }
    }
    else if (dynamic_cast<ElixirStorage*>(_targetBuilding)) {
        collected = _targetBuilding->getCurrentStock();
        if (collected > 0 && collected + g_elixirCount <= maxElixirVolum) {
            g_elixirCount += collected;
            resourceUpdated = true;
            collected = 0;
        }
        else if (collected > 0 && collected + g_elixirCount > maxElixirVolum) {
            int spaceAvailable = maxElixirVolum - g_elixirCount;
            if (spaceAvailable > 0) {
                g_elixirCount = maxElixirVolum;
                collected -= spaceAvailable;
                resourceUpdated = true;
            }
        }
    }

    CCLOG("BuildingInfoPanel: Before updating building stock, collected: %d", collected);
    // 发送收集生产请求到服务器（清空建筑存量之前发送，确保数据一致性）
    int originalStock = _targetBuilding->getCurrentStock();
    if (originalStock > 0) {
        auto director = cocos2d::Director::getInstance();
        auto scene = director->getRunningScene();
        if (scene) {
            auto secondScene = dynamic_cast<SecondScene*>(scene);
            if (secondScene) {
                int resourceType = 0;
                if (dynamic_cast<GoldMine*>(_targetBuilding) || dynamic_cast<GoldStorage*>(_targetBuilding)) {
                    resourceType = 1; // 金币
                }
                else if (dynamic_cast<ElixirCollector*>(_targetBuilding) || dynamic_cast<ElixirStorage*>(_targetBuilding)) {
                    resourceType = 2; // 圣水
                }
                if (resourceType > 0) {
                    // 计算收集后剩余的存储量
                    int remainingStock = originalStock - collected;
                    if (remainingStock < 0) remainingStock = 0;
                    secondScene->sendCollectProductionRequest(_targetBuilding, collected, 
                        remainingStock, resourceType);
                }
            }
        }
    }

    // 更新面板显示的资源数量
    _targetBuilding->clearCurrentStock();
    if (collected > 0) {
        _targetBuilding->updateCurrentStock(collected);
    }
    CCLOG("BuildingInfoPanel: Before updateInfo");
    updateInfo(_targetBuilding, temp); // 刷新信息显示
    CCLOG("BuildingInfoPanel: After updateInfo, resourceUpdated: %d", resourceUpdated);

    // 发送资源更新请求到服务器
    if (resourceUpdated) {
        CCLOG("BuildingInfoPanel: Resource collected, sending update...");
        auto director = cocos2d::Director::getInstance();
        auto scene = director->getRunningScene();
        if (!scene) {
            CCLOG("BuildingInfoPanel: Running scene is null!");
            return;
        }
        CCLOG("BuildingInfoPanel: Scene class: %s", typeid(*scene).name());
        auto secondScene = dynamic_cast<SecondScene*>(scene);
        if (secondScene) {
            // 立即更新 UI 标签
            secondScene->updateResourceLabels();
            secondScene->sendUpdateResourceRequest(0.0f);
        }
        else {
            CCLOG("BuildingInfoPanel: Failed to cast scene to SecondScene!");
        }
    }
    CCLOG("BuildingInfoPanel: onCollectClicked finished");
}

//展示士兵信息
void BuildingInfoPanel::showSoldierInfo(int lv){
   
    soldierNode = Node::create();
    soldierNode->setPosition(Vec2(soldierInfoBtn->getContentSize().width / 2, 0));
    soldierNode->setVisible(false);
    soldierInfoBtn->addChild(soldierNode);

    auto soldierData = Sprite::create("2.png"); // 面板背景图
    if (!soldierData) {
        problemLoading("'2.png'");
    }
    soldierData->setScale(2.0f);
    soldierData->setPosition(Vec2(0, -400));
    soldierNode->addChild(soldierData);
    
    //信息按钮点击事件
    soldierInfoBtn->setCallback([this](Ref* sender) {
        // 切换显示状态（显示→隐藏，隐藏→显示）
        bool isVisible = soldierNode->isVisible();
        soldierNode->setVisible(!isVisible);
        });

    AttackerData Data[6] = {
        AttackerData::CreateBarbarianData(lv >= 12 ? 12 : lv),
        AttackerData::CreateArcherData(lv >= 12 ? 12 : lv),
        AttackerData::CreateGiantData(lv >= 12 ? 12 : lv),
        AttackerData::CreateGoblinData(lv >= 12 ? 12 : lv),
        AttackerData::CreateBomberData(lv >= 12 ? 12 : lv),
        AttackerData::CreateBalloonData(lv >= 12 ? 12 : lv) };

    int L = 160, H = 300;
    for (int i = 0; i < 6; i++) {
        auto nameLabel=Label::createWithTTF(
            StringUtils::format("%s", Data[i].id.c_str()),
            "fonts/Marker Felt.ttf", 36
        );
        nameLabel->setPosition(L * (i + 1) - 60, H);
        nameLabel->setColor(Color3B::BLACK);
        soldierData->addChild(nameLabel);

        auto lvLabel = Label::createWithTTF(
            StringUtils::format("Lv :%d", Data[i].level),
            "fonts/Marker Felt.ttf", 24
        );
        lvLabel->setPosition(L * (i + 1) - 60, H-30);
        lvLabel->setColor(Color3B::BLACK);
        soldierData->addChild(lvLabel);

        auto hpLabel=Label::createWithTTF(
            StringUtils::format("Hp: %.0f", Data[i].health),
            "fonts/Marker Felt.ttf", 24
        );
        hpLabel->setPosition(L * (i + 1) - 60, H-30*4);
        hpLabel->setColor(Color3B::GREEN);
        soldierData->addChild(hpLabel);

        auto atkLabel = Label::createWithTTF(
            StringUtils::format("Atk: %.0f", Data[i].damage),
            "fonts/Marker Felt.ttf", 24
        );
        atkLabel->setPosition(L * (i + 1) - 60, H - 30 * 5);
        atkLabel->setColor(Color3B::RED);
        soldierData->addChild(atkLabel);

        auto spaceLabel = Label::createWithTTF(
            StringUtils::format("Space: %d", Data[i].housing_space),
            "fonts/Marker Felt.ttf", 24
        );
        spaceLabel->setPosition(L * (i + 1) - 60, H - 30 * 2);
        spaceLabel->setColor(Color3B::YELLOW);
        soldierData->addChild(spaceLabel);

        auto speedLabel = Label::createWithTTF(
            StringUtils::format("Speed: %.0f", Data[i].move_speed),
            "fonts/Marker Felt.ttf", 24
        );
        speedLabel->setPosition(L * (i + 1) - 60, H - 30 * 3);
        speedLabel->setColor(Color3B::BLUE);
        soldierData->addChild(speedLabel);

        auto atkDistanceLabel = Label::createWithTTF(
            StringUtils::format("AtkDistance: %.0f", Data[i].attack_distance),
            "fonts/Marker Felt.ttf", 24
        );
        atkDistanceLabel->setPosition(L * (i + 1) - 60, H - 30 * 6);
        atkDistanceLabel->setColor(Color3B::ORANGE);
        soldierData->addChild(atkDistanceLabel);

        auto atkIntervalLabel = Label::createWithTTF(
            StringUtils::format("AtkInterval: %.0fs", Data[i].attack_interval),
            "fonts/Marker Felt.ttf", 24
        );
        atkIntervalLabel->setPosition(L * (i + 1) - 60, H - 30 * 7);
        atkIntervalLabel->setColor(Color3B::ORANGE);
        soldierData->addChild(atkIntervalLabel);

        if (i == 4) {
            auto damageHpLabel= Label::createWithTTF(
                StringUtils::format("DamageHp: %d", Data[i].bomber_data->death_damage),
                "fonts/Marker Felt.ttf", 24
            );
            damageHpLabel->setPosition(L * (i + 1) - 60, H - 30 * 8);
            damageHpLabel->setColor(Color3B::ORANGE);
            soldierData->addChild(damageHpLabel);
        }
    }

}