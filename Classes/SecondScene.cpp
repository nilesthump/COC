#if 1
#include "HelloWorldScene.h"
#include "SecondScene.h"
#include "ConvertTest.h"
#include "SessionManager.h"
#include "cocos2d.h"
#include <cmath>
#include <ctime>
#include <algorithm>
#include <functional>

// 初始化全局变量
int maxLevel, maxGoldVolum = 1000, maxElixirVolum = 1000;
int g_elixirCount = 750, g_goldCount = 750;
int g_gemCount = 15, hutNum = 2;

std::vector<Building*> SecondScene::placedBuildings;

USING_NS_CC;

Scene* SecondScene::createScene()
{
    return SecondScene::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool SecondScene::init()
{

    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto session = SessionManager::getInstance();
    if (session->isAccountLogin() && session->hasResourceData()) {
        int gold, elixir, gems;
        session->getResourceData(gold, elixir, gems);
        g_goldCount = gold;
        g_elixirCount = elixir;
        g_gemCount = gems;
        CCLOG("Loaded resources from server: gold=%d, elixir=%d, gems=%d",
            g_goldCount, g_elixirCount, g_gemCount);
    }
    else {
        // 初始化产金相关变量
        g_goldCount = 750; // 确保金币计数初始化为0
        g_elixirCount = 750;
        g_gemCount = 15;
    }

    _sceneIsDestroyed = false;
    setupWebSocketCallbacks();

    auto session_manager = SessionManager::getInstance();
    std::string username = session_manager->getCurrentUsername();

    // 初始化拖拽状态
    isDragging = false;
    draggingItem = nullptr;

    // 初始化建筑移动相关变量
    isMovingBuilding = false;
    movingBuilding = nullptr;
    _buildingsInitialized = false; // 仅在账号登录模式下重置，游客模式在initDefaultBuildingsAndSave中设置

    //游戏背景
    auto label = Label::createWithTTF("Your Clan!!!", "fonts/Marker Felt.ttf", 36);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        label->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height - label->getContentSize().height));
        this->addChild(label, 1);
    }
    background_sprite_ = Sprite::create("normal(winter).jpg");
    if (background_sprite_ == nullptr)
    {
        problemLoading("'normal(winter).jpg'");
    }
    else
    {
        // position the sprite on the center of the screen
        background_sprite_->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        float scale = visibleSize.width / background_sprite_->getContentSize().width * 1.5f;
        background_sprite_->setScale(scale);
        // add the sprite as a child to this layer
        this->addChild(background_sprite_, 0);
    }

    if (!session_manager->isAccountLogin() && username.empty()) {
        CCLOG("SecondScene: Guest mode, initializing default buildings directly");
        initDefaultBuildingsAndSave();
    }
    // 默认建筑初始化在 onWebSocketBuildingsMessage 中处理，通过服务器返回的建筑列表判断

    // 总按钮部分
    auto backItem = MenuItemImage::create("5.png", "5.2.png",
        CC_CALLBACK_1(SecondScene::menuFirstCallback, this));
    if (backItem == nullptr ||
        backItem->getContentSize().width <= 0 ||
        backItem->getContentSize().height <= 0)
    {
        problemLoading("'5.png' and '5.2.png'");
    }
    else
    {
        double x = origin.x + backItem->getContentSize().width / 2;
        double y = origin.y + visibleSize.height - backItem->getContentSize().height / 2;
        backItem->setPosition(Vec2(x, y));

        auto backLabel = Label::createWithSystemFont("BACK", "fonts/Marker Felt.ttf", 30);
        backLabel->setColor(Color3B::WHITE);
        backLabel->setPosition(Vec2(backItem->getContentSize().width / 2, backItem->getContentSize().height / 2));
        backItem->addChild(backLabel);
    }

    auto buildItem = MenuItemImage::create("5.png", "5.2.png",
        CC_CALLBACK_1(SecondScene::menuBuildCallback, this));
    if (buildItem == nullptr ||
        buildItem->getContentSize().width <= 0 ||
        buildItem->getContentSize().height <= 0)

    {
        problemLoading("'5.png' and '5.2.png'");
    }
    else
    {
        double x = origin.x + buildItem->getContentSize().width / 2;
        double y = origin.y + visibleSize.height - buildItem->getContentSize().height*1.5;
        buildItem->setPosition(Vec2(x, y));

        auto buildLabel = Label::createWithSystemFont("BUILD", "fonts/Marker Felt.ttf", 30);
        buildLabel->setColor(Color3B::WHITE);
        buildLabel->setPosition(Vec2(buildItem->getContentSize().width / 2, buildItem->getContentSize().height / 2));
        buildItem->addChild(buildLabel);
    }

    auto attackItem= MenuItemImage::create("5.png", "5.2.png",
        CC_CALLBACK_1(SecondScene::menuAttackCallback, this));
    if (attackItem == nullptr ||
        attackItem->getContentSize().width <= 0 ||
        attackItem->getContentSize().height <= 0)

    {
        problemLoading("'5.png' and '5.2.png'");
    }
    else
    {
        double x = origin.x + visibleSize.width - attackItem->getContentSize().width / 2;
        double y = origin.y + attackItem->getContentSize().height / 2;
        attackItem->setPosition(Vec2(x, y));

        auto attackLabel = Label::createWithSystemFont("ATTACK", "fonts/Marker Felt.ttf", 30);
        attackLabel->setColor(Color3B::WHITE);
        attackLabel->setPosition(Vec2(attackItem->getContentSize().width / 2, attackItem->getContentSize().height / 2));
        attackItem->addChild(attackLabel);
    }

    auto menu = Menu::create(backItem, buildItem, attackItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    //进攻菜单
    attackPanel = Node::create();
    double attackPanelX = -attackItem->getContentSize().width / 2;
    double attackPanelY = attackItem->getContentSize().height / 2;
    attackPanel->setPosition(Vec2(attackPanelX, attackPanelY));
    attackPanel->setVisible(false);
    attackItem->addChild(attackPanel, 1);

    auto attackBg= Sprite::create("1.png");
    if (attackBg == nullptr) {
        problemLoading("'1.png'");
    }
    else {
        double panelBgX = attackItem->getContentSize().width- attackBg->getContentSize().width;
        double panelBgY = attackItem->getContentSize().height *2;
        attackBg->setPosition(Vec2(panelBgX, panelBgY));
        attackPanel->addChild(attackBg);
    }

    boss1Btn= MenuItemImage::create(
        "5.png","5.2.png",
        CC_CALLBACK_1(SecondScene::menuBoss1Callback, this));
    if (boss1Btn) {
        auto boss1Label = Label::createWithSystemFont("Boss-1", "fonts/Marker Felt.ttf", 30);
        boss1Label->setColor(Color3B::WHITE);
        boss1Label->setPosition(Vec2(boss1Btn->getContentSize().width / 2, boss1Btn->getContentSize().height / 2));
        boss1Btn->addChild(boss1Label);

        boss1Btn->setPosition(Vec2(attackBg->getContentSize().width / 2, attackBg->getContentSize().height - boss1Btn->getContentSize().height  / 2-20));
    }

    boss2Btn = MenuItemImage::create(
        "5.png", "5.2.png",
        CC_CALLBACK_1(SecondScene::menuBoss2Callback, this));
    if (boss2Btn) {
        auto boss2Label = Label::createWithSystemFont("Boss-2", "fonts/Marker Felt.ttf", 30);
        boss2Label->setColor(Color3B::WHITE);
        boss2Label->setPosition(Vec2(boss2Btn->getContentSize().width / 2, boss2Btn->getContentSize().height / 2));
        boss2Btn->addChild(boss2Label);

        boss2Btn->setPosition(Vec2(attackBg->getContentSize().width / 2, attackBg->getContentSize().height - boss2Btn->getContentSize().height *1.5 - 40));
    }

    auto attackMenu = Menu::create(boss1Btn, boss2Btn, nullptr);
    attackMenu->setPosition(Vec2::ZERO);
    attackBg->addChild(attackMenu);

    //建筑菜单
    buildPanel = Node::create();

    double buildPanelX = buildItem->getContentSize().width;
    double buildPanelY = buildItem->getContentSize().height;
    buildPanel->setPosition(Vec2(buildPanelX, buildPanelY));
    buildPanel->setVisible(false);
    buildItem->addChild(buildPanel, 1);

    auto panelBg = Sprite::create("3.1.png");
    if (panelBg == nullptr) {
        problemLoading("'3.1.png'");
    }
    else {
        double panelBgX = panelBg->getContentSize().width / 2;
        double panelBgY = buildItem->getContentSize().height - panelBg->getContentSize().height / 2;
        panelBg->setPosition(Vec2(panelBgX, panelBgY));
        buildPanel->addChild(panelBg);
    }
    // 1.创建金矿按钮
    goldMineBtn = MenuItemImage::create(
        "GoldMineLv1.png",
        "GoldMineLv1.png",
        [=](Ref* pSender) {
            // 先检查是否有足够资源
            GoldMine* tempMine = GoldMine::create("GoldMineLv1.png"); // 临时实例获取消耗
            int goldCost = tempMine->getGoldCost(), elixirCost = tempMine->getElixirCost();
            if (g_goldCount < goldCost||g_elixirCount< elixirCost) {
                return; // 直接返回，不允许放置
            }

            if (!isDragging) {
                log("goldMine ");
                isDragging = true;
                draggingItem = goldMineBtn;
                dragStartPosition = goldMineBtn->getPosition();
                
                auto goldMinePreview = GoldMine::create("GoldMineLv1.png"); // 预览用金矿纹理
                if (goldMinePreview) {
                    // 预览态设置：半透明（区分实际对象）
                    goldMinePreview->setOpacity(150);

                    Vec2 my = Vec2(goldMinePreview->getX(), goldMinePreview->getY());
                    if (background_sprite_) {
                        Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                        goldMinePreview->setMinePosition(you);
                        // 添加到背景精灵，并保存到按钮的UserData
                        background_sprite_->addChild(goldMinePreview, 10);
                        goldMineBtn->setUserData(goldMinePreview);
                    }
                }

                
            }
        }
    );
    if (goldMineBtn) {
        goldMineBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 / 2 ));
    }
    goldMineBtn->setScale(0.6f);

    // 2.创建圣水收集器按钮
    elixirCollectorBtn = MenuItemImage::create(
        "ElixirCollectorLv1.png",
        "ElixirCollectorLv1.png",
        [=](Ref* pSender) {
            // 先检查是否有足够资源
            ElixirCollector* tempMine = ElixirCollector::create("ElixirCollectorLv1.png"); // 临时实例获取消耗
            int goldCost = tempMine->getGoldCost(), elixirCost = tempMine->getElixirCost();
            if (g_goldCount < goldCost || g_elixirCount < elixirCost) {
                return; // 直接返回，不允许放置
            }

            if (!isDragging) {
                log("ElixirCollectorLv1 ");
                isDragging = true;
                draggingItem = elixirCollectorBtn;
                dragStartPosition = elixirCollectorBtn->getPosition();
                             
                auto elixirCollectorPreview = ElixirCollector::create("ElixirCollectorLv1.png"); // 预览用圣水收集器纹理
                if (elixirCollectorPreview) {
                    // 预览态设置：半透明（区分实际对象）
                    elixirCollectorPreview->setOpacity(150);
                    
                    Vec2 my = Vec2(elixirCollectorPreview->getX(), elixirCollectorPreview->getY());
                    if (background_sprite_) {
                        Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                        elixirCollectorPreview->setMinePosition(you);

                        // 添加到背景精灵，并保存到按钮的UserData
                        background_sprite_->addChild(elixirCollectorPreview, 10);
                        elixirCollectorBtn->setUserData(elixirCollectorPreview);
                    }
                }
            }
        }
    );
    if (elixirCollectorBtn) {
        elixirCollectorBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 * 1.5 ));
    }
    elixirCollectorBtn->setScale(0.6f);

    // 3.创建存钱罐按钮
    goldStorageBtn = MenuItemImage::create(
        "GoldStorageLv1.png",
        "GoldStorageLv1.png",
        [=](Ref* pSender) {
            // 先检查是否有足够资源
            GoldStorage* tempMine = GoldStorage::create("GoldStorageLv1.png"); // 临时实例获取消耗
            int goldCost = tempMine->getGoldCost(), elixirCost = tempMine->getElixirCost();
            if (g_goldCount < goldCost || g_elixirCount < elixirCost) {
                return; // 直接返回，不允许放置
            }

            if (!isDragging) {
                log("goldStorage ");
                isDragging = true;
                draggingItem = goldStorageBtn;
                dragStartPosition = goldStorageBtn->getPosition();

                auto goldStoragePreview = GoldStorage::create("GoldStorageLv1.png"); // 预览用存钱罐纹理
                if (goldStoragePreview) {
                    // 预览态设置：半透明（区分实际对象）
                    goldStoragePreview->setOpacity(150);                   
                    Vec2 my = Vec2(goldStoragePreview->getX(), goldStoragePreview->getY());
                    if (background_sprite_) {
                        Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                        goldStoragePreview->setMinePosition(you);

                        // 添加到背景精灵，并保存到按钮的UserData
                        background_sprite_->addChild(goldStoragePreview, 10);
                        goldStorageBtn->setUserData(goldStoragePreview);
                    }
                }
            }
        }
    );
    if (goldStorageBtn) {
        goldStorageBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 * 2.5 +8));
    }
    goldStorageBtn->setScale(0.6f);

    // 4.创建圣水瓶按钮
    elixirStorageBtn = MenuItemImage::create(
        "ElixirStorageLv1.png",
        "ElixirStorageLv1.png",
        [=](Ref* pSender) {
            // 先检查是否有足够资源
            ElixirStorage* tempMine = ElixirStorage::create("ElixirStorageLv1.png"); // 临时实例获取消耗
            int goldCost = tempMine->getGoldCost(), elixirCost = tempMine->getElixirCost();
            if (g_goldCount < goldCost || g_elixirCount < elixirCost) {
                return; // 直接返回，不允许放置
            }

            if (!isDragging) {
                log("elixirStorage ");
                isDragging = true;
                draggingItem = elixirStorageBtn;
                dragStartPosition = elixirStorageBtn->getPosition();

                auto elixirStoragePreview = ElixirStorage::create("ElixirStorageLv1.png"); 
                if (elixirStoragePreview) {
                    // 预览态设置：半透明（区分实际对象）
                    elixirStoragePreview->setOpacity(150);
                    Vec2 my = Vec2(elixirStoragePreview->getX(), elixirStoragePreview->getY());
                    if (background_sprite_) {
                        Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                        elixirStoragePreview->setMinePosition(you);

                        // 添加到背景精灵，并保存到按钮的UserData
                        background_sprite_->addChild(elixirStoragePreview, 10);
                        elixirStorageBtn->setUserData(elixirStoragePreview);
                    }
                }
            }
        }
    );
    if (elixirStorageBtn) {
        elixirStorageBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 * 3.5 +12));
    }
    elixirStorageBtn->setScale(0.6f);

    // 5.创建兵营按钮
    armyCampBtn = MenuItemImage::create(
        "ArmyCampLv1.png",
        "ArmyCampLv1.png",
        [=](Ref* pSender) {
            // 先检查是否有足够资源
            ArmyCamp* tempMine = ArmyCamp::create("ArmyCampLv1.png"); // 临时实例获取消耗
            int goldCost = tempMine->getGoldCost(), elixirCost = tempMine->getElixirCost();
            if (g_goldCount < goldCost || g_elixirCount < elixirCost) {
                return; // 直接返回，不允许放置
            }

            if (!isDragging) {
                log("armyCamp ");
                isDragging = true;
                draggingItem = armyCampBtn;
                dragStartPosition = armyCampBtn->getPosition();

                auto armyCampPreview = ArmyCamp::create("ArmyCampLv1.png");
                if (armyCampPreview) {
                    // 预览态设置：半透明（区分实际对象）
                    armyCampPreview->setOpacity(150);
                    Vec2 my = Vec2(armyCampPreview->getX(), armyCampPreview->getY());
                    if (background_sprite_) {
                        Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                        armyCampPreview->setMinePosition(you);

                        // 添加到背景精灵，并保存到按钮的UserData
                        background_sprite_->addChild(armyCampPreview, 10);
                        armyCampBtn->setUserData(armyCampPreview);
                    }
                }
            }
        }
    );
    if (armyCampBtn) {
        armyCampBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 * 4.5+15 ));
    }
    armyCampBtn->setScale(0.6f);

    // 6.创建城墙按钮
    wallsBtn = MenuItemImage::create(
        "WallsLv1.png",
        "WallsLv1.png",
        [=](Ref* pSender) {
            // 先检查是否有足够资源
            Walls* tempMine = Walls::create("WallsLv1.png"); // 临时实例获取消耗
            int goldCost = tempMine->getGoldCost(), elixirCost = tempMine->getElixirCost();
            if (g_goldCount < goldCost || g_elixirCount < elixirCost) {
                return; // 直接返回，不允许放置
            }

            if (!isDragging) {
                log("walls ");
                isDragging = true;
                draggingItem = wallsBtn;
                dragStartPosition = wallsBtn->getPosition();

                auto wallsPreview = Walls::create("WallsLv1.png"); // 预览用圣水瓶纹理
                if (wallsPreview) {
                    // 预览态设置：半透明（区分实际对象）
                    wallsPreview->setOpacity(150);
                    Vec2 my = Vec2(wallsPreview->getX(), wallsPreview->getY());
                    if (background_sprite_) {
                        Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                        wallsPreview->setMinePosition(you);

                        // 添加到背景精灵，并保存到按钮的UserData
                        background_sprite_->addChild(wallsPreview, 10);
                        wallsBtn->setUserData(wallsPreview);
                    }
                }
            }
        }
    );
    if (wallsBtn) {
        wallsBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 * 5.5 +20));
    }
    wallsBtn->setScale(0.6f);

    //7.建筑小屋按钮
    builderHutBtn = MenuItemImage::create(
        "BuilderHut.png",
        "BuilderHut.png",
        [=](Ref* pSender) {
            // 先检查是否有足够资源，且数量不超过5
            BuilderHut* tempMine = BuilderHut::create("BuilderHutLv1.png"); // 临时实例获取消耗
            int goldCost = tempMine->getGoldCost(), elixirCost = tempMine->getElixirCost();
            if (g_goldCount < goldCost || g_elixirCount < elixirCost || hutNum >= 5) {
                return; // 直接返回，不允许放置
            }

            if (!isDragging) {
                log("BuilderHut ");
                isDragging = true;
                draggingItem = builderHutBtn;
                dragStartPosition = builderHutBtn->getPosition();

                auto builderHutPreview = BuilderHut::create("BuilderHutLv1.png"); 
                if (builderHutPreview) {
                    // 预览态设置：半透明（区分实际对象）
                    builderHutPreview->setOpacity(150);
                    Vec2 my = Vec2(builderHutPreview->getX(), builderHutPreview->getY());
                    if (background_sprite_) {
                        Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                        builderHutPreview->setMinePosition(you);

                        // 添加到背景精灵，并保存到按钮的UserData
                        background_sprite_->addChild(builderHutPreview, 10);
                        builderHutBtn->setUserData(builderHutPreview);
                    }
                }
            }
        }
    );
    if (builderHutBtn) {
        builderHutBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 * 6.5 +25));
    }
    builderHutBtn->setScale(0.6f);

    // 8.创建箭塔按钮
    archerTowerBtn = MenuItemImage::create(
        "ArcherTowerLv1.png",
        "ArcherTowerLv1.png",
        [=](Ref* pSender) {
            // 先检查是否有足够资源
            ArcherTower* tempMine = ArcherTower::create("ArcherTowerLv1.png"); // 临时实例获取消耗
            int goldCost = tempMine->getGoldCost(), elixirCost = tempMine->getElixirCost();
            if (g_goldCount < goldCost || g_elixirCount < elixirCost) {
                return; // 直接返回，不允许放置
            }

            if (!isDragging) {
                log("archerTower ");
                isDragging = true;
                draggingItem = archerTowerBtn;
                dragStartPosition = archerTowerBtn->getPosition();

                auto archerTowerPreview = ArcherTower::create("ArcherTowerLv1.png"); 
                if (archerTowerPreview) {
                    // 预览态设置：半透明（区分实际对象）
                    archerTowerPreview->setOpacity(150);
                    Vec2 my = Vec2(archerTowerPreview->getX(), archerTowerPreview->getY());
                    if (background_sprite_) {
                        Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                        archerTowerPreview->setMinePosition(you);

                        // 添加到背景精灵，并保存到按钮的UserData
                        background_sprite_->addChild(archerTowerPreview, 10);
                        archerTowerBtn->setUserData(archerTowerPreview);
                    }
                }
            }
        }
    );
    if (archerTowerBtn) {
        archerTowerBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 * 7.5 + 15));
    }
    archerTowerBtn->setScale(0.6f);

    // 9.创建加农炮按钮
    cannonBtn = MenuItemImage::create(
        "CannonLv1.png",
        "CannonLv1.png",
        [=](Ref* pSender) {
            // 先检查是否有足够资源
            Cannon* tempMine = Cannon::create("CannonLv1.png"); // 临时实例获取消耗
            int goldCost = tempMine->getGoldCost(), elixirCost = tempMine->getElixirCost();
            if (g_goldCount < goldCost || g_elixirCount < elixirCost) {
                return; // 直接返回，不允许放置
            }

            if (!isDragging) {
                log("Cannon ");
                isDragging = true;
                draggingItem = cannonBtn;
                dragStartPosition = cannonBtn->getPosition();

                auto cannonPreview = Cannon::create("CannonLv1.png"); 
                if (cannonPreview) {
                    // 预览态设置：半透明（区分实际对象）
                    cannonPreview->setOpacity(150);
                    Vec2 my = Vec2(cannonPreview->getX(), cannonPreview->getY());
                    if (background_sprite_) {
                        Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                        cannonPreview->setMinePosition(you);

                        // 添加到背景精灵，并保存到按钮的UserData
                        background_sprite_->addChild(cannonPreview, 10);
                        cannonBtn->setUserData(cannonPreview);
                    }
                }
            }
        }
    );
    if (cannonBtn) {
        cannonBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 * 8.5 + 10));
    }
    cannonBtn->setScale(0.9f);

    // 10.创建迫击炮按钮
    mortarBtn = MenuItemImage::create(
        "MortarLv1.png",
        "MortarLv1.png",
        [=](Ref* pSender) {
            // 先检查是否有足够资源
            Mortar* tempMine = Mortar::create("MortarLv1.png"); // 临时实例获取消耗
            int goldCost = tempMine->getGoldCost(), elixirCost = tempMine->getElixirCost();
            if (g_goldCount < goldCost || g_elixirCount < elixirCost) {
                return; // 直接返回，不允许放置
            }

            if (!isDragging) {
                log("mortar ");
                isDragging = true;
                draggingItem = mortarBtn;
                dragStartPosition = mortarBtn->getPosition();

                auto mortarPreview = Mortar::create("MortarLv1.png"); 
                if (mortarPreview) {
                    // 预览态设置：半透明（区分实际对象）
                    mortarPreview->setOpacity(150);
                    Vec2 my = Vec2(mortarPreview->getX(), mortarPreview->getY());
                    if (background_sprite_) {
                        Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                        mortarPreview->setMinePosition(you);

                        // 添加到背景精灵，并保存到按钮的UserData
                        background_sprite_->addChild(mortarPreview, 10);
                        mortarBtn->setUserData(mortarPreview);
                    }
                }
            }
        }
    );
    if (mortarBtn) {
        mortarBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 * 9.5 + 10));
    }
    mortarBtn->setScale(0.8f);

    auto panelMenu = Menu::create(goldMineBtn, elixirCollectorBtn, goldStorageBtn, elixirStorageBtn,armyCampBtn, wallsBtn,builderHutBtn, archerTowerBtn,cannonBtn,mortarBtn,nullptr);
    panelMenu->setPosition(Vec2::ZERO);
    panelBg->addChild(panelMenu);

    // 创建缩放管理器
    zoom_manager_ = ZoomScrollManager::create(background_sprite_, 0.5f, 2.0f);
    this->addChild(zoom_manager_);

    // 初始化菱形位置信息（根据原有测量数据）
    int LEFT_X = 667;          // 左顶点x坐标（距左边界）
    int RIGHT_X = 3705 - 556;  // 右顶点x坐标（图像宽度 - 距右边界距离）
    int TOP_Y = 264;           // 上顶点y坐标（距上边界）
    int BOTTOM_Y = 2545 - 471; // 下顶点y坐标（图像高度 - 距下边界距离）

    float diamond_width = RIGHT_X - LEFT_X;       // 菱形宽度: 3149 - 667 = 2482
    float diamond_height = BOTTOM_Y - TOP_Y;      // 菱形高度: 2074 - 264 = 1810

    // 计算菱形中心相对于背景精灵中心的位置
    //Vec2 diamond_center_absolute = Vec2((left_x + right_x) / 2.0f, (top_y + bottom_y) / 2.0f);
    //Vec2 diamond_center = diamond_center_absolute - Vec2(background_sprite_->getContentSize().width / 2+28.0, background_sprite_->getContentSize().height / 2 -42.0*4);

    float diamond_center_x = (LEFT_X + RIGHT_X) / 2.0f;
    float diamond_center_y = (TOP_Y + BOTTOM_Y) / 2.0f;

    Vec2 diamond_center_absolute = Vec2(diamond_center_x, diamond_center_y);
    Vec2 diamond_center = diamond_center_absolute - Vec2(
        background_sprite_->getContentSize().width / 2.0f+28.0,
        background_sprite_->getContentSize().height / 2.0f-42.0*4
    );
    // 创建菱形网格管理器
    grid_manager_ = DiamondGridManager::create(diamond_center, diamond_width, diamond_height, 50);
    this->addChild(grid_manager_);

    // 绘制菱形网格
    grids_ = grid_manager_->drawDiamondGrid(background_sprite_, 50.0f);

    // 创建圣水图标和标签
    elixirIcon = Sprite::create("btn_normal.png"); // 实际项目中应该替换为正确的圣水图标资源名
    if (elixirIcon == nullptr)
    {
        problemLoading("'btn_normal.png' (作为圣水图标的替代)");
    }
    else
    {
        // 设置图标位置和大小
        elixirIcon->setPosition(Vec2(origin.x + visibleSize.width - elixirIcon->getContentSize().width / 4, origin.y + visibleSize.height - elixirIcon->getContentSize().height / 4));
        elixirIcon->setScale(0.5f);
        this->addChild(elixirIcon, 2);

        // 创建"圣水"文字标签
        elixirNameLabel = Label::createWithTTF("圣水", "fonts/STZhongSong_Bold.ttf", 20);
        elixirNameLabel->setColor(Color3B::BLUE);
        elixirNameLabel->setPosition(Vec2(elixirIcon->getPositionX() - elixirNameLabel->getContentSize().width / 2-50, elixirIcon->getPositionY()));
        this->addChild(elixirNameLabel, 2);

        // 创建圣水数量标签
        elixirLabel = Label::createWithTTF("750", "fonts/Marker Felt.ttf", 24);
        elixirLabel->setColor(Color3B::BLUE);
        elixirLabel->setPosition(Vec2(elixirIcon->getPositionX() , elixirIcon->getPositionY()));
        this->addChild(elixirLabel, 2);
    }

    // 创建金币图标和标签
    goldIcon = Sprite::create("btn_normal.png"); // 实际项目中应该替换为正确的金币图标资源名
    if (goldIcon == nullptr)
    {
        problemLoading("'btn_normal.png' (作为金币图标的替代)");
    }
    else
    {
        // 设置图标位置和大小，紧挨着圣水图标
        float spacing = 40.0f; // 与圣水图标之间的间距
        goldIcon->setPosition(Vec2(elixirIcon->getPositionX(), elixirIcon->getPositionY()-spacing));
        goldIcon->setScale(0.5f);
        this->addChild(goldIcon, 2);

        // 创建"金币"文字标签
        goldNameLabel = Label::createWithTTF("金币", "fonts/STZhongSong_Bold.ttf", 20);
        goldNameLabel->setColor(Color3B::YELLOW);
        goldNameLabel->setPosition(Vec2(goldIcon->getPositionX() - goldNameLabel->getContentSize().width / 2-50, goldIcon->getPositionY()));
        this->addChild(goldNameLabel, 2);

        // 创建金币数量标签
        goldLabel = Label::createWithTTF("750", "fonts/Marker Felt.ttf", 24);
        goldLabel->setColor(Color3B::YELLOW);
        goldLabel->setPosition(Vec2(goldIcon->getPositionX(), goldIcon->getPositionY()));
        this->addChild(goldLabel, 2);
    }

    // 347-361 创建宝石图标和标签
    gemIcon= Sprite::create("btn_normal.png");
    if (gemIcon == nullptr)
    {
        problemLoading("'btn_normal.png' (作为金币图标的替代)");
    }
    else
    {
        // 设置图标位置和大小，紧挨着圣水图标
        float spacing = 40.0f; // 与圣水图标之间的间距
        gemIcon->setPosition(Vec2(goldIcon->getPositionX(), goldIcon->getPositionY() - spacing));
        gemIcon->setScale(0.5f);
        this->addChild(gemIcon, 2);

        // 创建"宝石"文字标签
        gemNameLabel = Label::createWithTTF("宝石", "fonts/STZhongSong_Bold.ttf", 20);
        gemNameLabel->setColor(Color3B::GREEN);
        gemNameLabel->setPosition(Vec2(gemIcon->getPositionX() - gemNameLabel->getContentSize().width / 2-50, gemIcon->getPositionY()));
        this->addChild(gemNameLabel, 2);

        // 创建金币数量标签
        gemLabel = Label::createWithTTF("15", "fonts/Marker Felt.ttf", 24);
        gemLabel->setColor(Color3B::GREEN);
        gemLabel->setPosition(Vec2(gemIcon->getPositionX() , gemIcon->getPositionY()));
        this->addChild(gemLabel, 2);
    }

    // 设置事件监听器（使用新类的成员函数）
    auto touch_listener = EventListenerTouchOneByOne::create();
    touch_listener->setSwallowTouches(true);
    touch_listener->onTouchBegan = CC_CALLBACK_2(SecondScene::onTouchBegan, this);
    touch_listener->onTouchMoved = CC_CALLBACK_2(SecondScene::onTouchMoved, this);
    touch_listener->onTouchEnded = CC_CALLBACK_2(SecondScene::onTouchEnded, this);
    touch_listener->onTouchCancelled = CC_CALLBACK_2(SecondScene::onTouchCancelled, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);

    auto mouse_listener = EventListenerMouse::create();
    mouse_listener->onMouseScroll = CC_CALLBACK_1(SecondScene::onMouseScroll, this);
    mouse_listener->onMouseMove = CC_CALLBACK_1(SecondScene::onMouseMove, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouse_listener, this);

    auto multi_touch_listener = EventListenerTouchAllAtOnce::create();
    multi_touch_listener->onTouchesBegan = CC_CALLBACK_2(SecondScene::onTouchesBegan, this);
    multi_touch_listener->onTouchesMoved = CC_CALLBACK_2(SecondScene::onTouchesMoved, this);
    multi_touch_listener->onTouchesEnded = CC_CALLBACK_2(SecondScene::onTouchesEnded, this);
    multi_touch_listener->onTouchesCancelled = CC_CALLBACK_2(SecondScene::onTouchesCancelled, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(multi_touch_listener, this);

    // 启动定时器，每秒更新一次
    this->scheduleUpdate();
    return true;
}

