#ifndef _CONVERT_TEST_
#define _CONVERT_TEST_
#include "cocos2d.h"
#include <cmath>
//12/22 注意！，修改为0-49坐标转换。角色可放置位置为(-0.5---49.5)，
//建筑边界应在(1.5--47.5)或者建筑有自己一套逻辑判断是出界，你们决定哈
USING_NS_CC;

class ConvertTest
{
private:
    // 菱形网格参数
    static constexpr float HORIZONTAL_SPACING = 56.0f;   // 水平间距
    static constexpr float VERTICAL_SPACING = 42.0f;     // 垂直间距

    // 背景图片中的菱形区域边界（像素坐标）
    static constexpr int LEFT_X = 667;
    static constexpr int RIGHT_X = 3705 - 556;  // 3149
    static constexpr int TOP_Y = 264;
    static constexpr int BOTTOM_Y = 2545 - 471; // 2074

    static constexpr int GRID_COUNT = 50;

public:
    // ============================================
    // 核心转换：网格坐标 → game_world本地坐标
    // ============================================
    /**
     * 参考DiamondGridManager::drawDiamondGrid的逻辑
     * Vec2 dia = Vec2(leftCorner.x + (j + i) * horizontalSize / 2,
     *                 leftCorner.y + (j - i) * verticalSize / 2);
     *
     * @param gridX 网格X坐标 (0-49，从左到右)
     * @param gridY 网格Y坐标 (0-49，从下到上)
     * @param backgroundSprite 背景精灵
     * @return game_world_本地坐标
     */
    static Vec2 convertGridToLocal(float gridX, float gridY, Sprite* backgroundSprite)
    {
        if (!backgroundSprite)
            return Vec2::ZERO;

        // 1. 计算菱形区域的中心（相对于背景图片中心）
        float diamond_center_x = (LEFT_X + RIGHT_X) / 2.0f;  // 1908
        float diamond_center_y = (TOP_Y + BOTTOM_Y) / 2.0f;  // 1169

        Vec2 diamond_center_absolute = Vec2(diamond_center_x, diamond_center_y);

        // 转换为相对于背景精灵中心的坐标
        Vec2 diamond_center = diamond_center_absolute - Vec2(
            backgroundSprite->getContentSize().width / 2.0f,
            backgroundSprite->getContentSize().height / 2.0f
        );

        // 2.计算左角位置 - 向左移动3个横向对角线长度
        // 原始44x44的左角偏移：-21.75 * HORIZONTAL_SPACING
        // 50x50需要额外偏移：从44到50增加了6格，每边增加3格
        // 向左移动3格 = -3 * HORIZONTAL_SPACING
        // 所以新的左角X偏移 = -21.75 - 3 = -24.75

        // 对于Y轴，50x50比44x44高6格，所以上下各增加3格
        // 原始Y偏移是4.68，现在需要上移3格 = +3 * VERTICAL_SPACING / 2
        // 新的Y偏移 = 4.68 + 3 = 7.68

        // 根据DiamondGridManager的偏移量计算左角
        //! 整体网格调整在这里，如果整体偏右把-24.75调小，偏左调大；如果整体偏上把7.68调小，偏下调大
        Vec2 left_corner = diamond_center + Vec2(
            -24.75 * HORIZONTAL_SPACING, 
            4.68 * VERTICAL_SPACING       
        );

        // 3. 使用DiamondGridManager的公式计算位置
        // Vec2 dia = Vec2(leftCorner.x + (j + i) * horizontalSize / 2,
        //                 leftCorner.y + (j - i) * verticalSize / 2);
        // 这里gridX对应i，gridY对应j
        Vec2 local_pos = Vec2(
            left_corner.x + (gridY + gridX) * HORIZONTAL_SPACING / 2.0f,
            left_corner.y + (gridY - gridX) * VERTICAL_SPACING / 2.0f
        );

        return local_pos;
    }

