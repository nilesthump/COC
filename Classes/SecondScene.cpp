#if 1
#include "HelloWorldScene.h"
#include "SecondScene.h"
#include <cmath>


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


    auto houseBtn = MenuItemImage::create(
        "HelloWorld.png",
        "HelloWorld.png",
        [=](Ref* pSender) {
            log("huose");
        }
    );
    if (houseBtn) {
        houseBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - houseBtn->getContentSize().height * 0.5 / 2 - 10));
    }
    houseBtn->setScale(0.5f);


    auto storageBtn = MenuItemImage::create(
        "HelloWorld.png",
        "HelloWorld.png",
        [=](Ref* pSender) {
            log("storage");
        }
    );
    if (storageBtn) {
        storageBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - houseBtn->getContentSize().height * 0.5 * 1.5 - 10 - 10));
    }
    storageBtn->setScale(0.5f);


    auto panelMenu = Menu::create(houseBtn, storageBtn, nullptr);
    panelMenu->setPosition(Vec2::ZERO);
    panelBg->addChild(panelMenu);

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
    grid_manager_->drawDiamondGrid(background_sprite_, 50.0f);

    // 创建坐标显示标签
    coordinate_label_ = Label::createWithTTF("坐标: ", "fonts/STZhongSong_Bold.ttf", 20);
    coordinate_label_->setColor(Color3B::YELLOW);
    coordinate_label_->setPosition(Vec2(origin.x + visibleSize.width - 200, origin.y + 30));
    this->addChild(coordinate_label_, 2);

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

    return true;
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
    return zoom_manager_->onTouchBegan(touch, event);
}

void SecondScene::onTouchMoved(Touch* touch, Event* event)
{
    if (zoom_manager_)
    {
        zoom_manager_->onTouchMoved(touch, event);
    }
}

void SecondScene::onTouchEnded(Touch* touch, Event* event)
{

    if (zoom_manager_)
    {
        zoom_manager_->onTouchEnded(touch, event);
    }
}

void SecondScene::onTouchCancelled(Touch* touch, Event* event)
{

    if (zoom_manager_)
    {
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
    if (zoom_manager_)
    {
        zoom_manager_->onTouchesBegan(touches, event);
    }
}


void SecondScene::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
    if (zoom_manager_)
    {
        zoom_manager_->onTouchesMoved(touches, event);
    }
}


void SecondScene::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{

    if (zoom_manager_)
    {
        zoom_manager_->onTouchesEnded(touches, event);
    }
}


void SecondScene::onTouchesCancelled(const std::vector<Touch*>& touches, Event* event)
{
    if (zoom_manager_)
    {
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