#ifndef __GRID_DRAW_H__
#define __GRID_DRAW_H__

#include "cocos2d.h"

// 网格绘制类
class GridDraw : public cocos2d::Node
{
public:
    // 创建函数
    static GridDraw* create(float cellSize, float width, float height);

    // 初始化函数
    bool init(float cellSize, float width, float height);

    // 绘制隐形网格
    void drawGrid();

    // 在指定位置绘制菱形
    // horizontalSize: 水平对角线长度
    // verticalSize: 垂直对角线长度
    void drawDiamond(const cocos2d::Vec2& center, float horizontalSize, float verticalSize,
        const cocos2d::Color4F& color = cocos2d::Color4F(1.0f, 0.0f, 0.0f, 1.0f));

    // 获取菱形中心点坐标
    cocos2d::Vec2 getDiamondCenter() const;

    // 清除所有绘制
    void clearDraw();

private:
    // 网格参数
    float _cellSize;
    float _width;
    float _height;

    // DrawNode用于绘制
    cocos2d::DrawNode* _drawNode;

    // 菱形中心点
    cocos2d::Vec2 _diamondCenter;
};

#endif // __GRID_DRAW_H__