void SecondScene::update(float delta)
{
    // 累计时间并每秒增加圣水数量
    static float elapsedTime = 0.0f;
    elapsedTime += delta;
    Building* notFullGoldStorage = nullptr, * notFullElixirStorage = nullptr;
    for (auto building : placedBuildings) {
        if (dynamic_cast<GoldStorage*>(building) && building->getCurrentStock() < building->getMaxStock()) {
            notFullGoldStorage = building;
        }
    }
    for (auto building : placedBuildings) {
        if (dynamic_cast<ElixirStorage*>(building) && building->getCurrentStock() < building->getMaxStock()) {
            notFullElixirStorage = building;
        }
    }
    // 当经过1秒时
    if (elapsedTime >= 1.0f)
    {
        // 判断建筑类型并分别累加速度
        for (auto building : placedBuildings) {
            if (!building) continue;
            if (building->getIsUpgrade()) {
                building->cutTime();
            }
            //下均为非升级中
            else if (dynamic_cast<GoldMine*>(building)) {
                //两个临时变量存储
                int tempGold = building->getSpeed();
                while (tempGold > 0) {
                    //金矿非常未满
                    if (building->getMaxStock() - building->getCurrentStock() >= tempGold) {
                        building->updateCurrentStock(tempGold);
                        tempGold = 0;//接下来肯定会退出循环
                        break;
                    }
                    //金矿将要存满,存入building->getMaxStock() - building->getCurrentStock()，其余尽量进存钱罐
                    else if (building->getMaxStock() - building->getCurrentStock() < tempGold && building->getMaxStock() - building->getCurrentStock() > 0) {
                        building->updateCurrentStock(building->getMaxStock() - building->getCurrentStock());//尽量存
                        tempGold -= (building->getMaxStock() - building->getCurrentStock());//剩余未存
                        continue;//金矿已满，下次while循环会直接跳到下面对于存钱罐的判断
                    }
                    //有
                    if (notFullGoldStorage != nullptr) {
                        if (notFullGoldStorage->getMaxStock() - notFullGoldStorage->getCurrentStock() >= tempGold) {
                            notFullGoldStorage->addCurrent(tempGold);
                            tempGold = 0;
                            break;
                        }
                        else {
                            notFullGoldStorage->addCurrent(notFullGoldStorage->getMaxStock() - notFullGoldStorage->getCurrentStock());
                            tempGold -= notFullGoldStorage->getMaxStock() - notFullGoldStorage->getCurrentStock();//剩余未存
                            break;
                        }
                    }
                    else {
                        break;
                    }
                }
            }
            else if (dynamic_cast<ElixirCollector*>(building)) {
                int tempElixir = building->getSpeed();
                while (tempElixir > 0) {
                    //非常未满
                    if (building->getMaxStock() - building->getCurrentStock() >= tempElixir) {
                        building->updateCurrentStock(tempElixir);
                        tempElixir = 0;//接下来肯定会退出循环
                        break;
                    }
                    //将要存满,存入building->getMaxStock() - building->getCurrentStock()，其余尽量进存罐
                    else if (building->getMaxStock() - building->getCurrentStock() < tempElixir && building->getMaxStock() - building->getCurrentStock() > 0) {
                        building->updateCurrentStock(building->getMaxStock() - building->getCurrentStock());
                        tempElixir -= (building->getMaxStock() - building->getCurrentStock());
                        continue;
                    }
                    //有
                    else if (notFullElixirStorage != nullptr) {
                        if (notFullElixirStorage->getMaxStock() - notFullElixirStorage->getCurrentStock() >= tempElixir) {
                            notFullElixirStorage->addCurrent(tempElixir);
                            tempElixir = 0;
                            break;
                        }
                        else {
                            notFullElixirStorage->addCurrent(notFullElixirStorage->getMaxStock() - notFullElixirStorage->getCurrentStock());
                            tempElixir -= (notFullElixirStorage->getMaxStock() - notFullElixirStorage->getCurrentStock());//剩余未存
                            break;
                        }
                    }
                    else {
                        break;
                    }
                }
            }
            else {
                continue;//只有金矿和圣水收集器需要实时更新
            }
        }
        // 每一秒都更新标签显示
        if (elixirLabel) {
            elixirLabel->setString(StringUtils::format("%d", g_elixirCount));
        }
        if (goldLabel) {
            goldLabel->setString(StringUtils::format("%d", g_goldCount));
        }
        if (gemLabel) {
            gemLabel->setString(StringUtils::format("%d", g_gemCount));
        }
        // 重置计时器
        elapsedTime = 0.0f;
    }
}
void SecondScene::menuFirstCallback(Ref* pSender)
{
    Director::getInstance()->replaceScene(HelloWorld::createScene());
}

