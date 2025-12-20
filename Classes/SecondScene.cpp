#if 1
#include "HelloWorldScene.h"
#include "SecondScene.h"
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


    // 创建房子按钮
    houseBtn = MenuItemImage::create(
        "ArcherTowerLv10.png",
        "ArcherTowerLv10.png",
        [=](Ref* pSender) {
            if (!isDragging) {
                log("house ");
                isDragging = true;
                draggingItem = houseBtn;
                dragStartPosition = houseBtn->getPosition();
                
                // 创建拖拽副本
                auto dragSprite = Sprite::create("ArcherTowerLv10.png");
                if (dragSprite) {
                    // 获取按钮在屏幕上的位置
                    Vec2 worldPos = houseBtn->getParent()->convertToWorldSpace(houseBtn->getPosition());
                    // 转换为背景精灵的本地坐标
                    Vec2 localPos = background_sprite_->convertToNodeSpace(worldPos);
                    dragSprite->setScale(1.0f);
                    dragSprite->setPosition(localPos);
                    background_sprite_->addChild(dragSprite, 10);
                    houseBtn->setUserData(dragSprite);
                }
                
                // 移除隐藏按钮的代码，这样在拖拽时原始按钮仍然可见
                // houseBtn->setVisible(false);
            }
        }
    );
    if (houseBtn) {
        houseBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - houseBtn->getContentSize().height * 0.5 / 2 - 10));
    }
    houseBtn->setScale(0.5f);


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
        storageBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - houseBtn->getContentSize().height * 0.5 * 1.5 - 10 - 10));
    }
    storageBtn->setScale(0.5f);


    auto panelMenu = Menu::create(houseBtn, storageBtn, nullptr);
    panelMenu->setPosition(Vec2::ZERO);
    panelBg->addChild(panelMenu);

    // 初始化拖拽状态
    isDragging = false;
    draggingItem = nullptr;

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
        // 增加圣水数量
        g_elixirCount++;
        
        // 增加金币数量
        g_goldCount++;

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
    // 只处理拖拽状态下的逻辑和缩放管理器的逻辑
    
    if (isDragging) {
        return true; // 正在拖拽时返回true，保持事件被捕获
    }
    
    // 如果没有拖拽，则使用缩放管理器的触摸处理
    return zoom_manager_->onTouchBegan(touch, event);
}

void SecondScene::onTouchMoved(Touch* touch, Event* event)
{
    if (isDragging && draggingItem) {
        // 移动拖拽的精灵
        Sprite* dragSprite = static_cast<Sprite*>(draggingItem->getUserData());
        if (dragSprite) {
            // 将屏幕坐标转换为相对于背景精灵的本地坐标
            Vec2 localPos = background_sprite_->convertToNodeSpace(touch->getLocation());
            
            // 获取网格单元格大小
            float gridCellSizeX = grid_manager_->getGridCellSizeX();
            float gridCellSizeY = grid_manager_->getGridCellSizeY();
            
            // 将坐标按网格单元格大小的整数倍进行向上取整
            float snappedX = ceil(localPos.x / gridCellSizeX) * gridCellSizeX;
            float snappedY = ceil(localPos.y / gridCellSizeY) * gridCellSizeY;
            
            // 设置拖拽精灵的位置为网格对齐的位置
            dragSprite->setPosition(Vec2(snappedX, snappedY));
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
        
        // 获取拖拽的精灵
        Sprite* dragSprite = static_cast<Sprite*>(draggingItem->getUserData());
        if (dragSprite) {
            // 移除拖拽的精灵
            dragSprite->removeFromParentAndCleanup(true);
            draggingItem->setUserData(nullptr);
        }
        
        // 检查是否在有效区域内放置
        // 使用原始屏幕坐标进行菱形坐标转换检查
        Vec2 diamondPos = convertScreenToDiamond(screenPos);
        if (isInDiamond(diamondPos)) {
            // 在有效区域内，可以放置
            
            // 根据拖拽的按钮获取相应的纹理名称
            std::string textureName = (draggingItem == houseBtn ? "ArcherTowerLv10.png" : "CannonLv10.png");
            
            // 创建一个放置后的精灵
            auto placedSprite = Sprite::create(textureName);
            if (placedSprite) {
                placedSprite->setScale(1.0f); // 与拖拽时保持一致的大小
                placedSprite->setPosition(snappedPos); // 使用网格对齐的位置
                background_sprite_->addChild(placedSprite, 15); // 设置更高的Z轴层级，确保显示在网格之上
                
                // 添加成功的视觉反馈（闪烁）
                placedSprite->runAction(Blink::create(1.0f, 3));
            }
            
            // 记录成功放置的位置
            log("成功放置建筑物在位置: (%.2f, %.2f)", snappedPos.x, snappedPos.y);
            
            // 添加到已放置的建筑物列表（已移除，因为该成员变量未在类中声明）
            // placedBuildings.push_back(placedSprite);
        } else {
            // 不在有效区域内，不能放置
            
            // 根据拖拽的按钮获取相应的纹理名称
            std::string textureName = (draggingItem == houseBtn ? "ArcherTowerLv10.png" : "CannonLv10.png");
            
            // 创建一个失败放置的精灵
            auto failSprite = Sprite::create(textureName);
            if (failSprite) {
                failSprite->setScale(1.0f); // 与拖拽时保持一致的大小
                failSprite->setPosition(snappedPos); // 使用网格对齐的位置
                failSprite->setColor(Color3B::RED); // 设置为红色
                background_sprite_->addChild(failSprite, 15); // 设置更高的Z轴层级，确保显示在网格之上
                
                // 添加红色闪烁效果作为失败的视觉反馈
                failSprite->runAction(Sequence::create(
                    Blink::create(0.5f, 3),
                    RemoveSelf::create(),
                    nullptr
                ));
            }
            
            // 记录失败放置的位置
            log("尝试在无效位置放置建筑物: (%.2f, %.2f)", snappedPos.x, snappedPos.y);
        }
        
        // 重置拖拽状态
        isDragging = false;
        draggingItem = nullptr;
    }
    else if (zoom_manager_) {
        zoom_manager_->onTouchEnded(touch, event);
    }
}

void SecondScene::onTouchCancelled(Touch* touch, Event* event)
{
    // 处理触摸取消，类似于结束但不执行放置逻辑
    if (isDragging && draggingItem) {
        // 获取拖拽的精灵
        Sprite* dragSprite = static_cast<Sprite*>(draggingItem->getUserData());
        if (dragSprite) {
            // 移除拖拽的精灵
            dragSprite->removeFromParentAndCleanup(true);
            draggingItem->setUserData(nullptr);
        }
        
        // 恢复原按钮
        if (draggingItem == houseBtn) {
            // 不需要恢复位置和可见性，因为按钮没有被隐藏
            // houseBtn->setPosition(dragStartPosition);
            // houseBtn->setVisible(true);
        } else if (draggingItem == storageBtn) {
            // 不需要恢复位置和可见性，因为按钮没有被隐藏
            // storageBtn->setPosition(dragStartPosition);
            // storageBtn->setVisible(true);
        }
        
        // 重置拖拽状态
        isDragging = false;
        draggingItem = nullptr;
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

#endif