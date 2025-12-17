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

    is_dragging_ = false;
    drag_start_position_ = Vec2::ZERO;
    background_start_position_ = Vec2::ZERO;


    is_scaling_ = false;
    previous_distance_ = 0.0f;
    scale_center_ = Vec2::ZERO;
    min_scale_ = 0.5f;
    max_scale_ = 2.0f;


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

    // Initialize diamond position information based on precise measurements
    int left_x = 667;          // Left vertex x-coordinate from left boundary
    int right_x = 3705 - 556;  // Right vertex x-coordinate (image width - distance to right boundary)
    int top_y = 264;           // Top vertex y-coordinate from top boundary
    int bottom_y = 2545 - 471; // Bottom vertex y-coordinate (image height - distance to bottom boundary)

    diamond_width_ = right_x - left_x;       // Diamond width: 3149 - 667 = 2482
    diamond_height_ = bottom_y - top_y;      // Diamond height: 2074 - 264 = 1810

    // Calculate diamond center relative to background sprite center
    Vec2 diamond_center_absolute = Vec2((left_x + right_x) / 2.0f, (top_y + bottom_y) / 2.0f);
    diamond_center_ = diamond_center_absolute - Vec2(background_sprite_->getContentSize().width / 2, background_sprite_->getContentSize().height / 2);

    // Initialize diamond grid information
    grid_count_ = 44;
    // Each small square is approximately 30 pixels, recalculated based on actual diamond dimensions
    grid_cell_size_x_ = diamond_width_ / grid_count_;  // 2482 / 44 ≈ 56.409
    grid_cell_size_y_ = diamond_height_ / grid_count_; // 1810 / 44 ≈ 41.136

    // Create coordinate display label
    coordinate_label_ = Label::createWithTTF("坐标: ", "fonts/STZhongSong_Bold.ttf", 20);
    coordinate_label_->setColor(Color3B::YELLOW);
    coordinate_label_->setPosition(Vec2(origin.x + visibleSize.width - 200, origin.y + 30));
    this->addChild(coordinate_label_, 2);


    mouse_pos_ = Vec2::ZERO;


    auto multi_touch_listener = EventListenerTouchAllAtOnce::create();
    multi_touch_listener->onTouchesBegan = CC_CALLBACK_2(SecondScene::onTouchesBegan, this);
    multi_touch_listener->onTouchesMoved = CC_CALLBACK_2(SecondScene::onTouchesMoved, this);
    multi_touch_listener->onTouchesEnded = CC_CALLBACK_2(SecondScene::onTouchesEnded, this);
    multi_touch_listener->onTouchesCancelled = CC_CALLBACK_2(SecondScene::onTouchesCancelled, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(multi_touch_listener, this);
    //以下为网格绘制
    _cellSize = 50.0f;

    float scaledWidth = background_sprite_->getContentSize().width * background_sprite_->getScale();
    float scaledHeight = background_sprite_->getContentSize().height * background_sprite_->getScale();
    _gridDraw = GridDraw::create(_cellSize, scaledWidth, scaledHeight);
    _gridDraw->setPosition(Vec2::ZERO);
    background_sprite_->addChild(_gridDraw, 1);

    // 在背景精灵中心绘制一个菱形作为示例
    Vec2 diamondCenter = Vec2(scaledWidth / 2 - 24.0f, scaledHeight / 2 - 1.5f);
    float horizontalSize = 56.0f;  // 水平对角线长度
    float verticalSize = 42.0f;    // 垂直对角线长度
    _gridDraw->drawDiamond(diamondCenter, horizontalSize, verticalSize);
    //标注角
    auto grid_draw = GridDraw::create(_cellSize, scaledWidth, scaledHeight);
    grid_draw->setPosition(Vec2::ZERO);
    background_sprite_->addChild(grid_draw, 1);
    Vec2 diamond_center = Vec2(diamondCenter.x - 16 * horizontalSize, diamondCenter.y + 9 * verticalSize);//最左侧的角
    grid_draw->drawDiamond(diamond_center, horizontalSize, verticalSize);

    auto grid_draw2 = GridDraw::create(_cellSize, scaledWidth, scaledHeight);
    grid_draw2->setPosition(Vec2::ZERO);
    background_sprite_->addChild(grid_draw2, 1);
    Vec2 diamond_center2 = Vec2(diamond_center.x + 49 * horizontalSize, diamond_center.y);//最右侧的角
    grid_draw2->drawDiamond(diamond_center2, horizontalSize, verticalSize);
    for (int i = 0; i < 50; i += 2)
        for (int j = 0; j < 50; j += 2)
        {
            Vec2 dia = Vec2(diamond_center.x + (j + i) * horizontalSize / 2, diamond_center.y + (j - i) * verticalSize / 2);
            grid_draw->drawDiamond(dia, horizontalSize, verticalSize);
        }

    for (int i = 0; i < 50; i += 2)
        for (int j = 0; j < 50; j += 2)
        {
            Vec2 dia = Vec2(diamond_center2.x - (j + i) * horizontalSize / 2, diamond_center2.y - (j - i) * verticalSize / 2);
            grid_draw2->drawDiamond(dia, horizontalSize, verticalSize);
        }

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
   
    drag_start_position_ = touch->getLocation();
    background_start_position_ = background_sprite_->getPosition();
    is_dragging_ = true;
    return true;
}

