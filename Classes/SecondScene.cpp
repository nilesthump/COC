#if 1
#include "HelloWorldScene.h"
#include "SecondScene.h"
#include "ConvertTest.h"
#include "cocos2d.h"
#include <cmath>
#include<ctime>

// 初始化全局变量
int maxLevel,maxGoldVolum,maxElixirVolum;
int g_elixirCount = 750,g_goldCount = 750;
int g_gemCount = 15, hutNum = 2;

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
    // 初始化产金相关变量
    g_goldCount = 750; // 确保金币计数初始化为0
    g_elixirCount = 750;
    g_gemCount = 15;

    // 初始化拖拽状态
    isDragging = false;
    draggingItem = nullptr;

    // 初始化建筑移动相关变量
    isMovingBuilding = false;
    movingBuilding = nullptr;

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


    // 大本营
    auto townHall = TownHall::create("TownHallLv1.png");
    if (townHall)
    {
        townHall->updatePosition(Vec2(1918,1373));
        background_sprite_->addChild(townHall, 15);
        placedBuildings.push_back(townHall);
        townHall->setScale(0.9f);

        maxGoldVolum = townHall->getMaxGoldNum();
        maxElixirVolum = townHall->getMaxElixirNum();
        maxLevel = townHall->getLv();
    }

    //建筑小屋*2
    auto builderHut1 = BuilderHut::create("BuilderHutLv1.png");
    if (builderHut1)
    {
        builderHut1->updatePosition(Vec2(1600, 1373));
        background_sprite_->addChild(builderHut1, 15);
        placedBuildings.push_back(builderHut1);
       
    }
    auto builderHut2 = BuilderHut::create("BuilderHutLv1.png");
    if (builderHut2) {
        builderHut2->updatePosition(Vec2(2200, 1373));
        background_sprite_->addChild(builderHut2, 15);
        placedBuildings.push_back(builderHut2);
    }

    
    //53-100 总按钮部分
    auto backItem = MenuItemImage::create("btn_normal.png", "btn_pressed.png",
        CC_CALLBACK_1(SecondScene::menuFirstCallback, this));
    if (backItem == nullptr ||
        backItem->getContentSize().width <= 0 ||
        backItem->getContentSize().height <= 0)
    {
        problemLoading("'btn_normal.png' and 'btn_pressed.png'");
    }
    else
    {
        double x = origin.x + backItem->getContentSize().width / 2;
        double y = origin.y + visibleSize.height - backItem->getContentSize().height / 2;
        backItem->setPosition(Vec2(x, y));

        auto backLabel = Label::createWithSystemFont("BACK", "fonts/Marker Felt.ttf", 24);
        backLabel->setColor(Color3B::WHITE);
        backLabel->setPosition(Vec2(backItem->getContentSize().width / 2, backItem->getContentSize().height / 2));
        backItem->addChild(backLabel);
    }

    auto buildItem = MenuItemImage::create("btn_normal.png", "btn_pressed.png",
        CC_CALLBACK_1(SecondScene::menuBuildCallback, this));
    if (buildItem == nullptr ||
        buildItem->getContentSize().width <= 0 ||
        buildItem->getContentSize().height <= 0)

    {
        problemLoading("'btn_normal.png' and 'btn_pressed.png'");
    }
    else
    {
        double x = origin.x + buildItem->getContentSize().width / 2;
        double y = origin.y + visibleSize.height - buildItem->getContentSize().height*1.5;
        buildItem->setPosition(Vec2(x, y));

        auto buildLabel = Label::createWithSystemFont("BUILD", "fonts/Marker Felt.ttf", 24);
        buildLabel->setColor(Color3B::WHITE);
        buildLabel->setPosition(Vec2(buildItem->getContentSize().width / 2, buildItem->getContentSize().height / 2));
        buildItem->addChild(buildLabel);
    }

    auto attackItem1 = MenuItemImage::create("btn_normal.png", "btn_pressed.png",
        CC_CALLBACK_1(SecondScene::menuBuildCallback, this));
    if (attackItem1 == nullptr ||
        attackItem1->getContentSize().width <= 0 ||
        attackItem1->getContentSize().height <= 0)

    {
        problemLoading("'btn_normal.png' and 'btn_pressed.png'");
    }
    else 
    {
        double x = origin.x + visibleSize.width- attackItem1->getContentSize().width/2;
        double y = origin.y + attackItem1->getContentSize().height / 2;
        attackItem1->setPosition(Vec2(x, y));

        auto attackLabel = Label::createWithSystemFont("Boss1", "fonts/Marker Felt.ttf", 24);
        attackLabel->setColor(Color3B::WHITE);
        attackLabel->setPosition(Vec2(attackItem1->getContentSize().width / 2, attackItem1->getContentSize().height / 2));
        attackItem1->addChild(attackLabel);
    }

    auto attackItem2 = MenuItemImage::create("btn_normal.png", "btn_pressed.png",
        CC_CALLBACK_1(SecondScene::menuBuildCallback, this));
    if (attackItem2 == nullptr ||
        attackItem2->getContentSize().width <= 0 ||
        attackItem2->getContentSize().height <= 0)

    {
        problemLoading("'btn_normal.png' and 'btn_pressed.png'");
    }
    else
    {
        double x = origin.x + visibleSize.width - attackItem2->getContentSize().width *1.5;
        double y = origin.y + attackItem2->getContentSize().height / 2;
        attackItem2->setPosition(Vec2(x, y));

        auto attackLabel = Label::createWithSystemFont("Boss2", "fonts/Marker Felt.ttf", 24);
        attackLabel->setColor(Color3B::WHITE);
        attackLabel->setPosition(Vec2(attackItem2->getContentSize().width / 2, attackItem2->getContentSize().height / 2));
        attackItem2->addChild(attackLabel);
    }

    auto menu = Menu::create(backItem, buildItem, attackItem1,attackItem2, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    
    //101-257 建筑菜单
    buildPanel = Node::create();

    double buildPanelX = buildItem->getContentSize().width;
    double buildPanelY = 0;
    buildPanel->setPosition(Vec2(buildPanelX, buildPanelY));
    buildPanel->setVisible(false);
    buildItem->addChild(buildPanel, 1);

    auto panelBg = Sprite::create("3.png");
    if (panelBg == nullptr) {
        problemLoading("'3.png'");
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
                    // goldMinePreview->setScale(0.5f); // 如需缩放可加

                    // 计算预览初始位置（和原来的逻辑一致）
                    //Vec2 worldPos = goldMineBtn->getParent()->convertToWorldSpace(goldMineBtn->getPosition());
                    //Vec2 localPos = background_sprite_->convertToNodeSpace(worldPos);
                    //goldMinePreview->setMinePosition(Vec2(goldMinePreview->getX(), goldMinePreview->getY()));
                    Vec2 my = Vec2(goldMinePreview->getX(), goldMinePreview->getY());
                    Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                    goldMinePreview->setMinePosition(you);
                    // 添加到背景精灵，并保存到按钮的UserData
                    background_sprite_->addChild(goldMinePreview, 10);
                    goldMineBtn->setUserData(goldMinePreview);
                }

                
            }
        }
    );
    if (goldMineBtn) {
        goldMineBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 / 2 - 20));
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
                    Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                    elixirCollectorPreview->setMinePosition(you);

                    // 添加到背景精灵，并保存到按钮的UserData
                    background_sprite_->addChild(elixirCollectorPreview, 10);
                    elixirCollectorBtn->setUserData(elixirCollectorPreview);
                }
            }
        }
    );
    if (elixirCollectorBtn) {
        elixirCollectorBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 * 1.5 - 20));
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
                    Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                    goldStoragePreview->setMinePosition(you);

                    // 添加到背景精灵，并保存到按钮的UserData
                    background_sprite_->addChild(goldStoragePreview, 10);
                    goldStorageBtn->setUserData(goldStoragePreview);
                }
            }
        }
    );
    if (goldStorageBtn) {
        goldStorageBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 * 2.5 - 20));
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
                    Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                    elixirStoragePreview->setMinePosition(you);

                    // 添加到背景精灵，并保存到按钮的UserData
                    background_sprite_->addChild(elixirStoragePreview, 10);
                    elixirStorageBtn->setUserData(elixirStoragePreview);
                }
            }
        }
    );
    if (elixirStorageBtn) {
        elixirStorageBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 * 3.5 - 20));
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
                    Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                    armyCampPreview->setMinePosition(you);

                    // 添加到背景精灵，并保存到按钮的UserData
                    background_sprite_->addChild(armyCampPreview, 10);
                    armyCampBtn->setUserData(armyCampPreview);
                }
            }
        }
    );
    if (armyCampBtn) {
        armyCampBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 * 4.5 - 20));
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
                    Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                    wallsPreview->setMinePosition(you);

                    // 添加到背景精灵，并保存到按钮的UserData
                    background_sprite_->addChild(wallsPreview, 10);
                    wallsBtn->setUserData(wallsPreview);
                }
            }
        }
    );
    if (wallsBtn) {
        wallsBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 * 5.5 - 20));
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
                    Vec2 you = ConvertTest::convertLocalToGrid(my, background_sprite_);
                    builderHutPreview->setMinePosition(you);

                    // 添加到背景精灵，并保存到按钮的UserData
                    background_sprite_->addChild(builderHutPreview, 10);
                    builderHutBtn->setUserData(builderHutPreview);
                }
            }
        }
    );
    if (builderHutBtn) {
        builderHutBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.6 * 6.5 - 20));
    }
    builderHutBtn->setScale(0.6f);

    auto panelMenu = Menu::create(goldMineBtn, elixirCollectorBtn, goldStorageBtn, elixirStorageBtn,armyCampBtn, wallsBtn,builderHutBtn, nullptr);
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

    // 创建坐标显示标签
    //coordinate_label_ = Label::createWithTTF("坐标: ", "fonts/STZhongSong_Bold.ttf", 20);
    //coordinate_label_->setColor(Color3B::YELLOW);
    //coordinate_label_->setPosition(Vec2(origin.x + visibleSize.width - 200, origin.y + 30));
    //this->addChild(coordinate_label_, 2);
  
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
        elixirNameLabel->setPosition(Vec2(elixirIcon->getPositionX() - elixirNameLabel->getContentSize().width / 2, elixirIcon->getPositionY()));
        this->addChild(elixirNameLabel, 2);

        // 创建圣水数量标签
        elixirLabel = Label::createWithTTF("750", "fonts/Marker Felt.ttf", 24);
        elixirLabel->setColor(Color3B::BLUE);
        elixirLabel->setPosition(Vec2(elixirIcon->getPositionX() + 20, elixirIcon->getPositionY()));
        this->addChild(elixirLabel, 2);
    }

    // 创建金币图标和标签
    goldIcon = Sprite::create("btn_pressed.png"); // 实际项目中应该替换为正确的金币图标资源名
    if (goldIcon == nullptr)
    {
        problemLoading("'btn_pressed.png' (作为金币图标的替代)");
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
        goldNameLabel->setPosition(Vec2(goldIcon->getPositionX() - goldNameLabel->getContentSize().width / 2, goldIcon->getPositionY()));
        this->addChild(goldNameLabel, 2);

        // 创建金币数量标签
        goldLabel = Label::createWithTTF("750", "fonts/Marker Felt.ttf", 24);
        goldLabel->setColor(Color3B::YELLOW);
        goldLabel->setPosition(Vec2(goldIcon->getPositionX() + 20, goldIcon->getPositionY()));
        this->addChild(goldLabel, 2);
    }

    // 347-361 创建宝石图标和标签
    gemIcon= Sprite::create("btn_disabled.png");
    if (gemIcon == nullptr)
    {
        problemLoading("'btn_disabled.png' (作为金币图标的替代)");
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
        gemNameLabel->setPosition(Vec2(gemIcon->getPositionX() - gemNameLabel->getContentSize().width / 2, gemIcon->getPositionY()));
        this->addChild(gemNameLabel, 2);

        // 创建金币数量标签
        gemLabel = Label::createWithTTF("15", "fonts/Marker Felt.ttf", 24);
        gemLabel->setColor(Color3B::GREEN);
        gemLabel->setPosition(Vec2(gemIcon->getPositionX() + 20, gemIcon->getPositionY()));
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

    // 当经过1秒时
    if (elapsedTime >= 1.0f)
    {
        int totalGoldRate = baseGoldRate;
        int totalElixirRate = baseElixirRate;
        // 判断建筑类型并分别累加速度
        for (auto building : placedBuildings) {
            if (building->getIsUpgrade()) {
                building->cutTime();
            }
            //一下均为非升级中
            else if (dynamic_cast<GoldMine*>(building)) {
                building->updateCurrentStock(); // 产到库存
            }
            else if (dynamic_cast<ElixirCollector*>(building)) {
                building->updateCurrentStock(); // 产到库存
            }
            else {
                continue;//只有金矿和圣水收集器需要实时更新
            }
        }
        // 每一秒都更新标签显示
        elixirLabel->setString(StringUtils::format("%d", g_elixirCount));
        goldLabel->setString(StringUtils::format("%d", g_goldCount));
        gemLabel->setString(StringUtils::format("%d", g_gemCount));
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
            // 复用菱形碰撞检测代码（判断触摸点是否在当前建筑的菱形范围内）
            Sprite* mineSprite = building->getSprite();
            Vec2 buildingScreenPos = background_sprite_->convertToWorldSpace(building->getPosition());
            const float horizontalDiag = 56.0f * 3;
            const float verticalDiag = 42.0f * 3;
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
                _curOpenInfoPanel = BuildingInfoPanel::create(clickedBuilding, background_sprite_);
                clickedBuilding->addChild(_curOpenInfoPanel, 100); // 确保面板在最上层
                _curOpenBuilding = clickedBuilding; // 更新绑定的建筑
            }
            // 无面板 → 打开新面板
            else {
                _curOpenInfoPanel = BuildingInfoPanel::create(clickedBuilding, background_sprite_);
                clickedBuilding->addChild(_curOpenInfoPanel, 100);
                _curOpenBuilding = clickedBuilding;
            }
        }
        return true; // 吞噬事件，避免触发移动
    }

    // 非双击，只处理拖拽状态下的逻辑、建筑移动逻辑和缩放管理器的逻辑   
    if (isDragging) {

        return true; // 正在拖拽时返回true，保持事件被捕获
    }
    Vec2 touchPos = touch->getLocation();
    // 检查是否点击了已放置的
    for (auto& building : placedBuildings) {
        if (!building) continue;
        Sprite* mineSprite = building->getSprite();
        if (!mineSprite) continue;

        // 建筑的世界坐标（菱形中心）
        Vec2 buildingScreenPos = background_sprite_->convertToWorldSpace(building->getPosition());

        // 菱形参数配置
        const float horizontalDiag = 56.0f * 3; // 水平对角线总长度
        const float verticalDiag = 42.0f * 3;   // 竖直对角线总长度
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
            building->setOpacity(128);
            background_sprite_->reorderChild(building, 20);
            return true;
        }
    }
    // 如果没有拖拽且没有点击建筑，则使用缩放管理器的触摸处理
    return zoom_manager_->onTouchBegan(touch, event);
}

