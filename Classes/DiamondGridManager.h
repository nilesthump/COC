#pragma once
#ifndef __DIAMOND_GRID_MANAGER_H__
#define __DIAMOND_GRID_MANAGER_H__

#include "cocos2d.h"
#include "GridDraw.h"

class DiamondGridManager : public cocos2d::Node
{
public:
    // 创建函数
    static DiamondGridManager* create(const cocos2d::Vec2& diamondCenter,
        float diamondWidth,
        float diamondHeight,
        int gridCount = 44);

    // 初始化函数
    bool init(const cocos2d::Vec2& diamondCenter,
        float diamondWidth,
        float diamondHeight,
        int gridCount);

    // 将屏幕坐标转换为菱形坐标
    cocos2d::Vec2 convertScreenToDiamond(const cocos2d::Vec2& screenPos,
        cocos2d::Sprite* backgroundSprite);

    // 检查位置是否在菱形内
    bool isInDiamond(const cocos2d::Vec2& diamondPos);

    // 获取菱形中心点坐标
    cocos2d::Vec2 getDiamondCenter() const { return diamond_center_; }

    // 获取菱形宽度
    float getDiamondWidth() const { return diamond_width_; }

    // 获取菱形高度
    float getDiamondHeight() const { return diamond_height_; }

    // 获取X方向单元格大小
    float getGridCellSizeX() const { return grid_cell_size_x_; }

    // 获取Y方向单元格大小
    float getGridCellSizeY() const { return grid_cell_size_y_; }

    // 获取网格数量
    int getGridCount() const { return grid_count_; }

    // 绘制菱形网格
    void drawDiamondGrid(cocos2d::Sprite* backgroundSprite, float cellSize = 50.0f);

    // 在指定位置绘制菱形（用于测试）
    void drawTestDiamond(cocos2d::Sprite* backgroundSprite,
        const cocos2d::Vec2& center,
        float horizontalSize,
        float verticalSize);

    // 获取鼠标所在网格坐标
    void updateMouseGridPosition(const cocos2d::Vec2& mousePos,
        cocos2d::Sprite* backgroundSprite,
        cocos2d::Label* coordinateLabel);

private:
    // 菱形位置信息
    cocos2d::Vec2 diamond_center_;
    float diamond_width_;
    float diamond_height_;

    // 菱形网格信息
    float grid_cell_size_x_;
    float grid_cell_size_y_;
    int grid_count_;

    // 网格绘制实例
    GridDraw* _gridDraw;
};

#endif // __DIAMOND_GRID_MANAGER_H__