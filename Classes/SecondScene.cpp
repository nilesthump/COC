#if 1
#include "HelloWorldScene.h"
#include "SecondScene.h"
#include "GoldMine.h"
#include "cocos2d.h"
#include <cmath>

// 初始化全局变量
int g_elixirCount = 750;
int g_goldCount = 750;

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
    baseGoldRate = 0; // 基础每秒1金币
    baseElixirRate = 0;
    g_goldCount = 750; // 确保金币计数初始化为0
    g_elixirCount = 750;

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
        double y = origin.y + backItem->getPositionY() - buildItem->getContentSize().height;
        buildItem->setPosition(Vec2(x, y));

        auto buildLabel = Label::createWithSystemFont("BUILD", "fonts/Marker Felt.ttf", 24);
        buildLabel->setColor(Color3B::WHITE);
        buildLabel->setPosition(Vec2(buildItem->getContentSize().width / 2, buildItem->getContentSize().height / 2));
        buildItem->addChild(buildLabel);
    }

    auto menu = Menu::create(backItem, buildItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);


    buildPanel = Node::create();

    double buildPanelX = buildItem->getContentSize().width;
    double buildPanelY = 0;
    buildPanel->setPosition(Vec2(buildPanelX, buildPanelY));
    buildPanel->setVisible(false);
    buildItem->addChild(buildPanel, 1);


    auto panelBg = Sprite::create("btn_long.png");
    if (panelBg == nullptr) {
        problemLoading("'btn_long.png'");
    }
    else {
        double panelBgX = panelBg->getContentSize().width / 2;
        double panelBgY = buildItem->getContentSize().height - panelBg->getContentSize().height / 2;
        panelBg->setPosition(Vec2(panelBgX, panelBgY));
        buildPanel->addChild(panelBg);
    }


    // 创建金矿按钮
    goldMineBtn = MenuItemImage::create(
        "GoldMineLv1.png",
        "GoldMineLv1.png",
        [=](Ref* pSender) {
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
                    goldMinePreview->setMinePosition(Vec2(goldMinePreview->getX(), goldMinePreview->getY()));
                    // 添加到背景精灵，并保存到按钮的UserData
                    background_sprite_->addChild(goldMinePreview, 10);
                    goldMineBtn->setUserData(goldMinePreview);
                }

                
            }
        }
    );
    if (goldMineBtn) {
        goldMineBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.5 / 2 - 10));
    }
    goldMineBtn->setScale(0.5f);


    // 创建圣水收集器按钮
    elixirCollectorBtn = MenuItemImage::create(
        "ElixirCollectorLv1.png",
        "ElixirCollectorLv1.png",
        [=](Ref* pSender) {
            if (!isDragging) {
                log("ElixirCollectorLv1 ");
                isDragging = true;
                draggingItem = elixirCollectorBtn;
                dragStartPosition = elixirCollectorBtn->getPosition();
                             
                auto elixirCollectorPreview = ElixirCollector::create("ElixirCollectorLv1.png"); // 预览用圣水收集器纹理
                if (elixirCollectorPreview) {
                    // 预览态设置：半透明（区分实际对象）
                    elixirCollectorPreview->setOpacity(150);
                    // goldMinePreview->setScale(0.5f); // 如需缩放可加

                    // 计算预览初始位置（和原来的逻辑一致）
                    Vec2 worldPos = elixirCollectorBtn->getParent()->convertToWorldSpace(elixirCollectorBtn->getPosition());
                    //Vec2 localPos = background_sprite_->convertToNodeSpace(worldPos);
                    elixirCollectorPreview->setMinePosition(worldPos);

                    // 添加到背景精灵，并保存到按钮的UserData
                    background_sprite_->addChild(elixirCollectorPreview, 10);
                    elixirCollectorBtn->setUserData(elixirCollectorPreview);
                }
                // 移除隐藏按钮的代码，这样在拖拽时原始按钮仍然可见
                // storageBtn->setVisible(false);
            }
        }
    );
    if (elixirCollectorBtn) {
        elixirCollectorBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.5 * 1.5 - 10 - 10));
    }
    elixirCollectorBtn->setScale(0.5f);

 


    auto panelMenu = Menu::create(goldMineBtn, elixirCollectorBtn, nullptr);
    panelMenu->setPosition(Vec2::ZERO);
    panelBg->addChild(panelMenu);

    // 初始化拖拽状态
    isDragging = false;
    draggingItem = nullptr;
    
    // 初始化建筑移动相关变量
    isMovingBuilding = false;
    movingGoldMine = nullptr;
    movingElixirCollector = nullptr;

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
    // add "HelloWorld" splash screen"
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

    // 创建缩放管理器
    zoom_manager_ = ZoomScrollManager::create(background_sprite_, 0.5f, 2.0f);
    this->addChild(zoom_manager_);

    // 初始化菱形位置信息（根据原有测量数据）
    int left_x = 667;          // 左顶点x坐标（距左边界）
    int right_x = 3705 - 556;  // 右顶点x坐标（图像宽度 - 距右边界距离）
    int top_y = 264;           // 上顶点y坐标（距上边界）
    int bottom_y = 2545 - 471; // 下顶点y坐标（图像高度 - 距下边界距离）

    float diamond_width = right_x - left_x;       // 菱形宽度: 3149 - 667 = 2482
    float diamond_height = bottom_y - top_y;      // 菱形高度: 2074 - 264 = 1810

    // 计算菱形中心相对于背景精灵中心的位置
    Vec2 diamond_center_absolute = Vec2((left_x + right_x) / 2.0f, (top_y + bottom_y) / 2.0f);
    Vec2 diamond_center = diamond_center_absolute - Vec2(background_sprite_->getContentSize().width / 2, background_sprite_->getContentSize().height / 2);

    // 创建菱形网格管理器
    grid_manager_ = DiamondGridManager::create(diamond_center, diamond_width, diamond_height, 44);
    this->addChild(grid_manager_);

    // 绘制菱形网格
    grids_ = grid_manager_->drawDiamondGrid(background_sprite_, 50.0f);

    // 创建坐标显示标签
    coordinate_label_ = Label::createWithTTF("坐标: ", "fonts/STZhongSong_Bold.ttf", 20);
    coordinate_label_->setColor(Color3B::YELLOW);
    coordinate_label_->setPosition(Vec2(origin.x + visibleSize.width - 200, origin.y + 30));
    this->addChild(coordinate_label_, 2);
  
    // 创建圣水图标和标签
    // 尝试加载圣水图标，如果失败则使用HelloWorld.png作为替代
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
        elixirLabel = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 24);
        elixirLabel->setColor(Color3B::BLUE);
        elixirLabel->setPosition(Vec2(elixirIcon->getPositionX() + 20, elixirIcon->getPositionY()));
        this->addChild(elixirLabel, 2);
    }

    // 创建金币图标和标签
    // 尝试加载金币图标，如果失败则使用btn_pressed.png作为替代
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
        goldLabel = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 24);
        goldLabel->setColor(Color3B::YELLOW);
        goldLabel->setPosition(Vec2(goldIcon->getPositionX() + 20, goldIcon->getPositionY()));
        this->addChild(goldLabel, 2);
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
        for (auto mine : placedGoldMines) {
            totalGoldRate += mine->getGoldSpeed();
        }

        int totalElixirRate = baseElixirRate;
        for (auto mine : placedElixirCollectors) {
            totalElixirRate += mine->getElixirSpeed();
        }
        // 增加圣水数量
        g_elixirCount+= totalElixirRate;
        
        // 增加金币数量
        g_goldCount += totalGoldRate;

        // 更新标签显示
        if (elixirLabel)
        {
            elixirLabel->setString(StringUtils::format("%d", g_elixirCount));
        }
        
        // 更新金币标签显示
        if (goldLabel)
        {
            goldLabel->setString(StringUtils::format("%d", g_goldCount));
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

bool SecondScene::onTouchBegan(Touch* touch, Event* event)
{
    
    // 只处理拖拽状态下的逻辑、建筑移动逻辑和缩放管理器的逻辑   
    if (isDragging) {
        return true; // 正在拖拽时返回true，保持事件被捕获
    }
    Vec2 touchPos = touch->getLocation();
    // 检查是否点击了已放置的金矿
    for (auto& goldMine : placedGoldMines) {
        if (!goldMine) continue;
        Sprite* mineSprite = goldMine->getSprite();
        if (!mineSprite) continue;
        Vec2 buildingScreenPos = background_sprite_->convertToWorldSpace(goldMine->getPosition());
        Size buildingSize = mineSprite->getContentSize();
        float scale = mineSprite->getScale();
        Rect buildingRect(
            buildingScreenPos.x - buildingSize.width * scale / 2,
            buildingScreenPos.y - buildingSize.height * scale / 2,
            buildingSize.width * scale,
            buildingSize.height * scale
        );
        if (buildingRect.containsPoint(touchPos)) {
            isMovingBuilding = true;
            movingGoldMine = goldMine;
            goldMine->setOpacity(128);
            background_sprite_->reorderChild(goldMine, 20);
            return true;
        }
    }

    // 检查是否点击了已放置的圣水收集器
    for (auto& elixirCollector : placedElixirCollectors) {
        if (!elixirCollector) continue;
        Sprite* collectorSprite = elixirCollector->getSprite();
        if (!collectorSprite) continue;
        Vec2 buildingScreenPos = background_sprite_->convertToWorldSpace(elixirCollector->getPosition());
        Size buildingSize = collectorSprite->getContentSize();
        float scale = collectorSprite->getScale();
        Rect buildingRect(
            buildingScreenPos.x - buildingSize.width * scale / 2,
            buildingScreenPos.y - buildingSize.height * scale / 2,
            buildingSize.width * scale,
            buildingSize.height * scale
        );
        if (buildingRect.containsPoint(touchPos)) {
            isMovingBuilding = true;
            // 注意：如果需要同时移动两种建筑，建议将 movingBuilding 改为 void* 或基类指针
            // 这里先简化：新增 movingElixirCollector 变量
            movingElixirCollector = elixirCollector;
            elixirCollector->setOpacity(128);
            background_sprite_->reorderChild(elixirCollector, 20);
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
        if (!userData) return;

        Vec2 localPos = background_sprite_->convertToNodeSpace(touch->getLocation());
        float gridCellSizeX = grid_manager_->getGridCellSizeX();
        float gridCellSizeY = grid_manager_->getGridCellSizeY();
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
    }
    else if (isMovingBuilding) {
        Vec2 localPos = background_sprite_->convertToNodeSpace(touch->getLocation());
        Vec2 diamondPos = convertScreenToDiamond(touch->getLocation());
        bool inDiamond = isInDiamond(diamondPos);

        float gridCellSizeX = grid_manager_->getGridCellSizeX();
        float gridCellSizeY = grid_manager_->getGridCellSizeY();
        float snappedX = ceil(localPos.x / gridCellSizeX) * gridCellSizeX;
        float snappedY = ceil(localPos.y / gridCellSizeY) * gridCellSizeY;
        static Vec2 lastValidMovePos;

        if (inDiamond) {
            // 在移动建筑的代码块中添加
            bool isColliding = false;

            // 检查与其他建筑的碰撞
            if (movingGoldMine) {
                // 排除自身进行检测
                for (auto mine : placedGoldMines) {
                    if (mine == movingGoldMine) continue;
                    if (isPointInBuilding(Vec2(snappedX, snappedY), mine)) {
                        isColliding = true;
                        break;
                    }
                }
                // 检查与圣水收集器的碰撞
                if (!isColliding) {
                    for (auto collector : placedElixirCollectors) {
                        if (isPointInBuilding(Vec2(snappedX, snappedY), collector)) {
                            isColliding = true;
                            break;
                        }
                    }
                }
            }
            else if (movingElixirCollector) {
                // 排除自身进行检测
                for (auto collector : placedElixirCollectors) {
                    if (collector == movingElixirCollector) continue;
                    if (isPointInBuilding(Vec2(snappedX, snappedY), collector)) {
                        isColliding = true;
                        break;
                    }
                }
                // 检查与金矿的碰撞
                if (!isColliding) {
                    for (auto mine : placedGoldMines) {
                        if (isPointInBuilding(Vec2(snappedX, snappedY), mine)) {
                            isColliding = true;
                            break;
                        }
                    }
                }
            }

            // 如果碰撞，不更新位置或显示红色提示
            if (isColliding) {
                // 可以将建筑设为红色提示碰撞
                if (movingGoldMine) {
                    movingGoldMine->getSprite()->setColor(Color3B::RED);
                }
                else if (movingElixirCollector) {
                    movingElixirCollector->getSprite()->setColor(Color3B::RED);
                }
                return; // 不更新位置
            }
            else {
                // 恢复颜色
                if (movingGoldMine) {
                    movingGoldMine->getSprite()->setColor(Color3B::WHITE);
                }
                else if (movingElixirCollector) {
                    movingElixirCollector->getSprite()->setColor(Color3B::WHITE);
                }
            }

            // 如果没有碰撞，继续移动
            lastValidMovePos = Vec2(snappedX, snappedY);
        }
        else {
            snappedX = lastValidMovePos.x;
            snappedY = lastValidMovePos.y;
        }

        // 移动金矿
        if (movingGoldMine) {
            movingGoldMine->setPosition(Vec2(snappedX, snappedY));
        }
        // 移动圣水收集器
        else if (movingElixirCollector) {
            movingElixirCollector->setPosition(Vec2(snappedX, snappedY));
        }
    }
    else if (zoom_manager_) {
        zoom_manager_->onTouchMoved(touch, event);
    }
}

void SecondScene::onTouchEnded(Touch* touch, Event* event)
{
    if (isDragging && draggingItem) {
        // 获取拖拽结束位置
        Vec2 screenPos = touch->getLocation();
        // 将屏幕坐标转换为相对于背景精灵的本地坐标
        Vec2 localPos = background_sprite_->convertToNodeSpace(screenPos);
        
        // 获取网格单元格大小并进行向上取整
        float gridCellSizeX = grid_manager_->getGridCellSizeX();
        float gridCellSizeY = grid_manager_->getGridCellSizeY();
        float snappedX = ceil(localPos.x / gridCellSizeX) * gridCellSizeX;
        float snappedY = ceil(localPos.y / gridCellSizeY) * gridCellSizeY;
        Vec2 snappedPos = Vec2(snappedX, snappedY);
        
        // 1. 移除预览对象（区分金矿/圣水收集器）
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


        // 2. 检查放置区域有效性
        Vec2 diamondPos = convertScreenToDiamond(screenPos);
        if (isInDiamond(diamondPos)) {

            // 在有效区域放置建筑的代码块中添加
            bool isColliding = false;
            Vec2 targetPos = snappedPos;

            // 检查与已放置金矿的碰撞
            for (auto mine : placedGoldMines) {
                if (isPointInBuilding(targetPos, mine)) {
                    isColliding = true;
                    break;
                }
            }

            // 检查与已放置圣水收集器的碰撞
            if (!isColliding) {
                for (auto collector : placedElixirCollectors) {
                    if (isPointInBuilding(targetPos, collector)) {
                        isColliding = true;
                        break;
                    }
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
                return; // 阻止放置
            }
            // 如果没有碰撞，继续执行放置逻辑           
            // 有效区域：根据按钮类型创建对应建筑
            if (draggingItem == goldMineBtn) {
                // 创建金矿
                auto placedGoldMine = GoldMine::create("GoldMineLv1.png");
                /*if (placedGoldMine) {
                    placedGoldMine->setPosition(snappedPos);
                    background_sprite_->addChild(placedGoldMine, 15);
                    placedGoldMines.push_back(placedGoldMine);
                    placedGoldMine->playSuccessBlink();
                    log("成功放置金矿，产金速度：%.1f，位置：(%.2f, %.2f)",
                        placedGoldMine->getGoldSpeed(), snappedPos.x, snappedPos.y);
                }*/
                if (placedGoldMine) {
                    // 使用新的更新方法
                    placedGoldMine->updatePosition(snappedPos);
                    background_sprite_->addChild(placedGoldMine, 15);
                    placedGoldMines.push_back(placedGoldMine);
                    placedGoldMine->playSuccessBlink();
                    log("成功放置金矿，产金速度：%.1f，位置：(%.2f, %.2f)",
                        placedGoldMine->getGoldSpeed(), snappedPos.x, snappedPos.y);
                }
            }
            else if (draggingItem == elixirCollectorBtn) {
                // 创建圣水收集器
                auto placedElixir = ElixirCollector::create("ElixirCollectorLv1.png"); // 替换为你的圣水收集器纹理名
                if (placedElixir) {
                    // 使用新的更新方法
                    placedElixir->updatePosition(snappedPos);
                    background_sprite_->addChild(placedElixir, 15);
                    placedElixirCollectors.push_back(placedElixir);
                    placedElixir->playSuccessBlink();
                    log("成功放置金矿，产金速度：%.1f，位置：(%.2f, %.2f)",
                        placedElixir->getElixirSpeed(), snappedPos.x, snappedPos.y);
                }
            }
        }
        else {
            // 无效区域：创建对应建筑并执行失败反馈
            if (draggingItem == goldMineBtn) {
                auto failGoldMine = GoldMine::create("GoldMineLv1.png");
                if (failGoldMine) {
                    failGoldMine->setPosition(snappedPos);
                    background_sprite_->addChild(failGoldMine, 15);
                    failGoldMine->playFailBlinkAndRemove();
                    log("尝试在无效位置放置金矿: (%.2f, %.2f)", snappedPos.x, snappedPos.y);
                }
            }
            else if (draggingItem == elixirCollectorBtn) {
                auto failElixir = ElixirCollector::create("ElixirCollectorLv1.png");
                if (failElixir) {
                    failElixir->setPosition(snappedPos);
                    background_sprite_->addChild(failElixir, 15);
                    failElixir->playFailBlinkAndRemove(); // 确保圣水收集器有该方法
                    log("尝试在无效位置放置圣水收集器: (%.2f, %.2f)", snappedPos.x, snappedPos.y);
                }
            }
        }
        
        // 重置拖拽状态
        isDragging = false;
        draggingItem = nullptr;
    }
    else if (isMovingBuilding) {

        Vec2 localPos = background_sprite_->convertToNodeSpace(touch->getLocation());
        Vec2 diamondPos = convertScreenToDiamond(touch->getLocation());
        bool inDiamond = isInDiamond(diamondPos);

        float gridCellSizeX = grid_manager_->getGridCellSizeX();
        float gridCellSizeY = grid_manager_->getGridCellSizeY();
        float snappedX = ceil(localPos.x / gridCellSizeX) * gridCellSizeX;
        float snappedY = ceil(localPos.y / gridCellSizeY) * gridCellSizeY;

        // 移动金矿
        /*if (inDiamond && movingGoldMine) {
            movingGoldMine->setPosition(Vec2(snappedX, snappedY));
        }*/
        // 在移动建筑结束的代码块中添加
      
        if (inDiamond && movingGoldMine) {
            // 使用新的更新方法
            movingGoldMine->updatePosition(Vec2(snappedX, snappedY));
            movingGoldMine->setOpacity(255);
            background_sprite_->reorderChild(movingGoldMine, 15);
            movingGoldMine = nullptr;
        }
        // 移动圣水收集器
        else if (inDiamond && movingElixirCollector) {
            movingElixirCollector->updatePosition(Vec2(snappedX, snappedY));
            movingElixirCollector->setOpacity(255);
            background_sprite_->reorderChild(movingElixirCollector, 15);
            movingElixirCollector = nullptr;
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

        isDragging = false;
        draggingItem = nullptr;
    }
    else if (isMovingBuilding) {
        // 恢复金矿
        if (movingGoldMine) {
            movingGoldMine->setOpacity(255);
            background_sprite_->reorderChild(movingGoldMine, 15);
            movingGoldMine = nullptr;
        }
        // 恢复圣水收集器
        else if (movingElixirCollector) {
            movingElixirCollector->setOpacity(255);
            background_sprite_->reorderChild(movingElixirCollector, 15);
            movingElixirCollector = nullptr;
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
bool SecondScene::isPointInBuilding(const Vec2& point, Node* building) {
    if (!building) return false;

    Sprite* sprite = nullptr;
    // 判断建筑类型并获取精灵
    if (dynamic_cast<GoldMine*>(building)) {
        sprite = static_cast<GoldMine*>(building)->getSprite();
    }
    else if (dynamic_cast<ElixirCollector*>(building)) {
        sprite = static_cast<ElixirCollector*>(building)->getSprite();
    }

    if (!sprite) return false;

    // 计算建筑在背景上的世界坐标
    Vec2 buildingPos = background_sprite_->convertToNodeSpace(building->convertToWorldSpace(Vec2::ZERO));
    Size size = sprite->getContentSize() * sprite->getScale();

    // 建筑世界坐标 → 转换为背景精灵的本地坐标（和传入的point同空间）
    Vec2 buildingWorldPos = building->convertToWorldSpace(Vec2::ZERO);
    Vec2 buildingLocalCenter = background_sprite_->convertToNodeSpace(buildingWorldPos);

    //单个小菱形参数：水平对角线56f → 半长28f；竖直对角线42f → 半长21f
    const float halfHoriz = 28.0f;
    const float halfVert = 21.0f;
    const float extendRatio = 2.5f;
    
    // 第一部分：原有9个核心菱形（3x3）
    Vec2 coreOffsets[9] = {
        Vec2(0, 0),                // 中心
        Vec2(halfHoriz, 0),        // 右
        Vec2(-halfHoriz, 0),       // 左
        Vec2(0, halfVert),         // 上
        Vec2(0, -halfVert),        // 下
        Vec2(halfHoriz, halfVert), // 右上
        Vec2(-halfHoriz, halfVert),// 左上
        Vec2(halfHoriz, -halfVert),// 右下
        Vec2(-halfHoriz, -halfVert)// 左下
    };
    // 第二部分：外层12个扩展半菱形（上下左右/斜向各延伸半个菱形）
    Vec2 extendOffsets[12] = {
        Vec2(halfHoriz * extendRatio, 0),          // 正右扩展
        Vec2(-halfHoriz * extendRatio, 0),         // 正左扩展
        Vec2(0, halfVert * extendRatio),           // 正上扩展
        Vec2(0, -halfVert * extendRatio),          // 正下扩展
        Vec2(halfHoriz * 1.25f, halfVert * 1.25f), // 右上扩展
        Vec2(-halfHoriz * 1.25f, halfVert * 1.25f),// 左上扩展
        Vec2(halfHoriz * 1.25f, -halfVert * 1.25f),// 右下扩展
        Vec2(-halfHoriz * 1.25f, -halfVert * 1.25f),// 左下扩展
        Vec2(halfHoriz * extendRatio, halfVert),   // 右中扩展（上）
        Vec2(halfHoriz * extendRatio, -halfVert),  // 右中扩展（下）
        Vec2(-halfHoriz * extendRatio, halfVert),  // 左中扩展（上）
        Vec2(-halfHoriz * extendRatio, -halfVert)  // 左中扩展（下）
    };

    // 单个菱形碰撞判断（带容错）
    auto isPointInSingleDiamond = [](const Vec2& p, const Vec2& center, float hh, float hv) -> bool {
        float dx = abs(p.x - center.x);
        float dy = abs(p.y - center.y);
        return (dx / hh) + (dy / hv) <= 1.0f + 0.001f; // 浮点容错
        };

    // 第一步：检测核心9个菱形
    for (int i = 0; i < 9; ++i) {
        Vec2 diamondCenter = buildingLocalCenter + coreOffsets[i];
        if (isPointInSingleDiamond(point, diamondCenter, halfHoriz, halfVert)) {
            return true;
        }
    }

    // 第二步：检测外层12个扩展半菱形（偏移放大，菱形尺寸不变）
    for (int i = 0; i < 12; ++i) {
        Vec2 diamondCenter = buildingLocalCenter + extendOffsets[i];
        if (isPointInSingleDiamond(point, diamondCenter, halfHoriz, halfVert)) {
            return true;
        }
    }

    // 无碰撞
    return false;
}

#endif