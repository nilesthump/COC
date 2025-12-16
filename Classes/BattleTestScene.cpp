#include "BattleTestScene.h"
#include "UnitFactory.h"
#include "cocos2d.h"
#include <memory>

USING_NS_CC;

Scene* BattleTestScene::createScene()
{
    auto scene = Scene::create();
    auto layer = BattleTestScene::create();
    scene->addChild(layer);
    return scene;
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool BattleTestScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // ⭐ 1. 创建游戏世界容器
    gameWorld_ = Node::create();
    gameWorld_->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    gameWorld_->setScale(1.0f);
    this->addChild(gameWorld_, 0);

    current_scale_ = 1.0f;  // 初始缩放比例

    // 初始化战斗管理器
    battleMgr_ = std::make_unique<BattleManager>();
    battleMgr_->SetBattleActive(true);

    // ⭐ 2. 地图背景添加到游戏世界容器（而不是this）
    background_sprite_ = Sprite::create("normal(winter).jpg");
    if (background_sprite_ == nullptr)
    {
        problemLoading("'normal(winter).jpg'");
        // 这里不要return false，继续创建单位看看
    }
    else
    {
        // 计算初始缩放
        float scale = visibleSize.width / background_sprite_->getContentSize().width * 1.5f;
        background_sprite_->setScale(scale);
        background_sprite_->setPosition(0, 0);  // ⭐ 相对于gameWorld_中心
        gameWorld_->addChild(background_sprite_, 0);

        // 更新当前缩放
        current_scale_ = scale;
    }

    // --------------------------
    // 初始化拖拽和缩放
    // --------------------------
    is_dragging_ = false;
    drag_start_position_ = Vec2::ZERO;
    world_start_position_ = gameWorld_->getPosition();  // ⭐ 改为world_start_position_

    is_scaling_ = false;
    previous_distance_ = 0.0f;
    scale_center_ = Vec2::ZERO;
    min_scale_ = 0.5f;
    max_scale_ = 2.0f;

    // ---------------------------
    // 添加事件监听器
    // ---------------------------
    auto touch_listener = EventListenerTouchOneByOne::create();
    touch_listener->setSwallowTouches(true);
    touch_listener->onTouchBegan = CC_CALLBACK_2(BattleTestScene::onTouchBegan, this);
    touch_listener->onTouchMoved = CC_CALLBACK_2(BattleTestScene::onTouchMoved, this);
    touch_listener->onTouchEnded = CC_CALLBACK_2(BattleTestScene::onTouchEnded, this);
    touch_listener->onTouchCancelled = CC_CALLBACK_2(BattleTestScene::onTouchCancelled, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);

    // 鼠标滚轮缩放
    auto mouse_listener = EventListenerMouse::create();
    mouse_listener->onMouseScroll = CC_CALLBACK_1(BattleTestScene::onMouseScroll, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouse_listener, this);

    // 双指缩放
    auto multi_touch_listener = EventListenerTouchAllAtOnce::create();
    multi_touch_listener->onTouchesBegan = CC_CALLBACK_2(BattleTestScene::onTouchesBegan, this);
    multi_touch_listener->onTouchesMoved = CC_CALLBACK_2(BattleTestScene::onTouchesMoved, this);
    multi_touch_listener->onTouchesEnded = CC_CALLBACK_2(BattleTestScene::onTouchesEnded, this);
    multi_touch_listener->onTouchesCancelled = CC_CALLBACK_2(BattleTestScene::onTouchesCancelled, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(multi_touch_listener, this);

    // ---------------------------
    // ⭐ 3. 使用工厂创建单位，parent参数改为gameWorld_
    // ---------------------------
    // 创建Barbarian(攻击者) 
    BattleUnit* barbarian = UnitFactory::CreateCompleteBarbarian(1, gameWorld_);  // ⭐ 改为gameWorld_
    barbarian->SetPosition(100, 200);  // ⭐ 相对于gameWorld_的坐标


    // 创建Cannon(防御者)
    BattleUnit* cannon = UnitFactory::CreateCompleteCannon(1, gameWorld_);  // ⭐ 改为gameWorld_
    cannon->SetPosition(500, 200);  // ⭐ 相对于gameWorld_的坐标
 

    // 添加到战斗管理器
    battleMgr_->AddUnit(std::unique_ptr<BattleUnit>(barbarian), true);
    battleMgr_->AddUnit(std::unique_ptr<BattleUnit>(cannon), false);

    // 启动战斗循环
    this->scheduleUpdate();

    return true;
}

void BattleTestScene::update(float delta)  // ⭐ 改为float
{
    // 更新战斗管理器
    if (battleMgr_)
    {
        battleMgr_->Update(delta);
    }
}

// ⭐ 新增：以某点为中心的缩放函数
void BattleTestScene::zoomAtPosition(const cocos2d::Vec2& screenPos, float scaleFactor)
{
    float newScale = current_scale_ * scaleFactor;
    newScale = clampf(newScale, min_scale_, max_scale_);

    if (newScale == current_scale_) return;

    // 计算缩放中心在游戏世界坐标系中的位置
    Vec2 worldPos = gameWorld_->getParent()->convertToNodeSpace(screenPos);

    // 计算缩放前后该点的局部坐标变化
    Vec2 localPosBefore = (worldPos - gameWorld_->getPosition()) / current_scale_;

    // 应用新缩放
    gameWorld_->setScale(newScale);
    current_scale_ = newScale;

    // 调整位置，使缩放中心点看起来固定
    Vec2 localPosAfter = localPosBefore * newScale;
    Vec2 newWorldPos = worldPos - localPosAfter;
    gameWorld_->setPosition(newWorldPos);

    // 更新背景边界
    updateBackgroundBounds();
}

// ==================== 事件处理（需要重写）====================
bool BattleTestScene::onTouchBegan(Touch* touch, Event* event)
{
    drag_start_position_ = touch->getLocation();
    world_start_position_ = gameWorld_->getPosition();  // ⭐ 记录游戏世界位置
    is_dragging_ = true;
    return true;
}

void BattleTestScene::onTouchMoved(Touch* touch, Event* event)
{
    if (!is_dragging_) return;

    auto current_touch_position = touch->getLocation();
    auto offset = current_touch_position - drag_start_position_;

    // ⭐ 移动游戏世界（而不是背景图）
    auto new_position = world_start_position_ + offset;
    gameWorld_->setPosition(new_position);

    // 更新边界
    updateBackgroundBounds();
}

void BattleTestScene::onTouchEnded(Touch* touch, Event* event)
{
    is_dragging_ = false;
}

void BattleTestScene::onTouchCancelled(Touch* touch, Event* event)
{
    is_dragging_ = false;
}

// ⭐ 重写鼠标滚轮缩放
void BattleTestScene::onMouseScroll(EventMouse* event)
{
    float scrollY = event->getScrollY();
    float scaleFactor = scrollY > 0 ? 1.1f : 0.9f;

    // ⭐ 使用以鼠标位置为中心的缩放
    Vec2 mousePos = event->getLocationInView();
    zoomAtPosition(mousePos, scaleFactor);
}

// ⭐ 重写双指触摸开始
void BattleTestScene::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
{
    if (touches.size() == 2)
    {
        is_scaling_ = true;

        Vec2 touch1 = touches[0]->getLocation();
        Vec2 touch2 = touches[1]->getLocation();
        previous_distance_ = touch1.distance(touch2);
        scale_center_ = (touch1 + touch2) / 2;
    }
}

// ⭐ 重写双指缩放
void BattleTestScene::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
    if (!is_scaling_ || touches.size() != 2) return;

    Vec2 touch1 = touches[0]->getLocation();
    Vec2 touch2 = touches[1]->getLocation();
    float current_distance = touch1.distance(touch2);

    float scaleFactor = current_distance / previous_distance_;

    // ⭐ 使用以双指中心为中心的缩放
    zoomAtPosition(scale_center_, scaleFactor);

    previous_distance_ = current_distance;
}

