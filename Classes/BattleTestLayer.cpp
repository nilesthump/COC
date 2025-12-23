//宏观是触摸与鼠标监听配到整个layer，background添加到layer一起缩放移动
//game_world(node)作为所有part的父节点是background的孩子，其他part加到game_world中

//于是角色的setposition位置就是内部grid的逻辑位置，通过convertGridToLocal转换成对于game_world（考虑了缩放之类的）的本地位置
//由于game_word是background的孩子，所以BattleUnit更新显示位置时调用convertGridToLocal就可以正确显示
#include "BattleTestLayer.h"
#include "UnitFactory.h"
#include "HelloWorldScene.h"
#include "ConvertTest.h"
#include "BattleResultLayer.h"
#include "ccUtils.h"
#include <ctime>

USING_NS_CC;
void DebugFullGrid(Node* gameWorld, Sprite* backgroundSprite, int gridStep, bool showLabels);
//错误加载
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

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

    CCLOG("=== BattleTestLayer init START ===");

    //基本初始化
    next_barbarian_id_ = 1;
    last_click_time_ = 0;
    last_click_pos_ = Vec2::ZERO;

    //触摸事件监听
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(BattleTestLayer::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(BattleTestLayer::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(BattleTestLayer::onTouchEnded, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(BattleTestLayer::onTouchCancelled, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    //鼠标事件监听器
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseScroll = CC_CALLBACK_1(BattleTestLayer::onMouseScroll, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
    
    setupBattle();

    this->scheduleUpdate();

    return true;
}

//战斗启动！
void BattleTestLayer::setupBattle()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    //返回HelloWorldScene按钮部分
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

    auto menu = Menu::create(backItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    //1. 加载背景地图
    background_sprite_ = Sprite::create("normal(winter).jpg");
    if (background_sprite_)
    {
        CCLOG(" Background loaded successfully");
        background_sprite_->setPosition(Vec2(
            visibleSize.width / 2 + origin.x,
            visibleSize.height / 2 + origin.y
        ));

        float scale = visibleSize.width / background_sprite_->getContentSize().width * 1.5f;
        background_sprite_->setScale(scale);

        this->addChild(background_sprite_, 0);

        //2.创建game_world容器 
        game_world_ = Node::create();
        //这是系统的setPosition，直接绑定了game_world_和background的坐标关系
        //注意区分实际放置导入grid逻辑坐标的SetPosition（S大写)
        game_world_->setPosition(background_sprite_->getContentSize().width / 2,
            background_sprite_->getContentSize().height / 2);
        background_sprite_->addChild(game_world_, 1);

        //3.初始化缩放滚动管理器
        zoom_manager_ = ZoomScrollManager::create(background_sprite_, 0.5f, 2.0f);
        this->addChild(zoom_manager_);

        //4.初始化菱形地图（草坪）管理器
        int left_x = 667;
        int right_x = 3705 - 556;
        int top_y = 264;
        int bottom_y = 2545 - 471;

        float diamond_width = right_x - left_x;
        float diamond_height = bottom_y - top_y;

        Vec2 diamond_center_absolute = Vec2(
            (left_x + right_x) / 2.0f,
            (top_y + bottom_y) / 2.0f
        );

        Vec2 diamond_center = diamond_center_absolute - Vec2(
            background_sprite_->getContentSize().width / 2,
            background_sprite_->getContentSize().height / 2
        );

        //草坪地图部分也作为game_world的孩子
        grid_manager_ = DiamondGridManager::create(diamond_center, diamond_width, diamond_height, 50);
        game_world_->addChild(grid_manager_, 1);

        //5.初始化战斗管理器
        battle_manager_ = std::make_unique<BattleManager>();
        //这里的兵力总数最大由大本营携带过来，这里先预设
        battle_manager_->SetTotalHeroes(10);

        //debug放置格子线条，可删除
        DebugFullGrid(game_world_, background_sprite_, 2, true);

        placeDefender();
        CCLOG("Setup complete, cannon should be visible now");

        //6.一些坐标标签提示辅助，可以删除
        coordinates_label_ = Label::createWithSystemFont("Double-click to place Barbarian", "Arial", 20);
        coordinates_label_->setColor(Color3B::YELLOW);
        coordinates_label_->setPosition(Vec2(origin.x + 200, origin.y + visibleSize.height - 80));
        coordinates_label_->setAnchorPoint(Vec2(0, 1));
        this->addChild(coordinates_label_, 100);

        //时间倒计时标签
        countdown_label_ = Label::createWithSystemFont(
            "Time: 180",
            "Arial",
            24
        );
        countdown_label_->setColor(Color3B::RED);

        //
        countdown_label_->setAnchorPoint(Vec2(0, 1));
        countdown_label_->setPosition(Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - 20
        ));
        this->addChild(countdown_label_, 1000);
    }
}

//固定放置（直接放逻辑位置）
void BattleTestLayer::placeDefender()
{
    BattleUnit* cannon_unit1 = UnitFactory::CreateCannon(1, game_world_, background_sprite_);
    if (cannon_unit1)
    {
        cannon_unit1->SetPositionDefender(10, 10);
        battle_manager_->AddUnit(std::unique_ptr<BattleUnit>(cannon_unit1), false);
    }

    BattleUnit* archer_tower_unit1 = UnitFactory::CreateArcherTower(1, game_world_, background_sprite_);
    if (archer_tower_unit1)
    {
        archer_tower_unit1->SetPositionDefender(15, 33);
        battle_manager_->AddUnit(std::unique_ptr<BattleUnit>(archer_tower_unit1), false);
    }

}

//在给定点的位置放置野蛮人，作为双击鼠标点击的反应
//应该允许同一位置放置多个野蛮人
void BattleTestLayer::placeBarbarianAt(float gridX, float gridY)
{

    if (!battle_manager_->CanDeployAttacker())
    {
        
        Vec2 localPos = ConvertTest::convertGridToLocal(gridX, gridY, background_sprite_);
        showDeployHintAtWorldPos(localPos);
        return;
    }

    if (gridX < -0.5f || gridX > 49.5f || gridY < -0.5f || gridY > 49.5f)
    {
        
        //创建临时单位用于失败反馈
        BattleUnit* tempBarbarian = UnitFactory::CreateBarbarian(
            0, game_world_, background_sprite_);

        if (tempBarbarian)
        {
            tempBarbarian->SetPositionAttacker(gridX, gridY);

            //播放失败动画（动画结束后会自动清理视觉组件）
            tempBarbarian->PlayPlacementFailAnimation();

            //1秒后删除临时对象
            auto delay = DelayTime::create(1.0f);
            auto cleanup = CallFunc::create([tempBarbarian]()
                {
                    delete tempBarbarian;
                });
            auto sequence = Sequence::create(delay, cleanup, nullptr);
            this->runAction(sequence);
        }
        return;
    }
    // 无论是否被占用，都允许放置
    BattleUnit* barbarian = UnitFactory::CreateBarbarian(
        1, game_world_, background_sprite_);

    if (barbarian)
    {
        barbarian->SetPositionAttacker(gridX, gridY);
        battle_manager_->AddUnit(std::unique_ptr<BattleUnit>(barbarian), true);
        barbarian_positions_.push_back(
            std::make_pair(next_barbarian_id_, Vec2(gridX, gridY)));
        updateCoordinatesDisplay();
        next_barbarian_id_++;
    }
}

//更新坐标显示
void BattleTestLayer::updateCoordinatesDisplay()
{
    if (!coordinates_label_ || !battle_manager_)
        return;

    std::string display_text = "=== Barbarian List ===\n";

    if (barbarian_positions_.empty())
    {
        display_text += "(No units placed yet)\n";
        display_text += "Tip: Double-click to place unit\n";
    }
    else
    {
        for (const auto& pos : barbarian_positions_)
        {
            display_text += StringUtils::format(
                "Barbarian #%d: (%.2f, %.2f)\n",
                pos.first,
                pos.second.x,
                pos.second.y
            );
        }
    }

    display_text += "\n";
    display_text += StringUtils::format(
        "Placed: %d / %d",
        battle_manager_->GetHeroesPlaced(),
        battle_manager_->GetMaxHeros()
    );

    coordinates_label_->setString(display_text);
}

//整体更新，判断战斗结束等，需要拓展
void BattleTestLayer::update(float delta)
{
    auto remaining_time_ = battle_manager_->GetRemainTime();
    //倒计时标识显示，更新剩余时间
    if (remaining_time_ > 0.0f)
    {
        remaining_time_ -= delta;
        if (remaining_time_ < 0.0f)
            remaining_time_ = 0.0f;

        if (countdown_label_)
        {
            countdown_label_->setString(
                StringUtils::format("Time: %d", (int)ceil(remaining_time_))
            );
        }
    }

    if (battle_manager_)
    {
        battle_manager_->Update(delta);
        if (battle_manager_->HasBattleEnded() && !result_layer_shown_)
        {
            result_layer_shown_ = true;
            showBattleResultLayer(battle_manager_->GetBattleResult());
        }
    }
}

//以下是触摸和鼠标事件
bool BattleTestLayer::onTouchBegan(Touch* touch, Event* event)
{
    touch_begin_time_ = utils::gettime();
    touch_begin_pos_ = touch->getLocation();
    is_dragging_map_ = false;

    // 可选：防止拖拽污染双击
    // last_click_time_ = 0;
    return true;
}

//拖拽和双击判定，主要在ended
void BattleTestLayer::onTouchMoved(Touch* touch, Event* event)
{
    Vec2 current = touch->getLocation();
    float distance = current.distance(touch_begin_pos_);

    // 超过阈值，判定为拖拽
    if (!is_dragging_map_ && distance > 10.0f)
    {
        is_dragging_map_ = true;
        zoom_manager_->onTouchBegan(touch, event); // 开始拖地图
    }

    if (is_dragging_map_ && zoom_manager_)
    {
        zoom_manager_->onTouchMoved(touch, event);
    }
}

void BattleTestLayer::onTouchEnded(Touch* touch, Event* event)
{
    double now = utils::gettime();
    double duration = now - touch_begin_time_;
    Vec2 end_pos = touch->getLocation();
    float distance = end_pos.distance(touch_begin_pos_);

    // 如果是拖拽，结束拖拽即可
    if (is_dragging_map_)
    {
        zoom_manager_->onTouchEnded(touch, event);
        is_dragging_map_ = false;
        return;
    }

    // 过滤掉按太久 or 移动太多的情况
    if (duration > 0.3f || distance > 15.0f)
        return;

    // 双击判断
    double interval = now - last_click_time_;
    float click_distance = last_click_pos_.distance(end_pos);

    if (interval > 0.1 && interval < 0.5 && click_distance < 30.0f)
    {
        //双击成立
        CCLOG("[DOUBLE CLICK]");

        //直接从点击屏幕位置到gird，真好
        Vec2 gridPos = ConvertTest::convertScreenToGrid(
            end_pos, background_sprite_, game_world_);

        //这里调用了放置具体位置的barbarianAt函数
        placeBarbarianAt(gridPos.x,gridPos.y);

        //重置
        last_click_time_ = 0;
        last_click_pos_ = Vec2::ZERO;
    }
    else
    {
        // 记录为第一次点击
        last_click_time_ = now;
        last_click_pos_ = end_pos;
    }
}

void BattleTestLayer::onTouchCancelled(Touch* touch, Event* event)
{
    if (is_dragging_map_)
    {
        zoom_manager_->onTouchEnded(touch, event);
    }

    is_dragging_map_ = false;
}

void BattleTestLayer::onMouseScroll(EventMouse* event)
{
    if (zoom_manager_)
    {
        zoom_manager_->onMouseScroll(event);
    }
}

//展示兵力已经满了不让再放置的提示，提示在点击位置附近
void BattleTestLayer::showDeployHintAtWorldPos(const Vec2& worldPos)
{

    auto label = Label::createWithSystemFont("NO MORE UNITS", "fonts/Marker Felt.ttf", 20);
    label->setColor(Color3B::RED);

    // worldPos 是 game_world_ 的本地坐标
    label->setPosition(worldPos + Vec2(0, 20));
    label->setOpacity(255);

    // 挂在 game_world_，跟着缩放 / 拖拽
    game_world_->addChild(label, 5000);

    // 飘字动画
    auto moveUp = MoveBy::create(0.8f, Vec2(0, 40));
    auto fadeOut = FadeOut::create(0.8f);
    auto spawn = Spawn::create(moveUp, fadeOut, nullptr);

    auto cleanup = CallFunc::create([label]()
        {
            label->removeFromParent();
        });

    label->runAction(Sequence::create(spawn, cleanup, nullptr));
}

//展示结算画面
void BattleTestLayer::showBattleResultLayer(BattleResult result)
{
    auto layer = BattleResultLayer::create(result);
    this->addChild(layer, 10000);
}

//返回HelloWorldScene触发
void BattleTestLayer::menuFirstCallback(Ref* pSender)
{
    Director::getInstance()->replaceScene(HelloWorld::createScene());
}

//Debug用的放置条格子，辅助用
void DebugFullGrid(Node* gameWorld, Sprite* backgroundSprite, int gridStep, bool showLabels)
{
    if (!gameWorld || !backgroundSprite)
        return;

    auto gridContainer = Node::create();
    gridContainer->setName("GridDebugContainer");
    gameWorld->addChild(gridContainer, 1000);

    int pointCount = 0;

    //0-49
    for (int gridX = 0; gridX <= 49; gridX += gridStep)
    {
        for (int gridY = 0; gridY <= 49; gridY += gridStep)
        {
            Vec2 local_pos = ConvertTest::convertGridToLocal(gridX, gridY, backgroundSprite);

            Color3B color;
            float markerSize = 3.0f;

            // 特殊点标记
            if (gridX == 0 && gridY == 0)
            {
                color = Color3B::RED;
                markerSize = 6.0f;
            }
            else if (gridX == 49 && gridY == 49)  // 改为49
            {
                color = Color3B::BLUE;
                markerSize = 6.0f;
            }
            else if (gridX == 0 && gridY == 49)  // 改为49
            {
                color = Color3B::GREEN;
                markerSize = 6.0f;
            }
            else if (gridX == 49 && gridY == 0)  // 改为49
            {
                color = Color3B::YELLOW;
                markerSize = 6.0f;
            }
            else if (gridX == 24 && gridY == 24)  // 中心点改为24
            {
                color = Color3B::WHITE;
                markerSize = 5.0f;
            }
            else if (gridX == 0 || gridX == 49 || gridY == 0 || gridY == 49)  // 边界改为49
            {
                color = Color3B::ORANGE;
                markerSize = 4.0f;
            }
            else
            {
                color = Color3B(150, 150, 150);
                markerSize = 3.0f;
            }

            auto drawNode = DrawNode::create();
            drawNode->drawSolidCircle(Vec2::ZERO, markerSize, 0, 16, Color4F(color));
            drawNode->setPosition(local_pos);
            gridContainer->addChild(drawNode);

            // 标签显示逻辑
            bool shouldShowLabel = false;
            if (gridStep >= 4)
            {
                shouldShowLabel = showLabels;
            }
            else
            {
                shouldShowLabel = showLabels && (
                    (gridX == 0 || gridX == 49 || gridX == 24) &&  // 改为49和24
                    (gridY == 0 || gridY == 49 || gridY == 24)
                    );
            }

            if (shouldShowLabel)
            {
                auto label = Label::createWithSystemFont(
                    StringUtils::format("(%d,%d)", gridX, gridY),
                    "Arial",
                    14
                );
                label->setColor(color);
                label->setPosition(local_pos + Vec2(0, 20));
                gridContainer->addChild(label);
            }

            pointCount++;
        }
    }

    CCLOG("[DEBUG] Drew %d grid points", pointCount);

    // 绘制网格线
    auto gridLines = DrawNode::create();
    gridLines->setName("GridLines");

    for (int gridY = 0; gridY <= 49; gridY += (gridStep * 2))  // 改为49
    {
        Vec2 start = ConvertTest::convertGridToLocal(0, gridY, backgroundSprite);
        Vec2 end = ConvertTest::convertGridToLocal(49, gridY, backgroundSprite);  // 改为49
        gridLines->drawLine(start, end, Color4F(0.5f, 0.5f, 0.5f, 0.5f));
    }

    for (int gridX = 0; gridX <= 49; gridX += (gridStep * 2))  // 改为49
    {
        Vec2 start = ConvertTest::convertGridToLocal(gridX, 0, backgroundSprite);
        Vec2 end = ConvertTest::convertGridToLocal(gridX, 49, backgroundSprite);  // 改为49
        gridLines->drawLine(start, end, Color4F(0.5f, 0.5f, 0.5f, 0.5f));
    }

    gridContainer->addChild(gridLines, -1);

    // 绘制边界
    auto boundaryDraw = DrawNode::create();
    boundaryDraw->setName("Boundary");

    Vec2 corners[] = {
        ConvertTest::convertGridToLocal(0, 0, backgroundSprite),
        ConvertTest::convertGridToLocal(49, 0, backgroundSprite),    // 改为49
        ConvertTest::convertGridToLocal(49, 49, backgroundSprite),   // 改为49
        ConvertTest::convertGridToLocal(0, 49, backgroundSprite)     // 改为49
    };

    for (int i = 0; i < 4; i++)
    {
        boundaryDraw->drawLine(
            corners[i],
            corners[(i + 1) % 4],
            Color4F::RED
        );
    }

    gridContainer->addChild(boundaryDraw, 10);
}