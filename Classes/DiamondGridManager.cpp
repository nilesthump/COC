#include "DiamondGridManager.h"
#include <cmath>

USING_NS_CC;

DiamondGridManager* DiamondGridManager::create(const Vec2& diamondCenter,
    float diamondWidth,
    float diamondHeight,
    int gridCount)
{
    DiamondGridManager* manager = new (std::nothrow) DiamondGridManager();
    if (manager && manager->init(diamondCenter, diamondWidth, diamondHeight, gridCount))
    {
        manager->autorelease();
        return manager;
    }
    CC_SAFE_DELETE(manager);
    return nullptr;
}

bool DiamondGridManager::init(const Vec2& diamondCenter,
    float diamondWidth,
    float diamondHeight,
    int gridCount)
{
    if (!Node::init())
    {
        return false;
    }

    diamond_center_ = diamondCenter;
    diamond_width_ = diamondWidth;
    diamond_height_ = diamondHeight;
    grid_count_ = gridCount;

    // 计算网格单元格大小
    grid_cell_size_x_ = diamond_width_ / grid_count_;
    grid_cell_size_y_ = diamond_height_ / grid_count_;

    _gridDraw = nullptr;

    return true;
}

Vec2 DiamondGridManager::convertScreenToDiamond(const Vec2& screenPos, Sprite* backgroundSprite)
{
    if (!backgroundSprite)
        return Vec2::ZERO;

    // 获取缩放因子
    float scale = backgroundSprite->getScale();

    // 获取背景精灵实际位置
    Vec2 background_pos = backgroundSprite->getPosition();

    // 将屏幕坐标转换为背景精灵本地坐标
    Vec2 local_pos = screenPos - background_pos;

    // 转换为图像像素坐标（相对于背景精灵中心）
    Vec2 image_pos_relative_to_center = Vec2(
        local_pos.x / scale,
        local_pos.y / scale
    );

    // 转换为菱形坐标（菱形中心作为原点）
    Vec2 diamond_pos = image_pos_relative_to_center - diamond_center_;

    // 调试日志，追踪坐标转换过程
    log("屏幕坐标: (%.2f, %.2f), 背景位置: (%.2f, %.2f), 本地位置: (%.2f, %.2f)",
        screenPos.x, screenPos.y, background_pos.x, background_pos.y, local_pos.x, local_pos.y);
    log("相对于中心的图像位置: (%.2f, %.2f), 菱形中心: (%.2f, %.2f), 菱形坐标: (%.2f, %.2f)",
        image_pos_relative_to_center.x, image_pos_relative_to_center.y,
        diamond_center_.x, diamond_center_.y, diamond_pos.x, diamond_pos.y);

    return diamond_pos;
}

bool DiamondGridManager::isInDiamond(const Vec2& diamondPos)
{
    // 将菱形坐标转换为网格坐标，使用适当的四舍五入
    int grid_x = static_cast<int>(floor(diamondPos.x / grid_cell_size_x_ + 0.5f));
    int grid_y = static_cast<int>(floor(diamondPos.y / grid_cell_size_y_ + 0.5f));

    // 检查网格坐标的绝对值和是否在22以内
    return (fabs(grid_x) + fabs(grid_y) <= 22);
}

