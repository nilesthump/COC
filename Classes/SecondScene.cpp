#if 1
#include "HelloWorldScene.h"
#include "SecondScene.h"
#include "GoldMine.h"
#include "cocos2d.h"
#include <cmath>

// 初始化全局变量
int g_elixirCount = 0;
int g_goldCount = 0;

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
    baseGoldRate = 1; // 基础每秒1金币
    g_goldCount = 0; // 确保金币计数初始化为0

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
                    Vec2 worldPos = goldMineBtn->getParent()->convertToWorldSpace(goldMineBtn->getPosition());
                    Vec2 localPos = background_sprite_->convertToNodeSpace(worldPos);
                    goldMinePreview->setMinePosition(localPos);

                    // 添加到背景精灵，并保存到按钮的UserData
                    background_sprite_->addChild(goldMinePreview, 10);
                    goldMineBtn->setUserData(goldMinePreview);
                }

                // 保留按钮可见（你原来的逻辑）
                // houseBtn->setVisible(false);
            }
        }
    );
    if (goldMineBtn) {
        goldMineBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.5 / 2 - 10));
    }
    goldMineBtn->setScale(0.5f);


    // 创建仓库按钮
    storageBtn = MenuItemImage::create(
        "CannonLv10.png",
        "CannonLv10.png",
        [=](Ref* pSender) {
            if (!isDragging) {
                log("storage ");
                isDragging = true;
                draggingItem = storageBtn;
                dragStartPosition = storageBtn->getPosition();
                
                // 创建拖拽副本
                auto dragSprite = Sprite::create("CannonLv10.png");
                if (dragSprite) {
                    // 获取按钮在屏幕上的位置
                    Vec2 worldPos = storageBtn->getParent()->convertToWorldSpace(storageBtn->getPosition());
                    // 转换为背景精灵的本地坐标
                    Vec2 localPos = background_sprite_->convertToNodeSpace(worldPos);
                    dragSprite->setScale(1.0f);
                    dragSprite->setPosition(localPos);
                    background_sprite_->addChild(dragSprite, 10);
                    storageBtn->setUserData(dragSprite);
                }
                
                // 移除隐藏按钮的代码，这样在拖拽时原始按钮仍然可见
                // storageBtn->setVisible(false);
            }
        }
    );
    if (storageBtn) {
        storageBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - goldMineBtn->getContentSize().height * 0.5 * 1.5 - 10 - 10));
    }
    storageBtn->setScale(0.5f);

 


    auto panelMenu = Menu::create(goldMineBtn, storageBtn, nullptr);
    panelMenu->setPosition(Vec2::ZERO);
    panelBg->addChild(panelMenu);

    // 初始化拖拽状态
    isDragging = false;
    draggingItem = nullptr;
    
    // 初始化建筑移动相关变量
    isMovingBuilding = false;
    movingBuilding = nullptr;

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
        elixirLabel->setPosition(Vec2(elixirIcon->getPositionX() + 40, elixirIcon->getPositionY()));
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
        goldLabel->setPosition(Vec2(goldIcon->getPositionX() + 40, goldIcon->getPositionY()));
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

        // 增加圣水数量
        g_elixirCount+=1;
        
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
    // 不需要检测按钮点击，因为已经在按钮回调中处理了
    // 只处理拖拽状态下的逻辑、建筑移动逻辑和缩放管理器的逻辑
    
    if (isDragging) {
        return true; // 正在拖拽时返回true，保持事件被捕获
    }
    
    // 检查是否点击了已放置的金矿建筑
    Vec2 touchPos = touch->getLocation();
    // 遍历金矿列表
    for (auto& goldMine : placedGoldMines) {
        if (!goldMine) continue; // 空对象跳过

        // 1. 获取金矿的精灵（因为碰撞检测需要精灵的尺寸）
        Sprite* mineSprite = goldMine->getSprite();
        if (!mineSprite) continue;

        // 2. 将金矿的本地坐标转换为屏幕坐标（和原来逻辑一致）
        Vec2 buildingScreenPos = background_sprite_->convertToWorldSpace(goldMine->getPosition());

        // 3. 计算金矿的边界框（用精灵的尺寸和缩放）
        Size buildingSize = mineSprite->getContentSize();
        float scale = mineSprite->getScale();
        Rect buildingRect(
            buildingScreenPos.x - buildingSize.width * scale / 2,
            buildingScreenPos.y - buildingSize.height * scale / 2,
            buildingSize.width * scale,
            buildingSize.height * scale
        );

        // 4. 检查点击是否在金矿范围内（逻辑不变）
        if (buildingRect.containsPoint(touchPos)) {
            // 找到被点击的金矿
            isMovingBuilding = true;
            movingBuilding = goldMine; // 赋值给GoldMine*类型的变量

            // 设置金矿为半透明状态
            goldMine->setOpacity(128); // 金矿节点半透明（包含精灵）
            // 也可以只让精灵半透明：mineSprite->setOpacity(128);

            // 将金矿移到最顶层
            background_sprite_->reorderChild(goldMine, 20);
            return true;
        }
    }

    // 如果没有拖拽且没有点击建筑，则使用缩放管理器的触摸处理
    return zoom_manager_->onTouchBegan(touch, event);
}