void BattleTestScene::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    is_scaling_ = false;
}

void BattleTestScene::onTouchesCancelled(const std::vector<Touch*>& touches, Event* event)
{
    is_scaling_ = false;
}

// ⭐ 重写边界更新函数
void BattleTestScene::updateBackgroundBounds()
{
    // 获取当前游戏世界的缩放和位置
    float scale = gameWorld_->getScale();
    Vec2 current_position = gameWorld_->getPosition();

    // 获取可见区域
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size visible_size = Director::getInstance()->getVisibleSize();

    // 获取背景图原始大小
    if (!background_sprite_) return;
    Size background_size = background_sprite_->getContentSize();
    float background_scale = background_sprite_->getScale();

    // 计算实际显示大小（游戏世界缩放 × 背景图缩放）
    float total_scale = scale * background_scale;
    float scaled_width = background_size.width * total_scale;
    float scaled_height = background_size.height * total_scale;

    // 计算边界
    float minX = origin.x + visible_size.width / 2;
    float maxX = origin.x + visible_size.width / 2;
    float minY = origin.y + visible_size.height / 2;
    float maxY = origin.y + visible_size.height / 2;

    // 如果地图比屏幕大，则设置边界
    if (scaled_width > visible_size.width)
    {
        minX -= (scaled_width - visible_size.width) / 2;
        maxX += (scaled_width - visible_size.width) / 2;
    }

    if (scaled_height > visible_size.height)
    {
        minY -= (scaled_height - visible_size.height) / 2;
        maxY += (scaled_height - visible_size.height) / 2;
    }

    // 应用边界限制
    Vec2 new_position;
    new_position.x = clampf(current_position.x, minX, maxX);
    new_position.y = clampf(current_position.y, minY, maxY);

    // 更新游戏世界位置
    gameWorld_->setPosition(new_position);
}