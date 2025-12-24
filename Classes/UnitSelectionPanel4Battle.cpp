#include "UnitSelectionPanel4Battle.h"

USING_NS_CC;

// ========================================
// UnitIconButton 实现
// ========================================

UnitIconButton* UnitIconButton::create(
    const std::string& iconPath,
    const std::string& unitName,
    UnitType type,
    std::function<void(UnitType)> callback)
{
    auto button = new (std::nothrow) UnitIconButton();
    if (button && button->init(iconPath, unitName, type, callback))
    {
        button->autorelease();
        return button;
    }
    CC_SAFE_DELETE(button);
    return nullptr;
}

bool UnitIconButton::init(
    const std::string& iconPath,
    const std::string& unitName,
    UnitType type,
    std::function<void(UnitType)> callback)
{
    if (!Node::init())
        return false;

    unit_type_ = type;
    on_click_callback_ = callback;
    is_selected_ = false;

    // 1. 创建图标精灵
    icon_sprite_ = Sprite::create(iconPath);
    if (!icon_sprite_)
    {
        CCLOG("Failed to load unit icon: %s", iconPath.c_str());
        // 如果加载失败，创建一个占位方块
        icon_sprite_ = Sprite::create();
        auto placeholder = DrawNode::create();
        placeholder->drawSolidRect(Vec2(-40, -40), Vec2(40, 40), Color4F::GRAY);
        icon_sprite_->addChild(placeholder);
    }

    // 设置图标大小（统一为80x80像素）
    float iconSize = 100.0f;
    float originalWidth = icon_sprite_->getContentSize().width;
    base_scale_ = iconSize / originalWidth; // 记录基础比例
    icon_sprite_->setScale(base_scale_);

    icon_sprite_->setScale(base_scale_);
    icon_sprite_->setAnchorPoint(Vec2(0.5f, 0.5f));
    icon_sprite_->setPosition(Vec2::ZERO);
    this->addChild(icon_sprite_);

    // 2. 创建选中边框（DrawNode）
    selection_border_ = DrawNode::create();
    float halfSize = iconSize / 2 + 5.0f;

    // 修复：drawPolygon 的正确参数顺序
    // drawPolygon(顶点数组, 顶点数量, 填充颜色, 边框宽度, 边框颜色)
    Vec2 vertices[4] = {
        Vec2(-halfSize, -halfSize),
        Vec2(halfSize, -halfSize),
        Vec2(halfSize, halfSize),
        Vec2(-halfSize, halfSize)
    };

    // 画一个透明填充、黄色边框的矩形
    selection_border_->drawPolygon(
        vertices,                    // 顶点数组
        4,                          // 顶点数量
        Color4F(0, 0, 0, 0),       // 填充颜色（完全透明）
        3.0f,                       // 边框宽度
        Color4F(1, 1, 0, 1)        // 边框颜色（黄色）
    );

    selection_border_->setVisible(false);  // 默认不显示
    this->addChild(selection_border_, 1);

    // 3. 创建单位名称标签
    name_label_ = Label::createWithSystemFont(unitName, "Arial", 16);
    name_label_->setColor(Color3B::WHITE);
    name_label_->setPosition(Vec2(0, -iconSize / 2 - 15));
    this->addChild(name_label_);

    // 4. 设置触摸监听
    setupTouchListener();

    // 设置内容大小（用于布局）
    this->setContentSize(Size(iconSize + 10, iconSize + 35));

    count_label_ = Label::createWithSystemFont("0", "Arial", 18);
    count_label_->setColor(Color3B::RED);
    icon_sprite_->setColor(Color3B::GRAY);
    // 放在图标右下角
    count_label_->setAnchorPoint(Vec2(1, 0));
    count_label_->setPosition(Vec2(iconSize / 2, -iconSize / 2));
    this->addChild(count_label_, 2);
    return true;
}

//每个兵种数量，要更新显示
void UnitIconButton::setCount(int count)
{
    count_label_->setString(std::to_string(count));
    if (count == 0)
    {
        count_label_->setColor(Color3B::RED);
        icon_sprite_->setColor(Color3B::GRAY); // 没兵了图标变灰
    }
    else
    {
        count_label_->setColor(Color3B::GREEN);
        icon_sprite_->setColor(Color3B::WHITE);
    }
}