void SecondScene::onTouchMoved(Touch* touch, Event* event)
{
    if (!is_dragging_)
        return;

    auto current_touch_position = touch->getLocation();
    auto offset = current_touch_position - drag_start_position_;

  
    auto new_position = background_start_position_ + offset;

    auto visible_size = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    float background_width = background_sprite_->getContentSize().width * background_sprite_->getScale();
    float background_height = background_sprite_->getContentSize().height * background_sprite_->getScale();


    float minX = origin.x + visible_size.width / 2;
    float maxX = origin.x + visible_size.width / 2;
    float minY = origin.y + visible_size.height / 2;
    float maxY = origin.y + visible_size.height / 2;

    if (background_width > visible_size.width)
    {
        minX -= (background_width - visible_size.width) / 2;
        maxX += (background_width - visible_size.width) / 2;
    }

    if (background_height > visible_size.width)
    {
        minY -= (background_height - visible_size.height) / 2;
        maxY += (background_height - visible_size.height) / 2;
    }


    new_position.x = clampf(new_position.x, minX, maxX);
    new_position.y = clampf(new_position.y, minY, maxY);


    background_sprite_->setPosition(new_position);
}

void SecondScene::onTouchEnded(Touch* touch, Event* event)
{

    is_dragging_ = false;
}

void SecondScene::onTouchCancelled(Touch* touch, Event* event)
{

    is_dragging_ = false;
}


void SecondScene::onMouseScroll(EventMouse* event)
{
  
    float scrollY = event->getScrollY();


    float scale_factor = scrollY > 0 ? 1.1f : 0.9f;


    float current_scale = background_sprite_->getScale();


    float new_scale = current_scale * scale_factor;


    new_scale = clampf(new_scale, min_scale_, max_scale_);


    background_sprite_->setScale(new_scale);


    updateBackgroundBounds();
}


void SecondScene::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
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


void SecondScene::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
    if (!is_scaling_ || touches.size() != 2)
        return;


    Vec2 touch1 = touches[0]->getLocation();
    Vec2 touch2 = touches[1]->getLocation();
    float current_distance = touch1.distance(touch2);

    float scale_factor = current_distance / previous_distance_;


    float current_scale = background_sprite_->getScale();


    float new_scale = current_scale * scale_factor;


    new_scale = clampf(new_scale, min_scale_, max_scale_);


    background_sprite_->setScale(new_scale);


    previous_distance_ = current_distance;


    updateBackgroundBounds();
}


void SecondScene::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{

    is_scaling_ = false;
}


void SecondScene::onTouchesCancelled(const std::vector<Touch*>& touches, Event* event)
{

    is_scaling_ = false;
}


void SecondScene::updateBackgroundBounds()
{

    float scale = background_sprite_->getScale();


    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size visible_size = Director::getInstance()->getVisibleSize();


    Size background_size = background_sprite_->getContentSize();


    float scaled_width = background_size.width * scale;
    float scaled_height = background_size.height * scale;


    Vec2 current_position = background_sprite_->getPosition();

    float minX = origin.x + visible_size.width / 2;
    float maxX = origin.x + visible_size.width / 2;
    float minY = origin.y + visible_size.height / 2;
    float maxY = origin.y + visible_size.height / 2;


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


    Vec2 new_position;
    new_position.x = clampf(current_position.x, minX, maxX);
    new_position.y = clampf(current_position.y, minY, maxY);


    background_sprite_->setPosition(new_position);
}


void SecondScene::onMouseMove(EventMouse* event)
{
    mouse_pos_ = event->getLocation();
    
    // Convert screen coordinates to diamond coordinates
    Vec2 diamond_pos = convertScreenToDiamond(mouse_pos_);
    
    // Check if position is inside diamond
    if (isInDiamond(diamond_pos))
    {
        // Convert diamond coordinates to grid coordinates with proper rounding
        int grid_x = static_cast<int>(floor(diamond_pos.x / grid_cell_size_x_ + 0.5f));
        int grid_y = static_cast<int>(floor(diamond_pos.y / grid_cell_size_y_ + 0.5f));
        
        // Display grid coordinates
        coordinate_label_->setString(StringUtils::format("坐标: (%d, %d)", grid_x, grid_y));
    }
    else
    {
        // Display out of bounds message
        coordinate_label_->setString("坐标: 界外");
    }
}