void DiamondGridManager::drawDiamondGrid(Sprite* backgroundSprite, float cellSize)
{
    if (!backgroundSprite)
        return;

    float scaledWidth = backgroundSprite->getContentSize().width * backgroundSprite->getScale();
    float scaledHeight = backgroundSprite->getContentSize().height * backgroundSprite->getScale();

    if (_gridDraw)
    {
        _gridDraw->removeFromParent();
        _gridDraw = nullptr;
    }

    _gridDraw = GridDraw::create(cellSize, scaledWidth, scaledHeight);
    _gridDraw->setPosition(Vec2::ZERO);
    backgroundSprite->addChild(_gridDraw, 1);

    // 在背景精灵中心绘制一个菱形作为示例
    // 这里使用diamond_center_来指代成员变量，与局部变量区分
    Vec2 diamondCenterPos = Vec2(scaledWidth / 2 - 24.0f, scaledHeight / 2 - 1.5f);
    float horizontalSize = 56.0f;  // 水平对角线长度
    float verticalSize = 42.0f;    // 垂直对角线长度
    _gridDraw->drawDiamond(diamondCenterPos, horizontalSize, verticalSize);

    // 标注角
    auto grid_draw = GridDraw::create(cellSize, scaledWidth, scaledHeight);
    grid_draw->setPosition(Vec2::ZERO);
    backgroundSprite->addChild(grid_draw, 1);

    // 计算左上角位置
    Vec2 leftCorner = Vec2(diamondCenterPos.x - 16 * horizontalSize,
        diamondCenterPos.y + 9 * verticalSize);
    grid_draw->drawDiamond(leftCorner, horizontalSize, verticalSize);

    auto grid_draw2 = GridDraw::create(cellSize, scaledWidth, scaledHeight);
    grid_draw2->setPosition(Vec2::ZERO);
    backgroundSprite->addChild(grid_draw2, 1);

    // 计算右上角位置
    Vec2 rightCorner = Vec2(leftCorner.x + 49 * horizontalSize, leftCorner.y);
    grid_draw2->drawDiamond(rightCorner, horizontalSize, verticalSize);

    // 绘制菱形网格
    for (int i = 0; i < 50; i += 2)
    {
        for (int j = 0; j < 50; j += 2)
        {
            Vec2 dia = Vec2(leftCorner.x + (j + i) * horizontalSize / 2,
                leftCorner.y + (j - i) * verticalSize / 2);
            grid_draw->drawDiamond(dia, horizontalSize, verticalSize);
        }
    }

    for (int i = 0; i < 50; i += 2)
    {
        for (int j = 0; j < 50; j += 2)
        {
            Vec2 dia = Vec2(rightCorner.x - (j + i) * horizontalSize / 2,
                rightCorner.y - (j - i) * verticalSize / 2);
            grid_draw2->drawDiamond(dia, horizontalSize, verticalSize);
        }
    }
}

void DiamondGridManager::drawTestDiamond(Sprite* backgroundSprite,
    const Vec2& center,
    float horizontalSize,
    float verticalSize)
{
    if (!backgroundSprite)
        return;

    float scaledWidth = backgroundSprite->getContentSize().width * backgroundSprite->getScale();
    float scaledHeight = backgroundSprite->getContentSize().height * backgroundSprite->getScale();

    auto testGridDraw = GridDraw::create(50.0f, scaledWidth, scaledHeight);
    testGridDraw->setPosition(Vec2::ZERO);
    backgroundSprite->addChild(testGridDraw, 1);
    testGridDraw->drawDiamond(center, horizontalSize, verticalSize);
}

void DiamondGridManager::updateMouseGridPosition(const Vec2& mousePos,
    Sprite* backgroundSprite,
    Label* coordinateLabel)
{
    if (!backgroundSprite || !coordinateLabel)
        return;

    // 将屏幕坐标转换为菱形坐标
    Vec2 diamond_pos = convertScreenToDiamond(mousePos, backgroundSprite);

    // 检查位置是否在菱形内
    if (isInDiamond(diamond_pos))
    {
        // 将菱形坐标转换为网格坐标，使用适当的四舍五入
        int grid_x = static_cast<int>(floor(diamond_pos.x / grid_cell_size_x_ + 0.5f));
        int grid_y = static_cast<int>(floor(diamond_pos.y / grid_cell_size_y_ + 0.5f));

        // 显示网格坐标
        coordinateLabel->setString(StringUtils::format("坐标: (%d, %d)", grid_x, grid_y));
    }
    else
    {
        // 显示越界信息
        coordinateLabel->setString("坐标: 界外");
    }
}