void SecondScene::onTouchMoved(Touch* touch, Event* event)
{
    if (isDragging && draggingItem) {
        // 移动拖拽的金矿预览（替换：操作GoldMine而非Sprite）
        // 从按钮的UserData中获取GoldMine预览对象
        GoldMine* dragMinePreview = static_cast<GoldMine*>(draggingItem->getUserData());
        if (dragMinePreview) {
            // 将屏幕坐标转换为相对于背景精灵的本地坐标（逻辑不变）
            Vec2 localPos = background_sprite_->convertToNodeSpace(touch->getLocation());

            // 获取网格单元格大小（逻辑不变）
            float gridCellSizeX = grid_manager_->getGridCellSizeX();
            float gridCellSizeY = grid_manager_->getGridCellSizeY();

            // 将坐标按网格单元格大小的整数倍进行向上取整（逻辑不变）
            float snappedX = ceil(localPos.x / gridCellSizeX) * gridCellSizeX;
            float snappedY = ceil(localPos.y / gridCellSizeY) * gridCellSizeY;

            // 设置金矿预览的位置（调用金矿的方法，或直接setPosition）
            dragMinePreview->setPosition(Vec2(snappedX, snappedY));
            // 也可以用封装的方法：dragMinePreview->setMinePosition(Vec2(snappedX, snappedY));
        }
    }
    else if (isMovingBuilding && movingBuilding) {
        // 移动已放置的金矿建筑（替换：操作GoldMine而非Sprite）
        // 将屏幕坐标转换为相对于背景精灵的本地坐标（逻辑不变）
        Vec2 localPos = background_sprite_->convertToNodeSpace(touch->getLocation());

        // 获取网格单元格大小（逻辑不变）
        float gridCellSizeX = grid_manager_->getGridCellSizeX();
        float gridCellSizeY = grid_manager_->getGridCellSizeY();

        // 将坐标按网格单元格大小的整数倍进行向上取整（逻辑不变）
        float snappedX = ceil(localPos.x / gridCellSizeX) * gridCellSizeX;
        float snappedY = ceil(localPos.y / gridCellSizeY) * gridCellSizeY;

        // 设置金矿的位置（直接操作GoldMine对象）
        movingBuilding->setPosition(Vec2(snappedX, snappedY));
        // 也可以用封装的方法：movingBuilding->setMinePosition(Vec2(snappedX, snappedY));
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
        
        GoldMine* dragMinePreview = static_cast<GoldMine*>(draggingItem->getUserData());
        if (dragMinePreview) {
            dragMinePreview->removeFromParentAndCleanup(true);
            draggingItem->setUserData(nullptr);
        }

        // 检查是否在有效区域内放置（保留原逻辑，只执行一次）
        Vec2 diamondPos = convertScreenToDiamond(screenPos);
        if (isInDiamond(diamondPos)) {
            // 有效区域：创建真正的金矿对象
            std::string textureName = (draggingItem == goldMineBtn ? "GoldMineLv1.png" : "CannonLv10.png");
            auto placedGoldMine = GoldMine::create(textureName); // 创建金矿
            if (placedGoldMine) {
                // 设置位置（注意：如果GoldMine的setMinePosition是封装方法，就用这个；否则直接用setPosition）
                // 二选一：根据你的GoldMine类实现来定
                placedGoldMine->setPosition(snappedPos); // 直接设置位置（Node自带方法）
                // placedGoldMine->setMinePosition(snappedPos); // 如果你封装了setMinePosition方法

                background_sprite_->addChild(placedGoldMine, 15); // 添加到背景
                placedGoldMines.push_back(placedGoldMine); // 保存到金矿列表

                placedGoldMine->playSuccessBlink(); // 成功闪烁反馈

                // 打印日志验证
                log("成功放置金矿，产金速度：%.1f，位置：(%.2f, %.2f)",
                    placedGoldMine->getGoldSpeed(),
                    snappedPos.x, snappedPos.y);
            }
        }
        else {
            // 无效区域：创建金矿对象并执行失败反馈
            std::string textureName = (draggingItem == goldMineBtn ? "GoldMineLv1.png" : "CannonLv10.png");
            auto failGoldMine = GoldMine::create(textureName);
            if (failGoldMine) {
                failGoldMine->setPosition(snappedPos); // 同上，二选一
                // failGoldMine->setMinePosition(snappedPos);

                background_sprite_->addChild(failGoldMine, 15);
                failGoldMine->playFailBlinkAndRemove(); // 红色闪烁+销毁

                log("尝试在无效位置放置金矿: (%.2f, %.2f)", snappedPos.x, snappedPos.y);
            }
        }
        
        // 重置拖拽状态
        isDragging = false;
        draggingItem = nullptr;
    }
    else if (isMovingBuilding && movingBuilding) {
        // 处理建筑移动结束
        // 记录移动后的位置
        log("建筑移动到新位置: (%.2f, %.2f)", movingBuilding->getPosition().x, movingBuilding->getPosition().y);
        
        // 将建筑透明度恢复为完全不透明
        movingBuilding->setOpacity(255);
        
        // 将建筑Z轴层级恢复为正常
        background_sprite_->reorderChild(movingBuilding, 15);
        
        // 重置移动状态
        isMovingBuilding = false;
        movingBuilding = nullptr;
    } else if (zoom_manager_) {
        zoom_manager_->onTouchEnded(touch, event);
    }
}