void SecondScene::menuBuildCallback(Ref* pSender)
{
    buildPanel->setVisible(!buildPanel->isVisible());
}

void SecondScene::menuAttackCallback(Ref* pSender)
{
    attackPanel->setVisible(!attackPanel->isVisible());
}

void SecondScene::menuBoss1Callback(Ref* pSender)
{
    auto config = CombatSessionManager::getInstance();
    config->reset();

    // 建立临时 Map 进行累加 (Key: 兵种, Value: 兵力数据结构)
    // 使用 Map 可以自动合并多个军营中相同的兵种
    std::map<UnitType, Army> combinedArmy;

    for (auto building : placedBuildings)
    {
        auto camp = dynamic_cast<ArmyCamp*>(building);
        if (camp)
        {
            for (int i = 0; i < 6; i++)
            {
                int count = camp->getArmy(i);
                if (count > 0)
                {
                    UnitType t = static_cast<UnitType>(static_cast<int>(UnitType::BARBARIAN) + i);

                    if (combinedArmy.find(t) == combinedArmy.end())
                    {
                        combinedArmy[t].type = t;
                        combinedArmy[t].level = camp->getLv(); 
                        combinedArmy[t].amount = 0;
                    }
                    combinedArmy[t].amount += count;

                    //如果不同军营等级不同，这里可以取最高等级
                    if (camp->getLv() > combinedArmy[t].level)
                    {
                        combinedArmy[t].level = camp->getLv();
                    }
                }
            }
        }
    }

    auto& finalInv = config->battle_start_params.army;
    finalInv.clear();

    for (auto const& pair : combinedArmy)
    {
        finalInv.push_back(pair.second);
    }

    Director::getInstance()->replaceScene(BattleTestLayer::createScene());
}

void SecondScene::menuBoss2Callback(Ref* pSender)
{
    Director::getInstance()->replaceScene(BattleTestLayer::createScene());
}


bool SecondScene::onTouchBegan(Touch* touch, Event* event)
{
    // 1. 获取当前时间（使用clock()计算程序运行总时间，单位为秒）
    double currentTime = clock() / (double)CLOCKS_PER_SEC;
    Vec2 currentPos = touch->getLocation();
    double timeDiff = currentTime - _lastClickTime; // 时间差
    float posDiff = currentPos.distance(_lastClickPos); // 位置差

    // 判断是否为双击（时间差小于阈值且位置偏差较小）
    _isDoubleClick = (timeDiff < DOUBLE_CLICK_INTERVAL) && (posDiff < 10.0f);

    // 更新上一次点击记录
    _lastClickTime = currentTime;
    _lastClickPos = currentPos;

    // 2. 如果是双击，执行双击操作（例如打开升级面板）
    if (_isDoubleClick) {
        log("检测到双击");
        // 找到被双击的建筑
        Building* clickedBuilding = nullptr;
        Vec2 touchPos = touch->getLocation();//!!!改为附近坐标

        for (auto& building : placedBuildings) {
            if (!building || !background_sprite_) continue;
            // 复用菱形碰撞检测代码（判断触摸点是否在当前建筑的菱形范围内）
            Sprite* mineSprite = building->getSprite();
            Vec2 buildingScreenPos = background_sprite_->convertToWorldSpace(building->getPosition());
            const float horizontalDiag = 56.0f * building->getSize();
            const float verticalDiag = 42.0f * building->getSize();
            float a = horizontalDiag / 2;
            float b = verticalDiag / 2;
            float dx = touchPos.x - buildingScreenPos.x;
            float dy = touchPos.y - buildingScreenPos.y;
            bool isInDiamond = (fabs(dx) / a + fabs(dy) / b) <= 1.0f;

            if (isInDiamond) {
                clickedBuilding = building;
                break;
            }
        }

        // 若找到建筑，显示信息面板
        if (clickedBuilding) {       
            // 已有面板，且是当前建筑 → 关闭面板
            if (_curOpenInfoPanel && _curOpenBuilding == clickedBuilding) {
                _curOpenInfoPanel->removeFromParent(); // 关闭面板
                _curOpenInfoPanel = nullptr;
                _curOpenBuilding = nullptr;
            }
            // 已有面板，但不是当前建筑 → 关闭旧面板，打开新面板
            else if (_curOpenInfoPanel && _curOpenBuilding != clickedBuilding) {
                _curOpenInfoPanel->removeFromParent(); // 关闭旧面板
                _curOpenInfoPanel = nullptr;

                // 创建新面板
                if (background_sprite_) {
                    _curOpenInfoPanel = BuildingInfoPanel::create(clickedBuilding, background_sprite_);
                    clickedBuilding->addChild(_curOpenInfoPanel, 100); // 确保面板在最上层
                }
                _curOpenBuilding = clickedBuilding; // 更新绑定的建筑
            }
            // 无面板 → 打开新面板
            else if (background_sprite_) {
                _curOpenInfoPanel = BuildingInfoPanel::create(clickedBuilding, background_sprite_);
                clickedBuilding->addChild(_curOpenInfoPanel, 100);
                _curOpenBuilding = clickedBuilding;
            }
        }
        return true; // 吞噬事件，避免触发移动
    }
    else {
        // 非双击，只处理拖拽状态下的逻辑、建筑移动逻辑和缩放管理器的逻辑   
        if (isDragging) {

            return true; // 正在拖拽时返回true，保持事件被捕获
        }
        Vec2 touchPos = touch->getLocation();
        // 检查是否点击了已放置的
        for (auto& building : placedBuildings) {
            if (!building || !background_sprite_) continue;
            Sprite* mineSprite = building->getSprite();
            if (!mineSprite) continue;

            // 建筑的世界坐标（菱形中心）
            Vec2 buildingScreenPos = background_sprite_->convertToWorldSpace(building->getPosition());

            // 菱形参数配置
            const float horizontalDiag = 56.0f * building->getSize(); // 水平对角线总长度
            const float verticalDiag = 42.0f * building->getSize();   // 竖直对角线总长度
            const float a = horizontalDiag / 2;     // 水平半轴（x方向）
            const float b = verticalDiag / 2;       // 竖直半轴（y方向）

            // 菱形碰撞检测核心公式
            float dx = touchPos.x - buildingScreenPos.x; // 触摸点与中心的x偏移
            float dy = touchPos.y - buildingScreenPos.y; // 触摸点与中心的y偏移
            bool isInDiamond = (fabs(dx) / a + fabs(dy) / b) <= 1.0f;

            // 如果触摸点在菱形内，触发建筑移动逻辑
            if (isInDiamond) {
                if (_curOpenInfoPanel) {
                    _curOpenInfoPanel->removeFromParent(); // 关闭面板
                    _curOpenInfoPanel = nullptr;
                }
                isMovingBuilding = true;
                movingBuilding = building;
                _movingBuildingOriginalPos = building->getPosition(); // 保存原始位置
                building->setOpacity(128);
                background_sprite_->reorderChild(building, 20);
                return true;
            }
        }
        // 如果没有拖拽且没有点击建筑，则使用缩放管理器的触摸处理
        if (zoom_manager_) {
            return zoom_manager_->onTouchBegan(touch, event);
        }
        return false;
    }
}

