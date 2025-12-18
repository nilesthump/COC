#include "ZoomScrollManager.h"

USING_NS_CC;

ZoomScrollManager* ZoomScrollManager::create(Sprite* targetSprite, float minScale, float maxScale)
{
    ZoomScrollManager* manager = new (std::nothrow) ZoomScrollManager();
    if (manager && manager->init(targetSprite, minScale, maxScale))
    {
        manager->autorelease();
        return manager;
    }
    CC_SAFE_DELETE(manager);
    return nullptr;
}

bool ZoomScrollManager::init(Sprite* targetSprite, float minScale, float maxScale)
{
    if (!Node::init())
    {
        return false;
    }

    target_sprite_ = targetSprite;
    min_scale_ = minScale;
    max_scale_ = maxScale;

    is_dragging_ = false;
    drag_start_position_ = Vec2::ZERO;
    sprite_start_position_ = Vec2::ZERO;

    is_scaling_ = false;
    previous_distance_ = 0.0f;
    scale_center_ = Vec2::ZERO;

    return true;
}

void ZoomScrollManager::updateBackgroundBounds()
{
    // 获取缩放比例
    float scale = target_sprite_->getScale();

    // 获取可见区域原点
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size visible_size = Director::getInstance()->getVisibleSize();

    // 获取背景精灵原始尺寸
    Size sprite_size = target_sprite_->getContentSize();

    // 计算缩放后的尺寸
    float scaled_width = sprite_size.width * scale;
    float scaled_height = sprite_size.height * scale;

    // 获取当前位置
    Vec2 current_position = target_sprite_->getPosition();

    // 计算允许的最小和最大位置
    float minX = origin.x + visible_size.width / 2;
    float maxX = origin.x + visible_size.width / 2;
    float minY = origin.y + visible_size.height / 2;
    float maxY = origin.y + visible_size.height / 2;

    // 如果缩放后的宽度大于可见宽度，调整X轴边界
    if (scaled_width > visible_size.width)
    {
        minX -= (scaled_width - visible_size.width) / 2;
        maxX += (scaled_width - visible_size.width) / 2;
    }

    // 如果缩放后的高度大于可见高度，调整Y轴边界
    if (scaled_height > visible_size.height)
    {
        minY -= (scaled_height - visible_size.height) / 2;
        maxY += (scaled_height - visible_size.height) / 2;
    }

    // 计算新位置并限制在边界内
    Vec2 new_position;
    new_position.x = clampf(current_position.x, minX, maxX);
    new_position.y = clampf(current_position.y, minY, maxY);

    // 更新背景精灵位置
    target_sprite_->setPosition(new_position);
}

bool ZoomScrollManager::onTouchBegan(Touch* touch, Event* event)
{
    drag_start_position_ = touch->getLocation();
    sprite_start_position_ = target_sprite_->getPosition();
    is_dragging_ = true;
    return true;
}

void ZoomScrollManager::onTouchMoved(Touch* touch, Event* event)
{
    if (!is_dragging_)
        return;

    auto current_touch_position = touch->getLocation();
    auto offset = current_touch_position - drag_start_position_;

    auto new_position = sprite_start_position_ + offset;

    auto visible_size = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    float sprite_width = target_sprite_->getContentSize().width * target_sprite_->getScale();
    float sprite_height = target_sprite_->getContentSize().height * target_sprite_->getScale();

    // 计算允许的最小和最大位置
    float minX = origin.x + visible_size.width / 2;
    float maxX = origin.x + visible_size.width / 2;
    float minY = origin.y + visible_size.height / 2;
    float maxY = origin.y + visible_size.height / 2;

    // 如果精灵宽度大于可见宽度，调整X轴边界
    if (sprite_width > visible_size.width)
    {
        minX -= (sprite_width - visible_size.width) / 2;
        maxX += (sprite_width - visible_size.width) / 2;
    }

    // 如果精灵高度大于可见高度，调整Y轴边界
    if (sprite_height > visible_size.width)
    {
        minY -= (sprite_height - visible_size.height) / 2;
        maxY += (sprite_height - visible_size.height) / 2;
    }

    // 限制新位置在边界内
    new_position.x = clampf(new_position.x, minX, maxX);
    new_position.y = clampf(new_position.y, minY, maxY);

    // 更新精灵位置
    target_sprite_->setPosition(new_position);
}

void ZoomScrollManager::onTouchEnded(Touch* touch, Event* event)
{
    is_dragging_ = false;
}

void ZoomScrollManager::onTouchCancelled(Touch* touch, Event* event)
{
    is_dragging_ = false;
}

void ZoomScrollManager::onMouseScroll(EventMouse* event)
{
    // 获取鼠标滚轮滚动值
    float scrollY = event->getScrollY();

    // 根据滚动方向确定缩放因子
    float scale_factor = scrollY > 0 ? 1.1f : 0.9f;

    // 获取当前缩放比例
    float current_scale = target_sprite_->getScale();

    // 计算新缩放比例
    float new_scale = current_scale * scale_factor;

    // 限制缩放比例在最小最大值之间
    new_scale = clampf(new_scale, min_scale_, max_scale_);

    // 应用新缩放比例
    target_sprite_->setScale(new_scale);

    // 更新边界
    updateBackgroundBounds();
}

void ZoomScrollManager::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
{
    if (touches.size() == 2)
    {
        // 开始缩放
        is_scaling_ = true;

        // 获取两个触摸点位置
        Vec2 touch1 = touches[0]->getLocation();
        Vec2 touch2 = touches[1]->getLocation();
        previous_distance_ = touch1.distance(touch2);

        // 计算缩放中心
        scale_center_ = (touch1 + touch2) / 2;
    }
}

void ZoomScrollManager::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
    if (!is_scaling_ || touches.size() != 2)
        return;

    // 获取两个触摸点当前位置
    Vec2 touch1 = touches[0]->getLocation();
    Vec2 touch2 = touches[1]->getLocation();
    float current_distance = touch1.distance(touch2);

    // 计算缩放因子
    float scale_factor = current_distance / previous_distance_;

    // 获取当前缩放比例
    float current_scale = target_sprite_->getScale();

    // 计算新缩放比例
    float new_scale = current_scale * scale_factor;

    // 限制缩放比例在最小最大值之间
    new_scale = clampf(new_scale, min_scale_, max_scale_);

    // 应用新缩放比例
    target_sprite_->setScale(new_scale);

    // 更新前一次距离
    previous_distance_ = current_distance;

    // 更新边界
    updateBackgroundBounds();
}

void ZoomScrollManager::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    is_scaling_ = false;
}

void ZoomScrollManager::onTouchesCancelled(const std::vector<Touch*>& touches, cocos2d::Event* event)
{
    is_scaling_ = false;
}

void ZoomScrollManager::setScale(float scale)
{
    float new_scale = clampf(scale, min_scale_, max_scale_);
    target_sprite_->setScale(new_scale);
    updateBackgroundBounds();
}