void UnitIconButton::setupTouchListener()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    // 按下时
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
        {
            // 将触摸点转换到按钮的本地坐标
            Vec2 locationInNode = this->convertToNodeSpace(touch->getLocation());
            // 判定区域稍微设大一点，方便手指点击
            Rect touchRect = Rect(-60, -60, 120, 120);

            if (touchRect.containsPoint(locationInNode))
            {
                // 停止当前所有动画，立刻缩小反馈
                icon_sprite_->stopAllActions();
                icon_sprite_->setScale(base_scale_ * 0.9f);
                return true;
            }
            return false;
        };

    // 松开时
    listener->onTouchEnded = [this](Touch* touch, Event* event)
        {
            if (on_click_callback_)
            {
                on_click_callback_(unit_type_);
            }
            updateVisuals(); // 自动根据 is_selected_ 恢复到 base_scale_ 或 base_scale_ * 1.1
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void UnitIconButton::setSelected(bool selected)
{
    if (is_selected_ == selected)
        return;  // 状态没变，不需要更新

    is_selected_ = selected;
    updateVisuals();
}

void UnitIconButton::updateVisuals()
{
    icon_sprite_->stopAllActions(); // 切换状态前停止之前的动画，防止冲突
    if (is_selected_)
    {
        selection_border_->setVisible(true);
        // 选中的目标是 1.1 倍
        auto scaleUp = ScaleTo::create(0.1f, base_scale_ * 1.1f);
        icon_sprite_->runAction(EaseBackOut::create(scaleUp));
    }
    else
    {
        selection_border_->setVisible(false);
        // 未选中的目标必须是精确的 base_scale_
        auto scaleDown = ScaleTo::create(0.1f, base_scale_);
        icon_sprite_->runAction(EaseQuadraticActionOut::create(scaleDown));
    }
}

// UnitSelectionPanel 实现
UnitSelectionPanel* UnitSelectionPanel::create(std::function<void(UnitType)> callback)
{
    auto panel = new (std::nothrow) UnitSelectionPanel();
    if (panel && panel->init(callback))
    {
        panel->autorelease();
        return panel;
    }
    CC_SAFE_DELETE(panel);
    return nullptr;
}

bool UnitSelectionPanel::init(std::function<void(UnitType)> callback)
{
    if (!Node::init())
        return false;

    on_selection_changed_ = callback;
    currently_selected_ = nullptr;

    // 创建半透明黑色背景
    auto visibleSize = Director::getInstance()->getVisibleSize();
    float panelHeight = 120.0f;

    panel_background_ = DrawNode::create();
    panel_background_->drawSolidRect(
        Vec2(0, 0),
        Vec2(visibleSize.width, panelHeight),
        Color4F(0, 0, 0, 0.7f)  // 黑色，70%不透明度
    );
    this->addChild(panel_background_);

    CCLOG("UnitSelectionPanel created");

    return true;
}

//告诉特定面板更新数字
void UnitSelectionPanel::updateUnitCount(UnitType type, int count)
{
    for (auto btn : unit_buttons_)
    {
        if (btn->getUnitType() == type)
        {
            btn->setCount(count);
            break;
        }
    }
}

void UnitSelectionPanel::addUnitIcon( const std::string& iconPath,const std::string& unitName,UnitType type)
{
    // 创建按钮，传入点击回调
    auto button = UnitIconButton::create(iconPath, unitName, type,
        CC_CALLBACK_1(UnitSelectionPanel::onUnitButtonClicked, this));

    if (button)
    {
        this->addChild(button, 1);
        unit_buttons_.push_back(button);

        // 关键修复：添加图标后，根据 CombatSessionManager 的初始数据立即更新一次 UI 状态
        auto config = CombatSessionManager::getInstance();
        int initialCount = 0;
        if (config->battle_start_params.attackerInventory.count(type))
        {
            initialCount = config->battle_start_params.attackerInventory[type];
        }
        button->setCount(initialCount); // 这里会自动触发红色变灰逻辑

        updateButtonPositions();
    }
}

void UnitSelectionPanel::onUnitButtonClicked(UnitType type)
{
    CCLOG("Unit button clicked: %d", (int)type);

    // 1. 如果点击的是当前已经选中的按钮
    if (currently_selected_ && currently_selected_->getUnitType() == type)
    {
        CCLOG("De-selecting current unit");
        currently_selected_->setSelected(false);
        currently_selected_ = nullptr;

        // 通知外部现在什么都没选（你可以定义一个 UnitType::NONE）
        if (on_selection_changed_)
        {
            on_selection_changed_(UnitType::NONE);
        }
        return; // 直接返回，不再执行选中逻辑
    }

    // 2. 取消之前选中的旧按钮
    if (currently_selected_)
    {
        currently_selected_->setSelected(false);
    }

    // 3. 查找并选中新按钮
    for (auto button : unit_buttons_)
    {
        if (button->getUnitType() == type)
        {
            button->setSelected(true);
            currently_selected_ = button;
            break;
        }
    }

    if (on_selection_changed_)
    {
        on_selection_changed_(type);
    }
}

void UnitSelectionPanel::updateButtonPositions()
{
    if (unit_buttons_.empty())
        return;

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 按钮之间的间距
    float spacing = 40.0f;
    float buttonWidth = 110.0f;  // 每个按钮占用的宽度

    // 计算总宽度
    float totalWidth = unit_buttons_.size() * buttonWidth +
        (unit_buttons_.size() - 1) * spacing;

    // 居中排列的起始X坐标
    float startX = (visibleSize.width - totalWidth) / 2;
    float yPos = 60.0f;  // 距离面板底部的高度

    // 逐个设置按钮位置
    for (size_t i = 0; i < unit_buttons_.size(); ++i)
    {
        float xPos = startX + i * (buttonWidth + spacing) + buttonWidth / 2;
        unit_buttons_[i]->setPosition(Vec2(xPos, yPos));
    }

    CCLOG("Updated %lu button positions", unit_buttons_.size());
}

UnitType UnitSelectionPanel::getCurrentSelection() const
{
    if (currently_selected_)
    {
        return currently_selected_->getUnitType();
    }
    // 默认返回野蛮人
    return UnitType::BARBARIAN;
}