void SecondScene::onTouchMoved(Touch* touch, Event* event)
{
    if (isDragging && draggingItem) {
        // 区分拖拽的是金矿预览还是圣水收集器预览
        void* userData = draggingItem->getUserData();
        if (!userData) {
            return;
        }

        if (!background_sprite_) {
            return;
        }

        Vec2 localPos = background_sprite_->convertToNodeSpace(touch->getLocation());
        //Vec2 localPos = ConvertTest::convertScreenToGrid(touch->getLocation(), background_sprite_, buildPanel);

        if (!grid_manager_) {
            return;
        }
        float gridCellSizeX = grid_manager_->getGridCellSizeX()/2;
        float gridCellSizeY = grid_manager_->getGridCellSizeY()/2;
        float snappedX = ceil(localPos.x / gridCellSizeX) * gridCellSizeX;
        float snappedY = ceil(localPos.y / gridCellSizeY) * gridCellSizeY;

        // 金矿预览
        if (draggingItem == goldMineBtn) {
            GoldMine* dragMinePreview = static_cast<GoldMine*>(userData);
            if (dragMinePreview) {
                dragMinePreview->setPosition(Vec2(snappedX, snappedY));
            }
        }
        // 圣水收集器预览
        else if (draggingItem == elixirCollectorBtn) {
            ElixirCollector* dragElixirPreview = static_cast<ElixirCollector*>(userData);
            if (dragElixirPreview) {
                dragElixirPreview->setPosition(Vec2(snappedX, snappedY));
            }
        }
        //存钱罐预览
        else if(draggingItem == goldStorageBtn){
            GoldStorage* dragGoldStoragePreview = static_cast<GoldStorage*>(userData);
            if (dragGoldStoragePreview) {
                dragGoldStoragePreview->setPosition(Vec2(snappedX, snappedY));
            }
        }
        //圣水瓶预览
        else if (draggingItem == elixirStorageBtn) {
            ElixirStorage* dragElixirStoragePreview = static_cast<ElixirStorage*>(userData);
            if (dragElixirStoragePreview) {
                dragElixirStoragePreview->setPosition(Vec2(snappedX, snappedY));
            }
        }
        //兵营预览
        else if (draggingItem == armyCampBtn) {
            ArmyCamp* dragArmyCampPreview = static_cast<ArmyCamp*>(userData);
            if (dragArmyCampPreview) {
                dragArmyCampPreview->setPosition(Vec2(snappedX, snappedY));
            }
        }
        //城墙预览
        else if (draggingItem == wallsBtn) {
            Walls* dragWallsPreview = static_cast<Walls*>(userData);
            if (dragWallsPreview) {
                dragWallsPreview->setPosition(Vec2(snappedX, snappedY));
            }
        }
        //小屋预览
        else if (draggingItem == builderHutBtn) {
            BuilderHut* dragBuilderHutPreview = static_cast<BuilderHut*>(userData);
            if (dragBuilderHutPreview) {
                dragBuilderHutPreview->setPosition(Vec2(snappedX, snappedY));
            }
        }
        //箭塔预览
        else if (draggingItem == archerTowerBtn) {
            ArcherTower* dragArcherTowerPreview = static_cast<ArcherTower*>(userData);
            if (dragArcherTowerPreview) {
                dragArcherTowerPreview->setPosition(Vec2(snappedX, snappedY));
            }
        }
        //加农炮预览
        else if (draggingItem == cannonBtn) {
            Cannon* dragCannonPreview = static_cast<Cannon*>(userData);
            if (dragCannonPreview) {
                dragCannonPreview->setPosition(Vec2(snappedX, snappedY));
            }
        }
        //迫击炮预览
        else if (draggingItem == mortarBtn) {
            Mortar* dragMortarPreview = static_cast<Mortar*>(userData);
            if (dragMortarPreview) {
                dragMortarPreview->setPosition(Vec2(snappedX, snappedY));
            }
        }
    }
    else if (isMovingBuilding) {
        if (!background_sprite_ || !grid_manager_) {
            return;
        }
        Vec2 localPos = background_sprite_->convertToNodeSpace(touch->getLocation());       
        //Vec2 localPos = ConvertTest::convertScreenToGrid(touch->getLocation(), background_sprite_, buildPanel);
        Vec2 diamondPos = convertScreenToDiamond(touch->getLocation());
        bool inDiamond = isInDiamond(diamondPos);

        float gridCellSizeX = grid_manager_->getGridCellSizeX()/2;
        float gridCellSizeY = grid_manager_->getGridCellSizeY()/2;
        float snappedX = ceil(localPos.x / gridCellSizeX) * gridCellSizeX;
        float snappedY = ceil(localPos.y / gridCellSizeY) * gridCellSizeY;
        static Vec2 lastValidMovePos;

        if (inDiamond) {
            // 在移动建筑的代码块中添加
            bool isColliding = false;

            // 检查与其他建筑的碰撞
            if (movingBuilding) {
                // 排除自身进行检测
                for (auto building : placedBuildings) {
                    if (building == movingBuilding) continue;
                    if (isPointInBuilding(Vec2(snappedX, snappedY), building)) {
                        isColliding = true;
                        break;
                    }
                }              
            }          
            // 如果碰撞，不更新位置或显示红色提示
            if (isColliding) {
                // 可以将建筑设为红色提示碰撞
                if (movingBuilding) {
                    movingBuilding->getSprite()->setColor(Color3B::RED);
                }
            }
            //不碰撞&&在界内
            else {
                // 恢复颜色
                if (movingBuilding) {
                    movingBuilding->getSprite()->setColor(Color3B::WHITE);
                }
            }
            // 如果没有碰撞，继续移动
            lastValidMovePos = Vec2(snappedX, snappedY);
        }
        //不在界内，不更新
        else {
            snappedX = lastValidMovePos.x;
            snappedY = lastValidMovePos.y;
        }

        // 移动
        if (movingBuilding) {
            movingBuilding->setPosition(Vec2(snappedX, snappedY));
        }
    }
    else if (zoom_manager_) {
        zoom_manager_->onTouchMoved(touch, event);
    }
}