    //FHT添加坐标转化??
    static Vec2 myConvertLocalToGrid(const Vec2& localPos)
    {
        float diamond_center_x = (LEFT_X + RIGHT_X) / 2.0f;
        float diamond_center_y = (TOP_Y + BOTTOM_Y) / 2.0f;        
        Vec2 t1 = Vec2(localPos.x - diamond_center_x, localPos.y - diamond_center_y);//t1是相对于菱形中心的像素点坐标
        Vec2 t2 = Vec2(t1.x + HORIZONTAL_SPACING * 24.5, t1.y);//t2是对于左侧红点中心的像素点坐标

        float L = std::sqrt(t2.x * t2.x + t2.y * t2.y);//长度
        float sin = t2.y / L;
        float cos = t2.x / L;

        float x1 = L * (0.6 * cos - 0.8 * sin);
        float y1 = L * (0.8 * cos + 0.6 * sin);

        Vec2 result = Vec2((x1 + 28.0) / 56.0, (y1 + 21.0) / 28.0);
        return result;
    }

    // ============================================
    // 核心转换：game_world本地坐标 → 网格坐标
    // ============================================
    /**
     * 逆向转换
     * 已知：
     * x = leftCorner.x + (gridY + gridX) * (H/2)
     * y = leftCorner.y + (gridY - gridX) * (V/2)
     *
     * 求解：
     * gridY = [(x - leftCorner.x) / (H/2) + (y - leftCorner.y) / (V/2)] / 2
     * gridX = [(x - leftCorner.x) / (H/2) - (y - leftCorner.y) / (V/2)] / 2
     */
    static Vec2 convertLocalToGrid(const Vec2& localPos, Sprite* backgroundSprite)
    {
        if (!backgroundSprite)
            return Vec2::ZERO;

        // 1. 计算菱形中心和左角
        float diamond_center_x = (LEFT_X + RIGHT_X) / 2.0f;
        float diamond_center_y = (TOP_Y + BOTTOM_Y) / 2.0f;

        Vec2 diamond_center_absolute = Vec2(diamond_center_x, diamond_center_y);
        Vec2 diamond_center = diamond_center_absolute - Vec2(
            backgroundSprite->getContentSize().width / 2.0f,
            backgroundSprite->getContentSize().height / 2.0f
        );

        // 使用与convertGridToLocal相同的左角计算
        Vec2 left_corner = diamond_center + Vec2(
            -24.75 * HORIZONTAL_SPACING,
            4.68 * VERTICAL_SPACING
        );

        // 2. 计算相对于左角的偏移
        Vec2 relative = localPos - left_corner;

        // 3. 逆向计算网格坐标
        float norm_x = relative.x / (HORIZONTAL_SPACING / 2.0f);
        float norm_y = relative.y / (VERTICAL_SPACING / 2.0f);

        float gridY = (norm_x + norm_y) / 2.0f;
        float gridX = (norm_x - norm_y) / 2.0f;

        return Vec2(gridX, gridY);
    }

    // ============================================
    // 辅助转换：屏幕坐标 → 网格坐标
    // ============================================
    static Vec2 convertScreenToGrid(
        const Vec2& screenPos,
        Sprite* backgroundSprite,
        Node* gameWorld)
    {
        if (!backgroundSprite || !gameWorld)
            return Vec2::ZERO;

        Vec2 localPos = gameWorld->convertToNodeSpace(screenPos);
        return convertLocalToGrid(localPos, backgroundSprite);
    }

    // ============================================
    // 辅助转换：网格坐标 → 屏幕坐标
    // ============================================
    static Vec2 convertGridToScreen(
        int gridX,
        int gridY,
        Sprite* backgroundSprite,
        Node* gameWorld)
    {
        if (!backgroundSprite || !gameWorld)
            return Vec2::ZERO;

        Vec2 localPos = convertGridToLocal(gridX, gridY, backgroundSprite);
        return gameWorld->convertToWorldSpace(localPos);
    }
};

#endif // _CONVERT_TEST_