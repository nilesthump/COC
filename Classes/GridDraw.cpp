#include "GridDraw.h"

USING_NS_CC;

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

void GridDraw::clearDraw()
{
    if (_drawNode)
    {
        _drawNode->clear();
    }
}