void SecondScene::onTouchEnded(Touch* touch, Event* event)
{
    if (_isDoubleClick) {
        _isDoubleClick = false; // 重置双击标记
        return; // 双击时不执行移动逻辑
    }

    if (isDragging && draggingItem) {
        if (!background_sprite_) {
            isDragging = false;
            draggingItem = nullptr;
            return;
        }
        // 获取拖拽结束位置
        Vec2 screenPos = touch->getLocation();
        // 将屏幕坐标转换为相对于背景精灵的本地坐标
        Vec2 localPos = background_sprite_->convertToNodeSpace(screenPos);
        //Vec2 localPos = ConvertTest::convertScreenToGrid(screenPos, background_sprite_, buildPanel);

        if (!grid_manager_) {
            isDragging = false;
            draggingItem = nullptr;
            return;
        }
        // 获取网格单元格大小并进行向上取整
        float gridCellSizeX = grid_manager_->getGridCellSizeX()/2;
        float gridCellSizeY = grid_manager_->getGridCellSizeY()/2;
        float snappedX = ceil(localPos.x / gridCellSizeX) * gridCellSizeX;
        float snappedY = ceil(localPos.y / gridCellSizeY) * gridCellSizeY;
        Vec2 snappedPos = Vec2(snappedX, snappedY);

        // 检查放置区域有效性
        Vec2 diamondPos = convertScreenToDiamond(screenPos);
        // 在有效区域
        if (isInDiamond(diamondPos)) {
            bool isColliding = false;
            Vec2 targetPos = snappedPos;

            // 检查与已放置建筑的碰撞
            for (auto building : placedBuildings) {
                if (!building) continue;
                if (isPointInBuilding(targetPos, building)) {
                    isColliding = true;
                    break;
                }
            }

            if (isColliding) {
                // 碰撞处理：播放失败动画
                log("放置位置与其他建筑冲突！");
                // 这里可以添加显示提示信息的逻辑
                if (draggingItem == goldMineBtn) {
                    auto failGoldMine = GoldMine::create("GoldMineLv1.png");
                    if (failGoldMine) {
                        failGoldMine->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failGoldMine, 15);
                        }
                        failGoldMine->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == elixirCollectorBtn) {
                    auto failElixir = ElixirCollector::create("ElixirCollectorLv1.png");
                    if (failElixir) {
                        failElixir->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failElixir, 15);
                        }
                        failElixir->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == goldStorageBtn) {
                    auto failGoldStorage = GoldStorage::create("GoldStorageLv1.png");
                    if (failGoldStorage) {
                        failGoldStorage->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failGoldStorage, 15);
                        }
                        failGoldStorage->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == elixirStorageBtn) {
                    auto failElixirStorage = ElixirStorage::create("ElixirStorageLv1.png");
                    if (failElixirStorage) {
                        failElixirStorage->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failElixirStorage, 15);
                        }
                        failElixirStorage->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == armyCampBtn) {
                    auto failArmyCamp = ElixirStorage::create("ArmyCampLv1.png");
                    if (failArmyCamp) {
                        failArmyCamp->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failArmyCamp, 15);
                        }
                        failArmyCamp->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == wallsBtn) {
                    auto failwalls = Walls::create("WallsLv1.png");
                    if (failwalls) {
                        failwalls->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failwalls, 15);
                        }
                        failwalls->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == builderHutBtn) {
                    auto failBuilderHut = BuilderHut::create("BuilderHutLv1.png");
                    if (failBuilderHut) {
                        failBuilderHut->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failBuilderHut, 15);
                        }
                        failBuilderHut->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == archerTowerBtn) {
                    auto failarcherTower = ArcherTower::create("ArcherTowerLv1.png");
                    if (failarcherTower) {
                        failarcherTower->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failarcherTower, 15);
                        }
                        failarcherTower->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == cannonBtn) {
                    auto failCannon = Cannon::create("CannonLv1.png");
                    if (failCannon) {
                        failCannon->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failCannon, 15);
                        }
                        failCannon->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == mortarBtn) {
                    auto failMortar = Mortar::create("MortarLv1.png");
                    if (failMortar) {
                        failMortar->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failMortar, 15);
                        }
                        failMortar->playFailBlinkAndRemove();
                    }
                }
                return; // 阻止放置
            }
            // 如果没有碰撞，继续执行放置逻辑           
            if (draggingItem == goldMineBtn) {
                // 创建金矿
                int goldCost = static_cast<GoldMine*>(draggingItem->getUserData())->getGoldCost();
                int elixirCost = static_cast<GoldMine*>(draggingItem->getUserData())->getElixirCost();
                //再次判断资源是否足够
                if (g_goldCount >= goldCost && g_elixirCount >= elixirCost) {
                    //除旧
                    GoldMine* dragMinePreview = static_cast<GoldMine*>(draggingItem->getUserData());
                    if (dragMinePreview) {
                        dragMinePreview->removeFromParentAndCleanup(true);
                        draggingItem->setUserData(nullptr);
                    }
                    //迎新
                    auto placedGoldMine = GoldMine::create("GoldMineLv1.png");
                    if (placedGoldMine) {
                        // 更新
                        placedGoldMine->updatePosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(placedGoldMine, 15);
                        }
                        placedBuildings.push_back(placedGoldMine);
                        placedGoldMine->setScale(0.7f);
                        placedGoldMine->playSuccessBlink();
                        sendSaveBuildingRequest("GoldMine", snappedPos.x, snappedPos.y, 1,
                            placedGoldMine->getHp(), placedGoldMine->getHp(),
                            placedGoldMine->getSpeed(), placedGoldMine->getMaxStock(), 0);
                    }
                    //扣除资源
                    g_goldCount -= goldCost;
                    g_elixirCount -= elixirCost;
                }
            }
            else if (draggingItem == elixirCollectorBtn) {
                // 创建圣水收集器
                int goldCost = static_cast<ElixirCollector*>(draggingItem->getUserData())->getGoldCost();
                int elixirCost = static_cast<ElixirCollector*>(draggingItem->getUserData())->getElixirCost();
                //再次判断资源是否足够
                if (g_goldCount >= goldCost && g_elixirCount >= elixirCost) {
                    //除旧
                    ElixirCollector* dragElixirCollectorPreview = static_cast<ElixirCollector*>(draggingItem->getUserData());
                    if (dragElixirCollectorPreview) {
                        dragElixirCollectorPreview->removeFromParentAndCleanup(true);
                        draggingItem->setUserData(nullptr);
                    }
                    //迎新
                    auto placedElixirCollector = ElixirCollector::create("ElixirCollectorLv1.png");
                    if (placedElixirCollector) {
                        // 更新
                        placedElixirCollector->updatePosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(placedElixirCollector, 15);
                        }
                        placedBuildings.push_back(placedElixirCollector);
                        placedElixirCollector->setScale(0.8f);
                        placedElixirCollector->playSuccessBlink();
                        sendSaveBuildingRequest("ElixirCollector", snappedPos.x, snappedPos.y, 1,
                            placedElixirCollector->getHp(), placedElixirCollector->getHp(),
                            placedElixirCollector->getSpeed(), placedElixirCollector->getMaxStock(), 0);
                    }
                    //扣除资源
                    g_goldCount -= goldCost;
                    g_elixirCount -= elixirCost;
                }
            }
            else if (draggingItem == goldStorageBtn) {
                // 创建存钱罐
                int goldCost = static_cast<GoldStorage*>(draggingItem->getUserData())->getGoldCost();
                int elixirCost = static_cast<GoldStorage*>(draggingItem->getUserData())->getElixirCost();
                //再次判断资源是否足够
                if (g_goldCount >= goldCost && g_elixirCount >= elixirCost) {
                    //除旧
                    GoldStorage* dragGoldStoragePreview = static_cast<GoldStorage*>(draggingItem->getUserData());
                    if (dragGoldStoragePreview) {
                        dragGoldStoragePreview->removeFromParentAndCleanup(true);
                        draggingItem->setUserData(nullptr);
                    }
                    //迎新
                    auto placedGoldStorage = GoldStorage::create("GoldStorageLv1.png");
                    if (placedGoldStorage) {
                        // 更新
                        placedGoldStorage->updatePosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(placedGoldStorage, 15);
                        }
                        placedBuildings.push_back(placedGoldStorage);
                        placedGoldStorage->setScale(0.7f);
                        placedGoldStorage->playSuccessBlink();
                        sendSaveBuildingRequest("GoldStorage", snappedPos.x, snappedPos.y, 1,
                            placedGoldStorage->getHp(), placedGoldStorage->getHp(),
                            0, placedGoldStorage->getMaxStock(), 0);
                    }
                    //扣除资源
                    g_goldCount -= goldCost;
                    g_elixirCount -= elixirCost;
                }
            }
            else if (draggingItem == elixirStorageBtn) {
                // 创建圣水瓶
                int goldCost = static_cast<ElixirStorage*>(draggingItem->getUserData())->getGoldCost();
                int elixirCost = static_cast<ElixirStorage*>(draggingItem->getUserData())->getElixirCost();
                //再次判断资源是否足够
                if (g_goldCount >= goldCost && g_elixirCount >= elixirCost) {
                    //除旧
                    ElixirStorage* draElixirStoragePreview = static_cast<ElixirStorage*>(draggingItem->getUserData());
                    if (draElixirStoragePreview) {
                        draElixirStoragePreview->removeFromParentAndCleanup(true);
                        draggingItem->setUserData(nullptr);
                    }
                    //迎新
                    auto placedElixirStorage = ElixirStorage::create("ElixirStorageLv1.png");
                    if (placedElixirStorage) {
                        // 更新
                        placedElixirStorage->updatePosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(placedElixirStorage, 15);
                        }
                        placedBuildings.push_back(placedElixirStorage);
                        placedElixirStorage->setScale(1.1f);
                        placedElixirStorage->playSuccessBlink();
                        sendSaveBuildingRequest("ElixirStorage", snappedPos.x, snappedPos.y, 1,
                            placedElixirStorage->getHp(), placedElixirStorage->getHp(),
                            0, placedElixirStorage->getMaxStock(), 0);
                    }
                    //扣除资源
                    g_goldCount -= goldCost;
                    g_elixirCount -= elixirCost;
                }
            }
            else if (draggingItem == armyCampBtn) {
                // 创建兵营
                int goldCost = static_cast<ArmyCamp*>(draggingItem->getUserData())->getGoldCost();
                int elixirCost = static_cast<ArmyCamp*>(draggingItem->getUserData())->getElixirCost();
                //再次判断资源是否足够
                if (g_goldCount >= goldCost && g_elixirCount >= elixirCost) {
                    //除旧
                    ArmyCamp* dragArmyCampPreview = static_cast<ArmyCamp*>(draggingItem->getUserData());
                    if (dragArmyCampPreview) {
                        dragArmyCampPreview->removeFromParentAndCleanup(true);
                        draggingItem->setUserData(nullptr);
                    }
                    //迎新
                    auto placedArmyCamp = ArmyCamp::create("ArmyCampLv1.png");
                    if (placedArmyCamp) {
                        // 更新
                        placedArmyCamp->updatePosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(placedArmyCamp, 15);
                        }
                        placedBuildings.push_back(placedArmyCamp);
                        placedArmyCamp->setScale(1.1f);
                        placedArmyCamp->playSuccessBlink();
                        sendSaveBuildingRequest("ArmyCamp", snappedPos.x, snappedPos.y, 1,
                            placedArmyCamp->getHp(), placedArmyCamp->getHp(),
                            0, placedArmyCamp->getMaxStock(), 0);
                    }
                    //扣除资源
                    g_goldCount -= goldCost;
                    g_elixirCount -= elixirCost;
                }
            }
            else if (draggingItem == wallsBtn) {
                // 创建城墙
                int goldCost = static_cast<Walls*>(draggingItem->getUserData())->getGoldCost();
                int elixirCost = static_cast<Walls*>(draggingItem->getUserData())->getElixirCost();
                //再次判断资源是否足够
                if (g_goldCount >= goldCost && g_elixirCount >= elixirCost) {
                    //除旧
                    Walls* dragWallsPreview = static_cast<Walls*>(draggingItem->getUserData());
                    if (dragWallsPreview) {
                        dragWallsPreview->removeFromParentAndCleanup(true);
                        draggingItem->setUserData(nullptr);
                    }
                    //迎新
                    auto placedWalls = Walls::create("WallsLv1.png");
                    if (placedWalls) {
                        // 更新
                        placedWalls->updatePosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(placedWalls, 15);
                        }
                        placedBuildings.push_back(placedWalls);
                        placedWalls->playSuccessBlink();
                        sendSaveBuildingRequest("Walls", snappedPos.x, snappedPos.y, 1,
                            placedWalls->getHp(), placedWalls->getHp(),
                            0, 0, 0);
                    }
                    //扣除资源
                    g_goldCount -= goldCost;
                    g_elixirCount -= elixirCost;
                }
            }
            else if (draggingItem == builderHutBtn) {
                // 创建小屋
                int goldCost = static_cast<BuilderHut*>(draggingItem->getUserData())->getGoldCost();
                int elixirCost = static_cast<BuilderHut*>(draggingItem->getUserData())->getElixirCost();
                //再次判断资源是否足够
                if (g_goldCount >= goldCost && g_elixirCount >= elixirCost && hutNum < 5) {
                    //除旧
                    BuilderHut* dragBuilderHutPreview = static_cast<BuilderHut*>(draggingItem->getUserData());
                    if (dragBuilderHutPreview) {
                        dragBuilderHutPreview->removeFromParentAndCleanup(true);
                        draggingItem->setUserData(nullptr);
                    }
                    //迎新
                    auto placedBuilderhut = BuilderHut::create("BuilderHutLv1.png");
                    if (placedBuilderhut) {
                        // 更新
                        placedBuilderhut->updatePosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(placedBuilderhut, 15);
                        }
                        placedBuildings.push_back(placedBuilderhut);
                        placedBuilderhut->setScale(0.8f);
                        placedBuilderhut->playSuccessBlink();
                        sendSaveBuildingRequest("BuilderHut", snappedPos.x, snappedPos.y, 1,
                            placedBuilderhut->getHp(), placedBuilderhut->getHp(),
                            0, 0, 0);
                    }
                    //扣除资源
                    g_goldCount -= goldCost;
                    g_elixirCount -= elixirCost;
                    hutNum++;
                }
            }
            else if (draggingItem == archerTowerBtn) {
                // 创建箭塔
                int goldCost = static_cast<ArcherTower*>(draggingItem->getUserData())->getGoldCost();
                int elixirCost = static_cast<ArcherTower*>(draggingItem->getUserData())->getElixirCost();
                //再次判断资源是否足够
                if (g_goldCount >= goldCost && g_elixirCount >= elixirCost) {
                    //除旧
                    ArcherTower* dragArcherTowerPreview = static_cast<ArcherTower*>(draggingItem->getUserData());
                    if (dragArcherTowerPreview) {
                        dragArcherTowerPreview->removeFromParentAndCleanup(true);
                        draggingItem->setUserData(nullptr);
                    }
                    //迎新
                    auto placedArcherTower = ArcherTower::create("ArcherTowerLv1.png");
                    if (placedArcherTower) {
                        // 更新
                        placedArcherTower->updatePosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(placedArcherTower, 15);
                        }
                        placedBuildings.push_back(placedArcherTower);
                        placedArcherTower->setScale(0.8f);
                        placedArcherTower->playSuccessBlink();
                        sendSaveBuildingRequest("ArcherTower", snappedPos.x, snappedPos.y, 1,
                            placedArcherTower->getHp(), placedArcherTower->getHp(),
                            0, 0, 0);
                    }
                    //扣除资源
                    g_goldCount -= goldCost;
                    g_elixirCount -= elixirCost;
                }
            }
            else if (draggingItem == cannonBtn) {
                // 创建加农炮
                int goldCost = static_cast<Cannon*>(draggingItem->getUserData())->getGoldCost();
                int elixirCost = static_cast<Cannon*>(draggingItem->getUserData())->getElixirCost();
                //再次判断资源是否足够
                if (g_goldCount >= goldCost && g_elixirCount >= elixirCost) {
                    //除旧
                    Cannon* dragCannonPreview = static_cast<Cannon*>(draggingItem->getUserData());
                    if (dragCannonPreview) {
                        dragCannonPreview->removeFromParentAndCleanup(true);
                        draggingItem->setUserData(nullptr);
                    }
                    //迎新
                    auto placedCannon = Cannon::create("CannonLv1.png");
                    if (placedCannon) {
                        // 更新
                        placedCannon->updatePosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(placedCannon, 15);
                        }
                        placedBuildings.push_back(placedCannon);
                        placedCannon->setScale(1.1f);
                        placedCannon->playSuccessBlink();
                        sendSaveBuildingRequest("Cannon", snappedPos.x, snappedPos.y, 1,
                            placedCannon->getHp(), placedCannon->getHp(),
                            0, 0, 0);
                    }
                    //扣除资源
                    g_goldCount -= goldCost;
                    g_elixirCount -= elixirCost;
                }
            }
            else if (draggingItem == mortarBtn) {
                // 创建迫击炮
                int goldCost = static_cast<Mortar*>(draggingItem->getUserData())->getGoldCost();
                int elixirCost = static_cast<Mortar*>(draggingItem->getUserData())->getElixirCost();
                //再次判断资源是否足够
                if (g_goldCount >= goldCost && g_elixirCount >= elixirCost) {
                    //除旧
                    Mortar* dragMortarPreview = static_cast<Mortar*>(draggingItem->getUserData());
                    if (dragMortarPreview) {
                        dragMortarPreview->removeFromParentAndCleanup(true);
                        draggingItem->setUserData(nullptr);
                    }
                    //迎新
                    auto placedMortar = Mortar::create("MortarLv1.png");
                    if (placedMortar) {
                        // 更新
                        placedMortar->updatePosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(placedMortar, 15);
                        }
                        placedBuildings.push_back(placedMortar);
                        placedMortar->setScale(1.0f);
                        placedMortar->playSuccessBlink();
                        sendSaveBuildingRequest("Mortar", snappedPos.x, snappedPos.y, 1,
                            placedMortar->getHp(), placedMortar->getHp(),
                            0, 0, 0);
                    }
                    //扣除资源
                    g_goldCount -= goldCost;
                    g_elixirCount -= elixirCost;
                }
            }
            // 无效区域：创建对应建筑并执行失败反馈
            else {
                if (draggingItem == goldMineBtn) {
                    auto failGoldMine = GoldMine::create("GoldMineLv1.png");
                    if (failGoldMine) {
                        failGoldMine->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failGoldMine, 15);
                        }
                        failGoldMine->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == elixirCollectorBtn) {
                    auto failElixir = ElixirCollector::create("ElixirCollectorLv1.png");
                    if (failElixir) {
                        failElixir->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failElixir, 15);
                        }
                        failElixir->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == goldStorageBtn) {
                    auto failGoldStorage = GoldStorage::create("GoldStorageLv1.png");
                    if (failGoldStorage) {
                        failGoldStorage->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failGoldStorage, 15);
                        }
                        failGoldStorage->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == elixirStorageBtn) {
                    auto failElixirStorage = ElixirStorage::create("ElixirStorageLv1.png");
                    if (failElixirStorage) {
                        failElixirStorage->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failElixirStorage, 15);
                        }
                        failElixirStorage->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == armyCampBtn) {
                    auto failArmyCamp = ArmyCamp::create("ArmyCampLv1.png");
                    if (failArmyCamp) {
                        failArmyCamp->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failArmyCamp, 15);
                        }
                        failArmyCamp->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == wallsBtn) {
                    auto failWalls = Walls::create("WallsLv1.png");
                    if (failWalls) {
                        failWalls->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failWalls, 15);
                        }
                        failWalls->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == builderHutBtn) {
                    auto failBuilderHut = BuilderHut::create("BuilderHutLv1.png");
                    if (failBuilderHut) {
                        failBuilderHut->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failBuilderHut, 15);
                        }
                        failBuilderHut->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == archerTowerBtn) {
                    auto failArcherTower = ArcherTower::create("ArcherTowerLv1.png");
                    if (failArcherTower) {
                        failArcherTower->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failArcherTower, 15);
                        }
                        failArcherTower->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == cannonBtn) {
                    auto failCannon = Cannon::create("CannonLv1.png");
                    if (failCannon) {
                        failCannon->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failCannon, 15);
                        }
                        failCannon->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == mortarBtn) {
                    auto failMortar = Mortar::create("MortarLv1.png");
                    if (failMortar) {
                        failMortar->setPosition(snappedPos);
                        if (background_sprite_) {
                            background_sprite_->addChild(failMortar, 15);
                        }
                        failMortar->playFailBlinkAndRemove();
                    }
                }
            }

            // 重置拖拽状态
            isDragging = false;
            draggingItem = nullptr;
        }
    }
    else if (isMovingBuilding) {
        if (!background_sprite_) {
            isMovingBuilding = false;
            movingBuilding = nullptr;
            return;
        }

        // 检查是否可能是双击序列中的第一次点击
        double currentTime = clock() / (double)CLOCKS_PER_SEC;
        double timeDiff = currentTime - _lastClickTime;

        // 如果时间差小于双击间隔，说明可能是双击序列的第一次点击，不执行移动
        if (timeDiff < DOUBLE_CLICK_INTERVAL) {
            // 恢复建筑状态
            if (movingBuilding) {
                movingBuilding->setOpacity(255);
                background_sprite_->reorderChild(movingBuilding, 15);
                movingBuilding = nullptr;
            }
            isMovingBuilding = false;
            return;
        }

        Vec2 localPos = background_sprite_->convertToNodeSpace(touch->getLocation());
        //Vec2 localPos = ConvertTest::convertScreenToGrid(touch->getLocation(), background_sprite_, buildPanel);
        Vec2 diamondPos = convertScreenToDiamond(touch->getLocation());
        bool inDiamond = isInDiamond(diamondPos);

        if (!grid_manager_) {
            isMovingBuilding = false;
            movingBuilding = nullptr;
            return;
        }

        float gridCellSizeX = grid_manager_->getGridCellSizeX()/2;
        float gridCellSizeY = grid_manager_->getGridCellSizeY()/2;
        float snappedX = ceil(localPos.x / gridCellSizeX) * gridCellSizeX;
        float snappedY = ceil(localPos.y / gridCellSizeY) * gridCellSizeY;

        if (inDiamond && movingBuilding) {
            // 使用新的更新方法
            movingBuilding->updatePosition(Vec2(snappedX, snappedY));
            // 删除原位置记录
            sendDeleteBuildingRequest(_movingBuildingOriginalPos.x, _movingBuildingOriginalPos.y);
            // 保存建筑位置到数据库
            sendSaveBuildingRequest(movingBuilding->getBuildingType(),
                snappedX, snappedY, movingBuilding->getLv(),
                movingBuilding->getHp(), movingBuilding->getHp(),
                movingBuilding->getSpeed(), movingBuilding->getMaxStock(), 0);
            movingBuilding->setOpacity(255);
            if (background_sprite_) {
                background_sprite_->reorderChild(movingBuilding, 15);
            }
            movingBuilding = nullptr;
        }
        isMovingBuilding = false;
    }
    else if (zoom_manager_) {
        zoom_manager_->onTouchEnded(touch, event);
    }
    
}

void SecondScene::onTouchCancelled(Touch* touch, Event* event)
{
    if (isDragging && draggingItem) {
        // 移除预览对象
        if (draggingItem == goldMineBtn) {
            GoldMine* dragMinePreview = static_cast<GoldMine*>(draggingItem->getUserData());
            if (dragMinePreview) {
                dragMinePreview->removeFromParentAndCleanup(true);
                draggingItem->setUserData(nullptr);
            }
        }
        else if (draggingItem == elixirCollectorBtn) {
            ElixirCollector* dragElixirPreview = static_cast<ElixirCollector*>(draggingItem->getUserData());
            if (dragElixirPreview) {
                dragElixirPreview->removeFromParentAndCleanup(true);
                draggingItem->setUserData(nullptr);
            }
        }
        else if (draggingItem == goldStorageBtn) {
            GoldStorage* dragGoldStoragePreview = static_cast<GoldStorage*>(draggingItem->getUserData());
            if (dragGoldStoragePreview) {
                dragGoldStoragePreview->removeFromParentAndCleanup(true);
                draggingItem->setUserData(nullptr);
            }
        }
        else if (draggingItem == elixirStorageBtn) {
            ElixirStorage* dragElixirStoragePreview = static_cast<ElixirStorage*>(draggingItem->getUserData());
            if (dragElixirStoragePreview) {
                dragElixirStoragePreview->removeFromParentAndCleanup(true);
                draggingItem->setUserData(nullptr);
            }
        }
        else if (draggingItem == armyCampBtn) {
            ArmyCamp* dragArmyCampPreview = static_cast<ArmyCamp*>(draggingItem->getUserData());
            if (dragArmyCampPreview) {
                dragArmyCampPreview->removeFromParentAndCleanup(true);
                draggingItem->setUserData(nullptr);
            }
        }
        else if (draggingItem == wallsBtn) {
            Walls* dragWallsPreview = static_cast<Walls*>(draggingItem->getUserData());
            if (dragWallsPreview) {
                dragWallsPreview->removeFromParentAndCleanup(true);
                draggingItem->setUserData(nullptr);
            }
        }
        else if (draggingItem == builderHutBtn) {
            BuilderHut* dragBuilderHutPreview = static_cast<BuilderHut*>(draggingItem->getUserData());
            if (dragBuilderHutPreview) {
                dragBuilderHutPreview->removeFromParentAndCleanup(true);
                draggingItem->setUserData(nullptr);
            }
        }
        else if (draggingItem == archerTowerBtn) {
            ArcherTower* dragArcherTowerPreview = static_cast<ArcherTower*>(draggingItem->getUserData());
            if (dragArcherTowerPreview) {
                dragArcherTowerPreview->removeFromParentAndCleanup(true);
                draggingItem->setUserData(nullptr);
            }
        }
        else if (draggingItem == cannonBtn) {
            Cannon* dragCannonPreview = static_cast<Cannon*>(draggingItem->getUserData());
            if (dragCannonPreview) {
                dragCannonPreview->removeFromParentAndCleanup(true);
                draggingItem->setUserData(nullptr);
            }
        }
        else if (draggingItem == mortarBtn) {
            Mortar* dragMortarPreview = static_cast<Mortar*>(draggingItem->getUserData());
            if (dragMortarPreview) {
                dragMortarPreview->removeFromParentAndCleanup(true);
                draggingItem->setUserData(nullptr);
            }
        }
        isDragging = false;
        draggingItem = nullptr;
    }
    else if (isMovingBuilding) {
        // 恢复
        if (movingBuilding) {
            movingBuilding->setOpacity(255);
            if (background_sprite_) {
                background_sprite_->reorderChild(movingBuilding, 15);
            }
            movingBuilding = nullptr;
        }    
        isMovingBuilding = false;
    }
    else if (zoom_manager_) {
        zoom_manager_->onTouchCancelled(touch, event);
    }
}

