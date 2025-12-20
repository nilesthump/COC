#include "BattleTestLayer.h"
#include "UnitFactory.h"
#include "HelloWorldScene.h"

USING_NS_CC;

Scene* BattleTestLayer::createScene()
{
    auto scene = Scene::create();
    auto layer = BattleTestLayer::create();
    scene->addChild(layer);
    return scene;
}

bool BattleTestLayer::init()
{
    if (!Layer::init())
    {
        return false;
    }

    
    // 设置场景
    setupBattleScene();

    // 添加触摸监听
    auto touch_listener = EventListenerTouchOneByOne::create();
    touch_listener->setSwallowTouches(true);
    touch_listener->onTouchBegan = CC_CALLBACK_2(BattleTestLayer::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);

    // 启动更新循环
    this->scheduleUpdate();

    return true;
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

void BattleTestLayer::setupBattleScene()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto backItem = MenuItemImage::create("btn_normal.png", "btn_pressed.png",
        CC_CALLBACK_1(BattleTestLayer::menuFirstCallback, this));
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

    auto menu = Menu::create(backItem,nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // 1. 加载背景地图（直接添加到Layer，像SecondScene一样）
    background_sprite_ = Sprite::create("normal(winter).jpg");
    if (background_sprite_)
    {
        // 背景精灵居中
        background_sprite_->setPosition(Vec2(
            visibleSize.width / 2 + origin.x,
            visibleSize.height / 2 + origin.y
        ));

        // 设置缩放（和SecondScene相同）
        float scale = visibleSize.width / background_sprite_->getContentSize().width * 1.5f;
        background_sprite_->setScale(scale);

        this->addChild(background_sprite_, 0);
    }

    // 2. 创建游戏世界容器（作为背景的子节点，这样会随着背景一起移动缩放）
    game_world_ = Node::create();
    // 设置游戏世界位置为背景图像的中心
    game_world_->setPosition(background_sprite_->getContentSize().width / 2, background_sprite_->getContentSize().height / 2);
    background_sprite_->addChild(game_world_, 1); // 添加到背景中

    // 3. 初始化缩放滚动管理器（和SecondScene一样）
    zoom_manager_ = ZoomScrollManager::create(background_sprite_, 0.5f, 2.0f);
    this->addChild(zoom_manager_);

    // 4. 初始化菱形网格管理器（使用您提供的参数）
    int left_x = 667;
    int right_x = 3705 - 556;
    int top_y = 264;
    int bottom_y = 2545 - 471;

    float diamond_width = right_x - left_x;
    float diamond_height = bottom_y - top_y;

    // 计算菱形中心相对于背景精灵中心的位置
    Vec2 diamond_center_absolute = Vec2(
        (left_x + right_x) / 2.0f,
        (top_y + bottom_y) / 2.0f
    );

    // 这里的关键：菱形中心是相对于背景精灵的本地坐标
    Vec2 diamond_center = diamond_center_absolute - Vec2(
        background_sprite_->getContentSize().width / 2,
        background_sprite_->getContentSize().height / 2
    );

    // 创建菱形网格管理器，添加到game_world_
    grid_manager_ = DiamondGridManager::create(diamond_center, diamond_width, diamond_height, 44);
    game_world_->addChild(grid_manager_, 1);

    // 5. 初始化战斗管理器
    battle_manager_ = std::make_unique<BattleManager>();
    battle_manager_->SetBattleActive(true);

    placeCannon();
    placeBarbarian();
}

void BattleTestLayer::placeBarbarian()
{
    // 创建Barbarian(攻击者)
    BattleUnit* barbarian = UnitFactory::CreateCompleteBarbarian(1, game_world_);
    if (barbarian)
    {
        // 设置背景精灵，用于坐标转换
        barbarian->SetBackgroundSprite(background_sprite_);
        // 这里需要将屏幕坐标转换为网格坐标，暂时使用示例位置
        barbarian->SetPosition(10, 10);
        battle_manager_->AddUnit(std::unique_ptr<BattleUnit>(barbarian), true);
    }
}

void BattleTestLayer::placeCannon()
{
    cannon_unit_ = UnitFactory::CreateCompleteCannon(1, game_world_);

    cannon_unit_->SetBackgroundSprite(background_sprite_);

    cannon_unit_->SetPosition(21, 21);

    battle_manager_->AddUnit(std::unique_ptr<BattleUnit>(cannon_unit_), false);
}


void BattleTestLayer::update(float delta)
{
    // 更新战斗管理器
    if (battle_manager_)
    {
        battle_manager_->Update(delta);
    }
}

void BattleTestLayer::menuFirstCallback(Ref* pSender)
{

    Director::getInstance()->replaceScene(HelloWorld::createScene());
}