void SecondScene::onTouchCancelled(Touch* touch, Event* event)
{
    if (isDragging && draggingItem) {
        // --- 替换：获取GoldMine预览对象（代替原来的Sprite） ---
        GoldMine* dragMinePreview = static_cast<GoldMine*>(draggingItem->getUserData());
        if (dragMinePreview) {
            // 移除金矿预览对象
            dragMinePreview->removeFromParentAndCleanup(true);
            draggingItem->setUserData(nullptr);
        }

        // 重置拖拽状态
        isDragging = false;
        draggingItem = nullptr;
    }
    else if (isMovingBuilding && movingBuilding) {
        // --- 适配：操作GoldMine对象（代替原来的Sprite） ---
        // 将金矿透明度恢复为完全不透明（操作GoldMine节点或其精灵）
        movingBuilding->setOpacity(255); // 操作金矿节点（推荐）
        // 可选：如果需要只操作精灵，取消下面注释
        // Sprite* mineSprite = movingBuilding->getSprite();
        // if (mineSprite) {
        //     mineSprite->setOpacity(255);
        // }

        // 恢复金矿Z轴层级（操作GoldMine对象）
        background_sprite_->reorderChild(movingBuilding, 15);

        // 重置移动状态
        isMovingBuilding = false;
        movingBuilding = nullptr;
    }
    else if (zoom_manager_) {
        // 保留缩放管理器的逻辑
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

#endif