void SecondScene::onMouseScroll(EventMouse* event)
{
    if (zoom_manager_)
    {
        zoom_manager_->onMouseScroll(event);
    }
}

void SecondScene::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
{
    // 如果正在拖拽，不执行缩放功能
    if (isDragging) {
        return;
    }
    
    if (zoom_manager_) {
        zoom_manager_->onTouchesBegan(touches, event);
    }
}

void SecondScene::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
    // 如果正在拖拽，不执行缩放功能
    if (isDragging) {
        return;
    }
    
    if (zoom_manager_) {
        zoom_manager_->onTouchesMoved(touches, event);
    }
}

void SecondScene::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    // 如果正在拖拽，不执行缩放功能
    if (isDragging) {
        return;
    }
    
    if (zoom_manager_) {
        zoom_manager_->onTouchesEnded(touches, event);
    }
}

void SecondScene::onTouchesCancelled(const std::vector<Touch*>& touches, Event* event)
{
    // 如果正在拖拽，不执行缩放功能
    if (isDragging) {
        return;
    }
    
    if (zoom_manager_) {
        zoom_manager_->onTouchesCancelled(touches, event);
    }
}


void SecondScene::updateBackgroundBounds()
{
    if (zoom_manager_)
    {
        zoom_manager_->updateBackgroundBounds();
    }
}


void SecondScene::onMouseMove(EventMouse* event)
{
    mouse_pos_ = event->getLocation();

    if (grid_manager_ && background_sprite_ && coordinate_label_)
    {
        grid_manager_->updateMouseGridPosition(mouse_pos_, background_sprite_, coordinate_label_);
    }
}

// Convert screen coordinates to diamond coordinates
cocos2d::Vec2 SecondScene::convertScreenToDiamond(const cocos2d::Vec2& screenPos)
{
    if (grid_manager_ && background_sprite_)
    {
        return grid_manager_->convertScreenToDiamond(screenPos, background_sprite_);
    }
    return Vec2::ZERO;
}

// Check if position is inside diamond
bool SecondScene::isInDiamond(const Vec2& diamondPos)
{
    // 现在由DiamondGridManager处理
    if (grid_manager_)
    {
        return grid_manager_->isInDiamond(diamondPos);
    }
    return false;
}

//判断点是否在建筑范围内
bool SecondScene::isPointInBuilding(const Vec2& point, Building* building) {
    if (!building) {
        return false;
    }

    int yourSize = building->getSize();
    int mySize = 0;
    if (draggingItem == goldMineBtn ||
        draggingItem == elixirCollectorBtn ||
        draggingItem == goldStorageBtn ||
        draggingItem == elixirStorageBtn ||
        draggingItem == armyCampBtn) {
        mySize = 3;
    }
    else if (draggingItem == wallsBtn) {
        mySize = 1;
    }
    else if (draggingItem == builderHutBtn) {
        mySize = 2;
    }
    else {
        mySize = 4;
    }

    // 计算建筑中心在背景精灵的本地坐标（和传入的point同空间）
    if (!background_sprite_) {
        return false;
    }
    Vec2 buildingWorldPos = building->convertToWorldSpace(Vec2::ZERO);
    Vec2 buildingLocalCenter = background_sprite_->convertToNodeSpace(buildingWorldPos);

    // 大菱形参数：
    // 水平对角线长 56.0* = 336 → 半长 168
    // 竖直对角线长 42.0*6 = 252 → 半长 126
    const float bigHalfHoriz = 56.0f * (yourSize + mySize) / 8;
    const float bigHalfVert = 42.0f * (yourSize + mySize) / 8;

    // 菱形碰撞判断（带浮点容错）
    auto isPointInSingleDiamond = [](const Vec2& p, const Vec2& center, float hh, float hv) -> bool {
        float dx = abs(p.x - center.x);
        float dy = abs(p.y - center.y);
        // 菱形碰撞公式：|x/x半长| + |y/y半长| ≤ 1（加容错值避免浮点精度问题）
        return (dx / hh) + (dy / hv) <= 1.0f + 0.01f;
        };

    // 直接检测大菱形区域
    if (isPointInSingleDiamond(point, buildingLocalCenter, bigHalfHoriz, bigHalfVert)) {
        return true;
    }

    // 无碰撞
    return false;
}

void SecondScene::setupWebSocketCallbacks() {
    auto wsManager = WebSocketManager::getInstance();

    wsManager->setOnMessageCallback([this](const std::string& message) {
        CCLOG("SecondScene: WebSocket callback invoked, _sceneIsDestroyed=%s",
            _sceneIsDestroyed ? "true" : "false");

        if (_sceneIsDestroyed) {
            CCLOG("SecondScene: Scene destroyed, ignoring WebSocket message");
            return;
        }

        CCLOG("SecondScene: WebSocket message received: %s", message.c_str());
        onWebSocketMessage(message);
        });

    wsManager->setOnCloseCallback([this]() {
        if (_sceneIsDestroyed)
            return;
        CCLOG("SecondScene: WebSocket disconnected");
        });

    wsManager->setOnErrorCallback([this](WebSocket::ErrorCode errorCode) {
        if (_sceneIsDestroyed)
            return;
        CCLOG("SecondScene: WebSocket error: %d", static_cast<int>(errorCode));
        });
}

void SecondScene::setupWebSocketAndRequestResources() {
    auto wsManager = WebSocketManager::getInstance();
    auto session = SessionManager::getInstance();
    std::string username = session->getCurrentUsername();

    if (username.empty()) {
        CCLOG("SecondScene: No username available");
        return;
    }

    if (wsManager->getReadyState() == WebSocket::State::OPEN) {
        CCLOG("SecondScene: WebSocket already connected, sending resource and buildings request");
        sendGetResourceRequest();
        sendGetBuildingsRequest();
    }
    else {
        CCLOG("SecondScene: WebSocket not connected, setting up open callback and connecting");
        wsManager->setOnOpenCallback([this, username]() {
            CCLOG("SecondScene: WebSocket opened for user: %s", username.c_str());
            sendGetResourceRequest();
            sendGetBuildingsRequest();
            });
        wsManager->connect("ws://100.80.250.106:8080");
    }
}

void SecondScene::onWebSocketMessage(const std::string& message) {
    CCLOG("SecondScene: onWebSocketMessage called with: %s", message.c_str());

    rapidjson::Document doc;
    doc.Parse(message.c_str());

    if (doc.HasParseError() || !doc.IsObject()) {
        CCLOG("SecondScene: Failed to parse WebSocket message as JSON");
        return;
    }

    std::string action = "";
    bool result = false;
    std::string responseMessage = "";

    if (doc.HasMember("action") && doc["action"].IsString()) {
        action = doc["action"].GetString();
    }
    if (doc.HasMember("result") && doc["result"].IsBool()) {
        result = doc["result"].GetBool();
    }
    if (doc.HasMember("message") && doc["message"].IsString()) {
        responseMessage = doc["message"].GetString();
    }

    CCLOG("SecondScene: Parsed action=%s, result=%s", action.c_str(), result ? "true" : "false");

    if (action == "getResource") {
        CCLOG("SecondScene: Processing getResource response");
        if (result) {
            bool hasGold = doc.HasMember("gold") && doc["gold"].IsInt();
            bool hasElixir = doc.HasMember("elixir") && doc["elixir"].IsInt();
            bool hasGems = doc.HasMember("gems") && doc["gems"].IsInt();

            if (hasGold && hasElixir && hasGems) {
                int gold = doc["gold"].GetInt();
                int elixir = doc["elixir"].GetInt();
                int gems = doc["gems"].GetInt();

                CCLOG("SecondScene: Parsed gold=%d, elixir=%d, gems=%d from response", gold, elixir, gems);

                g_goldCount = gold;
                g_elixirCount = elixir;
                g_gemCount = gems;

                CCLOG("SecondScene: Updated global variables - gold=%d, elixir=%d, gems=%d",
                    g_goldCount, g_elixirCount, g_gemCount);

                auto session = SessionManager::getInstance();
                session->setResourceData(gold, elixir, gems);

                CCLOG("SecondScene: Resource data loaded from server - gold=%d, elixir=%d, gems=%d",
                    g_goldCount, g_elixirCount, g_gemCount);

                // 立即更新 UI 标签显示
                CCLOG("SecondScene: goldLabel=%p, elixirLabel=%p, gemLabel=%p", goldLabel, elixirLabel, gemLabel);
                if (goldLabel) {
                    goldLabel->setString(StringUtils::format("%d", g_goldCount));
                    CCLOG("SecondScene: Updated goldLabel to %d", g_goldCount);
                }
                else {
                    CCLOG("SecondScene: goldLabel is NULL!");
                }
                if (elixirLabel) {
                    elixirLabel->setString(StringUtils::format("%d", g_elixirCount));
                    CCLOG("SecondScene: Updated elixirLabel to %d", g_elixirCount);
                }
                if (gemLabel) {
                    gemLabel->setString(StringUtils::format("%d", g_gemCount));
                    CCLOG("SecondScene: Updated gemLabel to %d", g_gemCount);
                }
            }
            else {
                CCLOG("SecondScene: Incomplete resource response - gold=%s, elixir=%s, gems=%s, keeping current values",
                    hasGold ? "present" : "missing",
                    hasElixir ? "present" : "missing",
                    hasGems ? "present" : "missing");
                CCLOG("SecondScene: Current values kept - gold=%d, elixir=%d, gems=%d",
                    g_goldCount, g_elixirCount, g_gemCount);
            }
        }
        else {
            CCLOG("SecondScene: Failed to get resource data: %s", responseMessage.c_str());
        }
    }
    else if (action == "getBuildings") {
        CCLOG("SecondScene: Processing getBuildings response");
        onWebSocketBuildingsMessage(message);
    }
    else if (action == "getBuildingProduction") {
        CCLOG("SecondScene: Processing getBuildingProduction response");
        if (result) {
            CCLOG("SecondScene: Production data loaded, applying to buildings");
            applyProductionDataToBuildings();
        }
    }
}