// Convert screen coordinates to diamond coordinates
Vec2 SecondScene::convertScreenToDiamond(const Vec2& screenPos)
{
    // Get scaling factor
    float scale = background_sprite_->getScale();
    
    // Get background sprite actual position
    Vec2 background_pos = background_sprite_->getPosition();
    
    // Convert screen coordinates to background sprite local coordinates
    Vec2 local_pos = screenPos - background_pos;
    
    // Convert to image pixel coordinates (relative to background sprite center)
    Vec2 image_pos_relative_to_center = Vec2(
        local_pos.x / scale,
        local_pos.y / scale
    );
    
    // Convert to diamond coordinates (diamond center as origin)
    Vec2 diamond_pos = image_pos_relative_to_center - diamond_center_;
    
    // Debug log to trace coordinate transformations
    log("ScreenPos: (%.2f, %.2f), BackgroundPos: (%.2f, %.2f), LocalPos: (%.2f, %.2f)", 
        screenPos.x, screenPos.y, background_pos.x, background_pos.y, local_pos.x, local_pos.y);
    log("ImagePosRelativeToCenter: (%.2f, %.2f), DiamondCenter: (%.2f, %.2f), DiamondPos: (%.2f, %.2f)", 
        image_pos_relative_to_center.x, image_pos_relative_to_center.y, diamond_center_.x, diamond_center_.y, diamond_pos.x, diamond_pos.y);
    
    return diamond_pos;
}

// Check if position is inside diamond
bool SecondScene::isInDiamond(const Vec2& diamondPos)
{
    // Convert diamond coordinates to grid coordinates with proper rounding
    int grid_x = static_cast<int>(floor(diamondPos.x / grid_cell_size_x_ + 0.5f));
    int grid_y = static_cast<int>(floor(diamondPos.y / grid_cell_size_y_ + 0.5f));
    
    // Check if absolute sum of grid coordinates is within 22
    return (fabs(grid_x) + fabs(grid_y) <= 22);
}

// GridDraw类实现
GridDraw* GridDraw::create(float cellSize, float width, float height)
{
    GridDraw* grid = new (std::nothrow) GridDraw();
    if (grid && grid->init(cellSize, width, height))
    {
        grid->autorelease();
        return grid;
    }
    CC_SAFE_DELETE(grid);
    return nullptr;
}

bool GridDraw::init(float cellSize, float width, float height)
{
    if (!Node::init())
    {
        return false;
    }

    // 初始化参数
    _cellSize = cellSize;
    _width = width;
    _height = height;
    _diamondCenter = Vec2::ZERO;

    // 创建DrawNode
    _drawNode = DrawNode::create();
    this->addChild(_drawNode);

    // 绘制隐形网格
    drawGrid();

    return true;
}

void GridDraw::drawGrid()
{
    // 清空之前的绘制
    _drawNode->clear();

    // 计算网格线数量
    int rows = static_cast<int>(_height / _cellSize) + 1;
    int cols = static_cast<int>(_width / _cellSize) + 1;

    // 绘制垂直线（隐形）
    for (int i = 0; i < cols; ++i)
    {
        float x = i * _cellSize;
        Vec2 start(x, 0);
        Vec2 end(x, _height);
        // 使用完全透明的颜色，实现隐形网格
        _drawNode->drawSegment(start, end, 1.0f, Color4F(0.0f, 0.0f, 0.0f, 0.0f));
    }

    // 绘制水平线（隐形）
    for (int i = 0; i < rows; ++i)
    {
        float y = i * _cellSize;
        Vec2 start(0, y);
        Vec2 end(_width, y);
        // 使用完全透明的颜色，实现隐形网格
        _drawNode->drawSegment(start, end, 1.0f, Color4F(0.0f, 0.0f, 0.0f, 0.0f));
    }
}

void GridDraw::drawDiamond(const Vec2& center, float horizontalSize, float verticalSize, const Color4F& color)
{
    // 记录菱形中心点
    _diamondCenter = center;

    // 计算菱形的四个顶点
    // horizontalSize: 水平对角线长度
    // verticalSize: 垂直对角线长度
    Vec2 top(center.x, center.y + verticalSize / 2);     // 上顶点（垂直方向）
    Vec2 right(center.x + horizontalSize / 2, center.y);  // 右顶点（水平方向）
    Vec2 bottom(center.x, center.y - verticalSize / 2);   // 下顶点（垂直方向）
    Vec2 left(center.x - horizontalSize / 2, center.y);   // 左顶点（水平方向）

    // 创建顶点数组
    std::vector<Vec2> vertices = { top, right, bottom, left };

    // 绘制菱形
    _drawNode->drawPolygon(&vertices[0], vertices.size(), color, 1.0f, color);
}

Vec2 GridDraw::getDiamondCenter() const
{
    return _diamondCenter;
}

// SecondScene网格相关方法实现
Vec2 SecondScene::getDiamondCenter() const
{
    if (_gridDraw)
    {
        return _gridDraw->getDiamondCenter();
    }
    return Vec2::ZERO;
}

Vec2 SecondScene::getCellPosition(int gridX, int gridY) const
{
    Vec2 diamondCenter = getDiamondCenter();

    // 根据菱形中心点计算指定网格坐标的单元格位置
    float x = diamondCenter.x + gridX * _cellSize;
    float y = diamondCenter.y + gridY * _cellSize;

    return Vec2(x, y);
}

#endif