void SecondScene::onTouchMoved(Touch* touch, Event* event)
{
    if (isDragging && draggingItem) {
        // 区分拖拽的是金矿预览还是圣水收集器预览
        void* userData = draggingItem->getUserData();
        if (!userData) {
            return;
        }

        Vec2 localPos = background_sprite_->convertToNodeSpace(touch->getLocation());
        //Vec2 localPos = ConvertTest::convertScreenToGrid(touch->getLocation(), background_sprite_, buildPanel);
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
    }
    else if (isMovingBuilding) {
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
                return; // 不更新位置
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
        // 获取拖拽结束位置
        Vec2 screenPos = touch->getLocation();
        // 将屏幕坐标转换为相对于背景精灵的本地坐标
        Vec2 localPos = background_sprite_->convertToNodeSpace(screenPos);
        //Vec2 localPos = ConvertTest::convertScreenToGrid(screenPos, background_sprite_, buildPanel);

        // 获取网格单元格大小并进行向上取整
        float gridCellSizeX = grid_manager_->getGridCellSizeX()/2;
        float gridCellSizeY = grid_manager_->getGridCellSizeY()/2;
        float snappedX = ceil(localPos.x / gridCellSizeX) * gridCellSizeX;
        float snappedY = ceil(localPos.y / gridCellSizeY) * gridCellSizeY;
        Vec2 snappedPos = Vec2(snappedX, snappedY);
#if 0
        // 1. 移除预览对象
        if (draggingItem == elixirCollectorBtn) {
            ElixirCollector* dragElixirPreview = static_cast<ElixirCollector*>(draggingItem->getUserData());
            if (dragElixirPreview) {
                dragElixirPreview->removeFromParentAndCleanup(true);
                draggingItem->setUserData(nullptr);
            }
        }
#endif
        // 检查放置区域有效性
        Vec2 diamondPos = convertScreenToDiamond(screenPos);
        // 在有效区域
        if (isInDiamond(diamondPos)) {
            bool isColliding = false;
            Vec2 targetPos = snappedPos;

            // 检查与已放置建筑的碰撞
            for (auto building : placedBuildings) {
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
                        background_sprite_->addChild(failGoldMine, 15);
                        failGoldMine->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == elixirCollectorBtn) {
                    auto failElixir = ElixirCollector::create("ElixirCollectorLv1.png");
                    if (failElixir) {
                        failElixir->setPosition(snappedPos);
                        background_sprite_->addChild(failElixir, 15);
                        failElixir->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == goldStorageBtn) {
                    auto failGoldStorage = GoldStorage::create("GoldStorageLv1.png");
                    if (failGoldStorage) {
                        failGoldStorage->setPosition(snappedPos);
                        background_sprite_->addChild(failGoldStorage, 15);
                        failGoldStorage->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == elixirStorageBtn) {
                    auto failElixirStorage = ElixirStorage::create("ElixirStorageLv1.png");
                    if (failElixirStorage) {
                        failElixirStorage->setPosition(snappedPos);
                        background_sprite_->addChild(failElixirStorage, 15);
                        failElixirStorage->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == armyCampBtn) {
                    auto failArmyCamp = ElixirStorage::create("ArmyCampLv1.png");
                    if (failArmyCamp) {
                        failArmyCamp->setPosition(snappedPos);
                        background_sprite_->addChild(failArmyCamp, 15);
                        failArmyCamp->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == wallsBtn) {
                    auto failwalls = Walls::create("WallsLv1.png");
                    if (failwalls) {
                        failwalls->setPosition(snappedPos);
                        background_sprite_->addChild(failwalls, 15);
                        failwalls->playFailBlinkAndRemove();
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
                        background_sprite_->addChild(placedGoldMine, 15);
                        placedBuildings.push_back(placedGoldMine);
                        placedGoldMine->playSuccessBlink();
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
                        background_sprite_->addChild(placedElixirCollector, 15);
                        placedBuildings.push_back(placedElixirCollector);
                        placedElixirCollector->playSuccessBlink();
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
                        background_sprite_->addChild(placedGoldStorage, 15);
                        placedBuildings.push_back(placedGoldStorage);
                        placedGoldStorage->playSuccessBlink();
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
                        background_sprite_->addChild(placedElixirStorage, 15);
                        placedBuildings.push_back(placedElixirStorage);
                        placedElixirStorage->playSuccessBlink();
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
                        background_sprite_->addChild(placedArmyCamp, 15);
                        placedBuildings.push_back(placedArmyCamp);
                        placedArmyCamp->playSuccessBlink();
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
                        background_sprite_->addChild(placedWalls, 15);
                        placedBuildings.push_back(placedWalls);
                        placedWalls->playSuccessBlink();
                    }
                    //扣除资源
                    g_goldCount -= goldCost;
                    g_elixirCount -= elixirCost;
                }
            }
            else if (draggingItem == builderHutBtn) {
                // 创建小屋
                int goldCost = static_cast<Walls*>(draggingItem->getUserData())->getGoldCost();
                int elixirCost = static_cast<Walls*>(draggingItem->getUserData())->getElixirCost();
                //再次判断资源是否足够
                if (g_goldCount >= goldCost && g_elixirCount >= elixirCost && hutNum < 5) {
                    //除旧
                    BuilderHut* dragBuilderHutPreview = static_cast<BuilderHut*>(draggingItem->getUserData());
                    if (dragBuilderHutPreview) {
                        dragBuilderHutPreview->removeFromParentAndCleanup(true);
                        draggingItem->setUserData(nullptr);
                    }
                    //迎新
                    auto placedBuilderhut = Walls::create("BuilderHutLv1.png");
                    if (placedBuilderhut) {
                        // 更新
                        placedBuilderhut->updatePosition(snappedPos);
                        background_sprite_->addChild(placedBuilderhut, 15);
                        placedBuildings.push_back(placedBuilderhut);
                        placedBuilderhut->playSuccessBlink();
                    }
                    //扣除资源
                    g_goldCount -= goldCost;
                    g_elixirCount -= elixirCost;
                    hutNum++;
                }
            }
            // 无效区域：创建对应建筑并执行失败反馈
            else {
                if (draggingItem == goldMineBtn) {
                    auto failGoldMine = GoldMine::create("GoldMineLv1.png");
                    if (failGoldMine) {
                        failGoldMine->setPosition(snappedPos);
                        background_sprite_->addChild(failGoldMine, 15);
                        failGoldMine->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == elixirCollectorBtn) {
                    auto failElixir = ElixirCollector::create("ElixirCollectorLv1.png");
                    if (failElixir) {
                        failElixir->setPosition(snappedPos);
                        background_sprite_->addChild(failElixir, 15);
                        failElixir->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == goldStorageBtn) {
                    auto failGoldStorage = GoldStorage::create("GoldStorageLv1.png");
                    if (failGoldStorage) {
                        failGoldStorage->setPosition(snappedPos);
                        background_sprite_->addChild(failGoldStorage, 15);
                        failGoldStorage->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == elixirStorageBtn) {
                    auto failElixirStorage = ElixirStorage::create("ElixirStorageLv1.png");
                    if (failElixirStorage) {
                        failElixirStorage->setPosition(snappedPos);
                        background_sprite_->addChild(failElixirStorage, 15);
                        failElixirStorage->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == armyCampBtn) {
                    auto failArmyCamp = ArmyCamp::create("ArmyCampLv1.png");
                    if (failArmyCamp) {
                        failArmyCamp->setPosition(snappedPos);
                        background_sprite_->addChild(failArmyCamp, 15);
                        failArmyCamp->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == wallsBtn) {
                    auto failWalls = Walls::create("WallsLv1.png");
                    if (failWalls) {
                        failWalls->setPosition(snappedPos);
                        background_sprite_->addChild(failWalls, 15);
                        failWalls->playFailBlinkAndRemove();
                    }
                }
                else if (draggingItem == builderHutBtn) {
                    auto failBuilderHut = BuilderHut::create("BuilderHutLv1.png");
                    if (failBuilderHut) {
                        failBuilderHut->setPosition(snappedPos);
                        background_sprite_->addChild(failBuilderHut, 15);
                        failBuilderHut->playFailBlinkAndRemove();
                    }
                }
            }

            // 重置拖拽状态
            isDragging = false;
            draggingItem = nullptr;
        }
    }
    else if (isMovingBuilding) {

        Vec2 localPos = background_sprite_->convertToNodeSpace(touch->getLocation());
        //Vec2 localPos = ConvertTest::convertScreenToGrid(touch->getLocation(), background_sprite_, buildPanel);
        Vec2 diamondPos = convertScreenToDiamond(touch->getLocation());
        bool inDiamond = isInDiamond(diamondPos);

        float gridCellSizeX = grid_manager_->getGridCellSizeX()/2;
        float gridCellSizeY = grid_manager_->getGridCellSizeY()/2;
        float snappedX = ceil(localPos.x / gridCellSizeX) * gridCellSizeX;
        float snappedY = ceil(localPos.y / gridCellSizeY) * gridCellSizeY;

        if (inDiamond && movingBuilding) {
            // 使用新的更新方法
            movingBuilding->updatePosition(Vec2(snappedX, snappedY));
            movingBuilding->setOpacity(255);
            background_sprite_->reorderChild(movingBuilding, 15);
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

        isDragging = false;
        draggingItem = nullptr;
    }
    else if (isMovingBuilding) {
        // 恢复
        if (movingBuilding) {
            movingBuilding->setOpacity(255);
            background_sprite_->reorderChild(movingBuilding, 15);
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
    Vec2 buildingWorldPos = building->convertToWorldSpace(Vec2::ZERO);
    Vec2 buildingLocalCenter = background_sprite_->convertToNodeSpace(buildingWorldPos);

    // 大菱形参数：
    // 水平对角线长 56.0* = 336 → 半长 168
    // 竖直对角线长 42.0*6 = 252 → 半长 126
    const float bigHalfHoriz = 56.0f * (yourSize + mySize) / 8; // 水平半对角线（56*6的一半）
    const float bigHalfVert = 42.0f * (yourSize + mySize) / 8;   // 竖直半对角线（42*6的一半）

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

#endif