void SecondScene::sendGetResourceRequest() {
    auto wsManager = WebSocketManager::getInstance();
    WebSocket::State readyState = wsManager->getReadyState();

    if (readyState != WebSocket::State::OPEN) {
        CCLOG("SecondScene: WebSocket not connected, cannot send resource request");
        return;
    }

    auto session = SessionManager::getInstance();
    std::string username = session->getCurrentUsername();

    if (username.empty()) {
        CCLOG("SecondScene: No username available for resource request");
        return;
    }

    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    doc.AddMember("action", "getResource", allocator);
    doc.AddMember("username", rapidjson::Value(username.c_str(), allocator), allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    std::string message = buffer.GetString();
    if (wsManager->send(message)) {
        CCLOG("SecondScene: Resource request sent for user: %s", username.c_str());
    }
    else {
        CCLOG("SecondScene: Failed to send resource request");
    }
}

void SecondScene::sendUpdateResourceRequest(float dt) {
    auto wsManager = WebSocketManager::getInstance();
    WebSocket::State readyState = wsManager->getReadyState();

    if (readyState != WebSocket::State::OPEN) {
        CCLOG("SecondScene: WebSocket not connected, cannot send resource update request");
        return;
    }

    auto session = SessionManager::getInstance();
    std::string username = session->getCurrentUsername();

    if (username.empty()) {
        CCLOG("SecondScene: No username available for resource update");
        return;
    }

    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    doc.AddMember("action", "updateResource", allocator);
    doc.AddMember("username", rapidjson::Value(username.c_str(), allocator), allocator);
    doc.AddMember("gold", g_goldCount, allocator);
    doc.AddMember("elixir", g_elixirCount, allocator);
    doc.AddMember("gems", g_gemCount, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    std::string message = buffer.GetString();
    if (wsManager->send(message)) {
        CCLOG("SecondScene: Resource update sent for user: %s - gold=%d, elixir=%d, gems=%d",
            username.c_str(), g_goldCount, g_elixirCount, g_gemCount);
    }
    else {
        CCLOG("SecondScene: Failed to send resource update");
    }
}

void SecondScene::sendCollectProductionRequest(Building* building, int collectedAmount,
    int remainingStock, int resourceType) {
    auto wsManager = WebSocketManager::getInstance();
    WebSocket::State readyState = wsManager->getReadyState();

    if (readyState != WebSocket::State::OPEN) {
        CCLOG("SecondScene: WebSocket not connected, cannot send collect production request");
        return;
    }

    if (!building) {
        CCLOG("SecondScene: Building is null for collect production request");
        return;
    }

    auto session = SessionManager::getInstance();
    std::string username = session->getCurrentUsername();

    if (username.empty()) {
        CCLOG("SecondScene: No username available for collect production");
        return;
    }

    std::string buildingType = building->getBuildingType();
    float x = building->getX();
    float y = building->getY();

    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    doc.AddMember("action", "collectProduction", allocator);
    doc.AddMember("username", rapidjson::Value(username.c_str(), allocator), allocator);
    doc.AddMember("buildingType", rapidjson::Value(buildingType.c_str(), allocator), allocator);
    doc.AddMember("x", x, allocator);
    doc.AddMember("y", y, allocator);
    doc.AddMember("collectedAmount", collectedAmount, allocator);
    doc.AddMember("remainingStock", remainingStock, allocator);
    doc.AddMember("resourceType", resourceType, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    std::string message = buffer.GetString();
    if (wsManager->send(message)) {
        CCLOG("SecondScene: Collect production sent - user: %s, type: %s, x: %.2f, y: %.2f, collected: %d, remaining: %d",
            username.c_str(), buildingType.c_str(), x, y, collectedAmount, remainingStock);
    }
    else {
        CCLOG("SecondScene: Failed to send collect production request");
    }
}


void SecondScene::sendSaveBuildingRequest(const std::string& buildingType, float x, float y, int level,
    int hp, int maxHp, int productionRate, int maxStock, int attack) {
    auto wsManager = WebSocketManager::getInstance();
    WebSocket::State readyState = wsManager->getReadyState();

    if (readyState != WebSocket::State::OPEN) {
        CCLOG("SecondScene: WebSocket not connected, cannot send save building request");
        return;
    }

    auto session = SessionManager::getInstance();
    std::string username = session->getCurrentUsername();

    if (username.empty()) {
        CCLOG("SecondScene: No username available for save building");
        return;
    }

    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    doc.AddMember("action", "saveBuilding", allocator);
    doc.AddMember("username", rapidjson::Value(username.c_str(), allocator), allocator);
    doc.AddMember("buildingType", rapidjson::Value(buildingType.c_str(), allocator), allocator);
    doc.AddMember("x", x, allocator);
    doc.AddMember("y", y, allocator);
    doc.AddMember("level", level, allocator);
    doc.AddMember("hp", hp, allocator);
    doc.AddMember("maxHp", maxHp, allocator);
    doc.AddMember("productionRate", productionRate, allocator);
    doc.AddMember("maxStock", maxStock, allocator);
    doc.AddMember("attack", attack, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    std::string message = buffer.GetString();
    if (wsManager->send(message)) {
        CCLOG("SecondScene: Save building request sent for user: %s - type=%s, x=%.2f, y=%.2f, level=%d, hp=%d, maxHp=%d, prod=%d, maxStock=%d, attack=%d",
            username.c_str(), buildingType.c_str(), x, y, level, hp, maxHp, productionRate, maxStock, attack);
    }
    else {
        CCLOG("SecondScene: Failed to send save building request");
    }
}

void SecondScene::sendDeleteBuildingRequest(float x, float y) {
    auto wsManager = WebSocketManager::getInstance();
    WebSocket::State readyState = wsManager->getReadyState();

    if (readyState != WebSocket::State::OPEN) {
        CCLOG("SecondScene: WebSocket not connected, cannot send delete building request");
        return;
    }

    auto session = SessionManager::getInstance();
    std::string username = session->getCurrentUsername();

    if (username.empty()) {
        CCLOG("SecondScene: No username available for delete building");
        return;
    }

    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    doc.AddMember("action", "deleteBuilding", allocator);
    doc.AddMember("username", rapidjson::Value(username.c_str(), allocator), allocator);
    doc.AddMember("x", x, allocator);
    doc.AddMember("y", y, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    std::string message = buffer.GetString();
    if (wsManager->send(message)) {
        CCLOG("SecondScene: Delete building request sent for user: %s - x=%.2f, y=%.2f",
            username.c_str(), x, y);
    }
    else {
        CCLOG("SecondScene: Failed to send delete building request");
    }
}

void SecondScene::sendGetBuildingsRequest() {
    auto wsManager = WebSocketManager::getInstance();
    WebSocket::State readyState = wsManager->getReadyState();

    if (readyState != WebSocket::State::OPEN) {
        CCLOG("SecondScene: WebSocket not connected, cannot send get buildings request");
        return;
    }

    auto session = SessionManager::getInstance();
    std::string username = session->getCurrentUsername();

    if (username.empty()) {
        CCLOG("SecondScene: No username available for get buildings");
        return;
    }

    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    doc.AddMember("action", "getBuildings", allocator);
    doc.AddMember("username", rapidjson::Value(username.c_str(), allocator), allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    std::string message = buffer.GetString();
    if (wsManager->send(message)) {
        CCLOG("SecondScene: Get buildings request sent for user: %s", username.c_str());
    }
    else {
        CCLOG("SecondScene: Failed to send get buildings request");
    }
}

void SecondScene::onWebSocketBuildingsMessage(const std::string& message) {
    CCLOG("SecondScene: Buildings message received, length=%zu", message.length());

    if (_sceneIsDestroyed) {
        CCLOG("SecondScene: Scene destroyed, ignoring buildings message");
        return;
    }

    rapidjson::Document doc;
    rapidjson::ParseResult parseResult = doc.Parse(message.c_str());
    if (!parseResult) {
        CCLOG("SecondScene: Failed to parse buildings JSON, error offset=%u", parseResult.Offset());
        return;
    }
    CCLOG("SecondScene: Buildings JSON parsed successfully");

    std::string action;
    bool result = false;
    std::string responseMessage;

    if (doc.HasMember("action") && doc["action"].IsString()) {
        action = doc["action"].GetString();
    }
    if (doc.HasMember("result") && doc["result"].IsBool()) {
        result = doc["result"].GetBool();
    }
    if (doc.HasMember("message") && doc["message"].IsString()) {
        responseMessage = doc["message"].GetString();
    }

    CCLOG("SecondScene: Buildings parsed action=%s, result=%s", action.c_str(), result ? "true" : "false");

    if (action == "getBuildings") {
        if (result && doc.HasMember("buildings") && doc["buildings"].IsArray()) {
            const rapidjson::Value& buildingsArray = doc["buildings"];
            int totalBuildings = static_cast<int>(buildingsArray.Size());

            if (totalBuildings > 0) {
                CCLOG("SecondScene: Loading %d buildings from server", totalBuildings);

                if (totalBuildings > 10) {
                    CCLOG("SecondScene: Large number of buildings, using async creation");
                    createBuildingsAsync(buildingsArray);
                }
                else {
                    CCLOG("SecondScene: Small number of buildings, using sync creation");
                    createBuildingsSync(buildingsArray);
                }
                return;
            }
        }

        if (_buildingsInitialized) {
            CCLOG("SecondScene: Buildings already initialized, skipping");
            return;
        }

        if (_sceneIsDestroyed) {
            CCLOG("SecondScene: Scene destroyed, skipping initialization");
            return;
        }

        CCLOG("SecondScene: Account login - no buildings from server (unexpected case)");
    }
}

void SecondScene::createBuildingsSync(const rapidjson::Value& buildingsArray) {
    if (_sceneIsDestroyed) return;

    int createdCount = 0;
    for (rapidjson::SizeType i = 0; i < buildingsArray.Size(); i++) {
        if (_sceneIsDestroyed) break;

        const rapidjson::Value& building = buildingsArray[i];

                if (!building.HasMember("type") || !building["type"].IsString() ||
                    !building.HasMember("x") || !building["x"].IsNumber() ||
                    !building.HasMember("y") || !building["y"].IsNumber() ||
                    !building.HasMember("level") || !building["level"].IsInt()) {
                    CCLOG("SecondScene: Invalid building data at index %d", i);
                    continue;
                }

                std::string buildingType = building["type"].GetString();
                float x = static_cast<float>(building["x"].GetDouble());
                float y = static_cast<float>(building["y"].GetDouble());
                int level = building["level"].GetInt();
                int hp = building.HasMember("hp") ? building["hp"].GetInt() : 100;
                int maxHp = building.HasMember("maxHp") ? building["maxHp"].GetInt() : hp;
                int productionRate = building.HasMember("productionRate") ? building["productionRate"].GetInt() : 1;
                int maxStock = building.HasMember("maxStock") ? building["maxStock"].GetInt() : 100;
                int attack = building.HasMember("attack") ? building["attack"].GetInt() : 0;

                CCLOG("SecondScene: Creating building - type=%s, x=%.2f, y=%.2f, level=%d, hp=%d, maxHp=%d",
                    buildingType.c_str(), x, y, level, hp, maxHp);

                Building* newBuilding = createBuildingByType(buildingType, x, y,
                    level, hp, maxHp, productionRate, maxStock, attack);
                if (newBuilding) {
                    newBuilding->updatePosition(Vec2(x, y));

                    if (background_sprite_) {
                        background_sprite_->addChild(newBuilding, 15);
                    }
                    placedBuildings.push_back(newBuilding);
                    createdCount++;
                    CCLOG("SecondScene: Building placed successfully (count=%d)", createdCount);
                }
                else {
                    CCLOG("SecondScene: Failed to create building of type: %s", buildingType.c_str());
                }
    }

    CCLOG("SecondScene: Sync creation complete, %d buildings created", createdCount);
    _buildingsInitialized = true;
}

void SecondScene::createBuildingsAsync(const rapidjson::Value& buildingsArray) {
    if (_sceneIsDestroyed) return;

    int totalBuildings = static_cast<int>(buildingsArray.Size());
    int currentIndex = 0;

    CCLOG("SecondScene: Starting async building creation, total=%d", totalBuildings);

    auto createNextBatch = [this, &buildingsArray, totalBuildings, currentIndex]() mutable {
        if (_sceneIsDestroyed) {
            CCLOG("SecondScene: Async creation aborted - scene destroyed");
            return;
        }

        if (currentIndex >= totalBuildings) {
            CCLOG("SecondScene: Async creation complete, all buildings created");
            _buildingsInitialized = true;
            return;
        }

        int batchEnd = std::min(currentIndex + 5, totalBuildings);
        int createdInBatch = 0;

        for (int i = currentIndex; i < batchEnd; i++) {
            if (_sceneIsDestroyed) break;

            const rapidjson::Value& building = buildingsArray[i];

            if (!building.HasMember("type") || !building["type"].IsString() ||
                !building.HasMember("x") || !building["x"].IsNumber() ||
                !building.HasMember("y") || !building["y"].IsNumber() ||
                !building.HasMember("level") || !building["level"].IsInt()) {
                CCLOG("SecondScene: Invalid building data at index %d", i);
                continue;
            }

            std::string buildingType = building["type"].GetString();
            float x = static_cast<float>(building["x"].GetDouble());
            float y = static_cast<float>(building["y"].GetDouble());
            int level = building["level"].GetInt();
            int hp = building.HasMember("hp") ? building["hp"].GetInt() : 100;
            int maxHp = building.HasMember("maxHp") ? building["maxHp"].GetInt() : hp;
            int productionRate = building.HasMember("productionRate") ? building["productionRate"].GetInt() : 1;
            int maxStock = building.HasMember("maxStock") ? building["maxStock"].GetInt() : 100;
            int attack = building.HasMember("attack") ? building["attack"].GetInt() : 0;

            CCLOG("SecondScene: Async creating building %d/%d - type=%s, level=%d",
                i + 1, totalBuildings, buildingType.c_str(), level);

            Building* newBuilding = createBuildingByType(buildingType, x, y,
                level, hp, maxHp, productionRate, maxStock, attack);
            if (newBuilding) {
                newBuilding->updatePosition(Vec2(x, y));

                if (background_sprite_) {
                    background_sprite_->addChild(newBuilding, 15);
                }
                placedBuildings.push_back(newBuilding);
                createdInBatch++;
                CCLOG("SecondScene: Async building placed (batch progress: %d/%d)",
                    i + 1, batchEnd);
            }
            else {
                CCLOG("SecondScene: Failed to create building of type: %s", buildingType.c_str());
            }
        }

        currentIndex = batchEnd;
        CCLOG("SecondScene: Batch complete, %d/%d buildings created", currentIndex, totalBuildings);

        std::function<void(float)> nextBatchCallback;
        nextBatchCallback = [this, &nextBatchCallback, &buildingsArray, totalBuildings, currentIndex](float dt) mutable {
            if (_sceneIsDestroyed) {
                CCLOG("SecondScene: Async creation aborted - scene destroyed");
                return;
            }

            if (currentIndex >= totalBuildings) {
                CCLOG("SecondScene: Async creation complete, all buildings created");
                _buildingsInitialized = true;
                return;
            }

            int batchEnd = std::min(currentIndex + 5, totalBuildings);
            int createdInBatch = 0;

            for (int i = currentIndex; i < batchEnd; i++) {
                if (_sceneIsDestroyed) break;

                const rapidjson::Value& building = buildingsArray[i];

                if (!building.HasMember("type") || !building["type"].IsString() ||
                    !building.HasMember("x") || !building["x"].IsNumber() ||
                    !building.HasMember("y") || !building["y"].IsNumber() ||
                    !building.HasMember("level") || !building["level"].IsInt()) {
                    CCLOG("SecondScene: Invalid building data at index %d", i);
                    continue;
                }

                std::string buildingType = building["type"].GetString();
                float x = static_cast<float>(building["x"].GetDouble());
                float y = static_cast<float>(building["y"].GetDouble());
                int level = building["level"].GetInt();
                int hp = building.HasMember("hp") ? building["hp"].GetInt() : 100;
                int maxHp = building.HasMember("maxHp") ? building["maxHp"].GetInt() : hp;
                int productionRate = building.HasMember("productionRate") ? building["productionRate"].GetInt() : 1;
                int maxStock = building.HasMember("maxStock") ? building["maxStock"].GetInt() : 100;
                int attack = building.HasMember("attack") ? building["attack"].GetInt() : 0;

                CCLOG("SecondScene: Async creating building %d/%d - type=%s",
                    i + 1, totalBuildings, buildingType.c_str());

                Building* newBuilding = createBuildingByType(buildingType, x, y, level,
                    hp, maxHp, productionRate, maxStock, attack);
                if (newBuilding) {
                    newBuilding->updatePosition(Vec2(x, y));

                    if (background_sprite_) {
                        background_sprite_->addChild(newBuilding, 15);
                    }
                    placedBuildings.push_back(newBuilding);
                    createdInBatch++;
                    CCLOG("SecondScene: Async building placed (batch progress: %d/%d)",
                        i + 1, batchEnd);
                }
                else {
                    CCLOG("SecondScene: Failed to create building of type: %s", buildingType.c_str());
                }
            }

            currentIndex = batchEnd;
            CCLOG("SecondScene: Batch complete, %d/%d buildings created", currentIndex, totalBuildings);

            std::function<void(float)> nextBatchCallback;
            nextBatchCallback = [this, &nextBatchCallback, &buildingsArray, totalBuildings, currentIndex](float dt) mutable {
                if (_sceneIsDestroyed) {
                    CCLOG("SecondScene: Async creation aborted - scene destroyed");
                    return;
                }

                if (currentIndex >= totalBuildings) {
                    CCLOG("SecondScene: Async creation complete, all buildings created");
                    _buildingsInitialized = true;
                    return;
                }

                int batchEnd = std::min(currentIndex + 5, totalBuildings);
                int createdInBatch = 0;

                for (int i = currentIndex; i < batchEnd; i++) {
                    if (_sceneIsDestroyed) break;

                    const rapidjson::Value& building = buildingsArray[i];

                    if (!building.HasMember("type") || !building["type"].IsString() ||
                        !building.HasMember("x") || !building["x"].IsNumber() ||
                        !building.HasMember("y") || !building["y"].IsNumber() ||
                        !building.HasMember("level") || !building["level"].IsInt()) {
                        CCLOG("SecondScene: Invalid building data at index %d", i);
                        continue;
                    }

                    std::string buildingType = building["type"].GetString();
                    float x = static_cast<float>(building["x"].GetDouble());
                    float y = static_cast<float>(building["y"].GetDouble());
                    int level = building["level"].GetInt();
                    int hp = building.HasMember("hp") ? building["hp"].GetInt() : 100;
                    int maxHp = building.HasMember("maxHp") ? building["maxHp"].GetInt() : hp;
                    int productionRate = building.HasMember("productionRate") ? building["productionRate"].GetInt() : 1;
                    int maxStock = building.HasMember("maxStock") ? building["maxStock"].GetInt() : 100;
                    int attack = building.HasMember("attack") ? building["attack"].GetInt() : 0;

                    CCLOG("SecondScene: Async creating building %d/%d - type=%s, level=%d",
                        i + 1, totalBuildings, buildingType.c_str(), level);

                    Building* newBuilding = createBuildingByType(buildingType, x, y, level, 
                        hp, maxHp, productionRate, maxStock, attack);
                    if (newBuilding) {
                        newBuilding->updatePosition(Vec2(x, y));

                        if (background_sprite_) {
                            background_sprite_->addChild(newBuilding, 15);
                        }
                        placedBuildings.push_back(newBuilding);
                        createdInBatch++;
                        CCLOG("SecondScene: Async building placed (batch progress: %d/%d)",
                            i + 1, batchEnd);
                    }
                    else {
                        CCLOG("SecondScene: Failed to create building of type: %s", buildingType.c_str());
                    }
                }

                currentIndex = batchEnd;
                CCLOG("SecondScene: Batch complete, %d/%d buildings created", currentIndex, totalBuildings);

                Director::getInstance()->getScheduler()->schedule([nextBatchCallback](float dt) {
                    nextBatchCallback(dt);
                    }, this, 0.0f, 0, 0.05f, false, "asyncCreateBuildings");
                };

            Director::getInstance()->getScheduler()->schedule(nextBatchCallback, this, 0.0f, 0, 0.05f, false, "asyncCreateBuildings");
            };
        };
}

Building* SecondScene::createBuildingByType(const std::string& buildingType, float x, float y, int level,
    int hp, int maxHp, int productionRate, int maxStock, int attack) {
    if (buildingType == "GoldMine") {
        GoldMine* building = GoldMine::create(StringUtils::format("GoldMineLv%d.png", level), hp, level, x, y);
        if (building) {
            building->setHp(maxHp);
        }
        return building;
    }
    else if (buildingType == "ElixirCollector") {
        ElixirCollector* building = ElixirCollector::create(StringUtils::format("ElixirCollectorLv%d.png", 
            level), hp, level, x, y);
        if (building) {
            building->setHp(maxHp);
        }
        return building;
    }
    else if (buildingType == "GoldStorage") {
        GoldStorage* building = GoldStorage::create(StringUtils::format("GoldStorageLv%d.png", level),
            hp, level, x, y);
        if (building) {
            building->setHp(maxHp);
        }
        return building;
    }
    else if (buildingType == "ElixirStorage") {
        ElixirStorage* building = ElixirStorage::create(StringUtils::format("ElixirStorageLv%d.png", level),
            hp, level, x, y);
        if (building) {
            building->setHp(maxHp);
        }
        return building;
    }
    else if (buildingType == "ArmyCamp") {
        ArmyCamp* building = ArmyCamp::create(StringUtils::format("ArmyCampLv%d.png", level),
            hp, level, x, y);
        if (building) {
            building->setHp(maxHp);
        }
        return building;
    }
    else if (buildingType == "Walls") {
        Walls* building = Walls::create(StringUtils::format("WallsLv%d.png", level), hp, 
            level, x, y);
        if (building) {
            building->setHp(maxHp);
        }
        return building;
    }
    else if (buildingType == "BuilderHut") {
        BuilderHut* building = BuilderHut::create(StringUtils::format("BuilderHutLv%d.png", level), 
            hp, level, x, y);
        if (building) {
            building->setHp(maxHp);
        }
        return building;
    }
    else if (buildingType == "TownHall") {
        TownHall* building = TownHall::create(StringUtils::format("TownHallLv%d.png", level), 
            hp, level, x, y);
        if (building) {
            building->setHp(maxHp);
        }
        return building;
    }
    else if (buildingType == "Mortar") {
        Walls* building = Walls::create(StringUtils::format("MortarLv%d.png", level), hp,
            level, x, y);
        if (building) {
            building->setHp(maxHp);
        }
        return building;
    }
    else if (buildingType == "ArcherTower") {
        Walls* building = Walls::create(StringUtils::format("ArcherTowerLv%d.png", level), hp,
            level, x, y);
        if (building) {
            building->setHp(maxHp);
        }
        return building;
    }
    else if (buildingType == "Cannon") {
        Walls* building = Walls::create(StringUtils::format("CannonLv%d.png", level), hp,
            level, x, y);
        if (building) {
            building->setHp(maxHp);
        }
        return building;
    }
    return nullptr;
}

bool SecondScene::trySyncProductionDataToBuilding(Building* building) {
    if (!building) return false;

    std::string btype = building->getBuildingType();
    float bx = building->getX();
    float by = building->getY();

    std::vector<ProductionData> productions;
    if (!SessionManager::getInstance()->getProductionData(productions)) {
        CCLOG("SecondScene: trySyncProductionDataToBuilding - No production data available");
        return false;
    }

    for (const auto& prod : productions) {
        if (prod.buildingType == btype &&
            fabs(prod.x - bx) < 1.0f &&
            fabs(prod.y - by) < 1.0f) {
            building->addCurrent(prod.currentStock);
            CCLOG("SecondScene: trySyncProductionDataToBuilding success - type=%s, x=%.2f, y=%.2f, stock=%d",
                btype.c_str(), bx, by, prod.currentStock);
            return true;
        }
    }

    CCLOG("SecondScene: trySyncProductionDataToBuilding - No matching production data for %s at (%.2f, %.2f)",
        btype.c_str(), bx, by);
    return false;
}

void SecondScene::applyProductionDataToBuildings() {
    CCLOG("SecondScene: applyProductionDataToBuildings called");

    std::vector<ProductionData> productions;
    if (!SessionManager::getInstance()->getProductionData(productions)) {
        CCLOG("SecondScene: No production data available in SessionManager, scheduling retry");
        this->unschedule("retryApplyProductionData");
        this->scheduleOnce([this](float dt) {
            if (_sceneIsDestroyed) {
                CCLOG("SecondScene: Scene destroyed, skipping retry");
                return;
            }
            CCLOG("SecondScene: Retry applying production data");
            applyProductionDataToBuildings();
            }, 0.3f, "retryApplyProductionData");
        return;
    }

    CCLOG("SecondScene: Applying production data to %zu buildings, productions count: %zu",
        placedBuildings.size(), productions.size());

    if (productions.empty()) {
        CCLOG("SecondScene: Production data is empty array");
        return;
    }

    int appliedCount = 0;
    for (auto building : placedBuildings) {
        if (!building) continue;

        std::string btype = building->getBuildingType();

        float bx = building->getX();
        float by = building->getY();

        CCLOG("SecondScene: Checking building - type=%s, x=%.2f, y=%.2f",
            btype.c_str(), bx, by);

        for (const auto& prod : productions) {
            if (prod.buildingType == btype &&
                fabs(prod.x - bx) < 1.0f &&
                fabs(prod.y - by) < 1.0f) {
                building->addCurrent(prod.currentStock);
                CCLOG("SecondScene: Applied production data - type=%s, x=%.2f, y=%.2f, stock=%d",
                    btype.c_str(), bx, by, prod.currentStock);
                appliedCount++;
                break;
            }
        }
    }

    CCLOG("SecondScene: Applied production data to %d buildings", appliedCount);
}

void SecondScene::initDefaultBuildingsAndSave() {
    _buildingsInitialized = true;

    auto townHall = TownHall::create("TownHallLv1.png");
    if (townHall) {
        townHall->updatePosition(Vec2(1918, 1373));
        if (background_sprite_) {
            background_sprite_->addChild(townHall, 15);
        }
        placedBuildings.push_back(townHall);
        townHall->setScale(0.9f);
        maxGoldVolum = townHall->getMaxGoldNum();
        maxElixirVolum = townHall->getMaxElixirNum();
        maxLevel = townHall->getLv();
    }

    auto goldMine = GoldMine::create("GoldMineLv1.png");
    if (goldMine) {
        goldMine->updatePosition(Vec2(1918, 1625));
        if (background_sprite_) {
            background_sprite_->addChild(goldMine, 15);
        }
        placedBuildings.push_back(goldMine);
        goldMine->setScale(0.7f);
    }

    auto elixirCollector = ElixirCollector::create("ElixirCollectorLv1.png");
    if (elixirCollector) {
        elixirCollector->updatePosition(Vec2(1918, 1121));
        if (background_sprite_) {
            background_sprite_->addChild(elixirCollector, 15);
        }
        placedBuildings.push_back(elixirCollector);
        elixirCollector->setScale(0.9f);
    }

    auto goldStorage = GoldStorage::create("GoldStorageLv1.png");
    if (goldStorage) {
        goldStorage->updatePosition(Vec2(1918, 869));
        if (background_sprite_) {
            background_sprite_->addChild(goldStorage, 15);
        }
        placedBuildings.push_back(goldStorage);
        goldStorage->setScale(0.7f);
    }

    auto elixirStorage = GoldStorage::create("ElixirStorageLv1.png");
    if (elixirStorage) {
        elixirStorage->updatePosition(Vec2(1918, 1877));
        if (background_sprite_) {
            background_sprite_->addChild(elixirStorage, 15);
        }
        placedBuildings.push_back(elixirStorage);
        elixirStorage->setScale(1.1f);
    }

    auto armyCamp = ArmyCamp::create("ArmyCampLv1.png");
    if (armyCamp) {
        armyCamp->updatePosition(Vec2(2142, 1625));
        if (background_sprite_) {
            background_sprite_->addChild(armyCamp, 15);
        }
        placedBuildings.push_back(armyCamp);
        armyCamp->setScale(1.1f);
    }

    auto builderHut1 = BuilderHut::create("BuilderHutLv1.png");
    if (builderHut1) {
        builderHut1->updatePosition(Vec2(1638, 1373));
        if (background_sprite_) {
            background_sprite_->addChild(builderHut1, 15);
        }
        placedBuildings.push_back(builderHut1);
        builderHut1->setScale(0.9f);
    }

    auto builderHut2 = BuilderHut::create("BuilderHutLv1.png");
    if (builderHut2) {
        builderHut2->updatePosition(Vec2(2198, 1373));
        if (background_sprite_) {
            background_sprite_->addChild(builderHut2, 15);
        }
        placedBuildings.push_back(builderHut2);
        builderHut2->setScale(0.9f);
    }
}

void SecondScene::onEnter() {
    Scene::onEnter();
    _sceneIsDestroyed = false;
    setupWebSocketCallbacks();

    auto eventDispatcher = Director::getInstance()->getEventDispatcher();
    _upgradeCompleteListener = eventDispatcher->addCustomEventListener("upgrade_complete",
        CC_CALLBACK_1(SecondScene::handleUpgradeCompleteEvent, this));

    auto sessionManager = SessionManager::getInstance();
    if (sessionManager->isAccountLogin()) {
        CCLOG("SecondScene: onEnter - user logged in, requesting resources");
        this->scheduleOnce([this](float dt) {
            auto wsManager = WebSocketManager::getInstance();
            if (wsManager->getReadyState() == WebSocket::State::OPEN) {
                sendGetResourceRequest();
                sendGetBuildingsRequest();
                wsManager->syncProductionData();
            }
            else {
                CCLOG("SecondScene: WebSocket not open in onEnter, connecting...");
                setupWebSocketAndRequestResources();
            }
            }, 0.5f, "requestResourceDelay");

        this->scheduleOnce([this](float dt) {
            CCLOG("SecondScene: Delayed production data sync check");
            applyProductionDataToBuildings();
            }, 0.5f, "syncProductionDataFinal");

        this->schedule([this](float dt) {
            sendUpdateResourceRequest(dt);
            }, 0.5f, "resourceUpdateInterval");
    }
}

void SecondScene::onExit() {
    CCLOG("SecondScene: Exiting, cleaning up...");
    this->unschedule("requestResourceDelay");
    this->unschedule("resourceUpdateInterval");
    this->unschedule("syncProductionDataFinal");
    this->unschedule("retryApplyProductionData");
    this->unschedule("asyncCreateBuildings");
    this->unscheduleAllCallbacks();
    _sceneIsDestroyed = true;

    auto eventDispatcher = Director::getInstance()->getEventDispatcher();
    eventDispatcher->removeEventListener(_upgradeCompleteListener);

    if (_curOpenInfoPanel) {
        _curOpenInfoPanel->removeFromParent();
        _curOpenInfoPanel = nullptr;
    }
    _curOpenBuilding = nullptr;

    if (_curOpenInfoPanel) {
        _curOpenInfoPanel->removeFromParent();
        _curOpenInfoPanel = nullptr;
    }
    _curOpenBuilding = nullptr;

    if (background_sprite_) {
        background_sprite_->removeAllChildren();
    }

    placedBuildings.clear();

    if (background_sprite_) {
        background_sprite_->removeFromParent();
        background_sprite_ = nullptr;
    }

    Scene::onExit();
}

void SecondScene::handleUpgradeCompleteEvent(cocos2d::EventCustom* event) {
    if (_sceneIsDestroyed) {
        return;
    }

    UpgradeCompleteData* data = static_cast<UpgradeCompleteData*>(event->getUserData());
    if (data) {
        onUpgradeComplete(data);
        delete data;
    }
}

void SecondScene::onUpgradeComplete(UpgradeCompleteData* data) {
    CCLOG("SecondScene: Upgrade completed - buildingType=%s, x=%.1f, y=%.1f, newLevel=%d",
        data->buildingType.c_str(), data->x, data->y, data->newLevel);

    for (auto building : placedBuildings) {
        if (building &&
            std::abs(building->getX() - data->x) < 0.1f &&
            std::abs(building->getY() - data->y) < 0.1f) {

            building->setIsUpgrade(false);

            CCLOG("SecondScene: Updated building %s to level %d",
                building->getBuildingType().c_str(), data->newLevel);

            if (_curOpenInfoPanel && _curOpenBuilding == building) {
                _curOpenInfoPanel->refreshUpgradeButton(building);
            }

            std::string oldImage = StringUtils::format("%sLv1.png", building->getBuildingType().c_str());
            std::string newImage = StringUtils::format("%sLv%d.png", building->getBuildingType().c_str(), data->newLevel);

            if (FileUtils::getInstance()->isFileExist(newImage)) {
                building->updateTexture(newImage);
                CCLOG("SecondScene: Updated building image from %s to %s",
                    oldImage.c_str(), newImage.c_str());
            }
            else {
                CCLOG("SecondScene: New image %s not found, keeping current image", newImage.c_str());
            }

            break;
        }
    }

    auto sessionManager = SessionManager::getInstance();
    std::vector<UpgradeData> allUpgrades;
    if (sessionManager->getUpgradeData(allUpgrades)) {
        std::vector<UpgradeData> remainingUpgrades;
        for (const auto& upgrade : allUpgrades) {
            if (!(std::abs(upgrade.x - data->x) < 0.1f && std::abs(upgrade.y - data->y) < 0.1f)) {
                remainingUpgrades.push_back(upgrade);
            }
        }
        sessionManager->setUpgradeData(remainingUpgrades);
        CCLOG("SecondScene: Cleared completed upgrade from session data, remaining: %zu", remainingUpgrades.size());
    }
}

#endif