#ifndef _CONVERT_TEST_
#define _CONVERT_TEST_
#include "cocos2d.h"
#include <cmath>

USING_NS_CC;

class ConvertTest
{
private:
    // 数学常量
    static constexpr float SIN_45 = 0.70710678118f;  // sqrt(2)/2
    static constexpr float COS_45 = 0.70710678118f;  // sqrt(2)/2

    // 菱形网格参数（基于实际测量）
    static constexpr float HORIZONTAL_SPACING = 56.0f;   // 菱形水平对角线长度
    static constexpr float VERTICAL_SPACING = 42.0f;     // 菱形垂直对角线长度
    static constexpr float CENTER_OFFSET_X = 21.5f;      // 菱形中心偏移倍数

    // 背景图片中的菱形区域边界（像素坐标，相对于图片左上角）
    static constexpr int LEFT_X = 667;           // 菱形左边界
    static constexpr int RIGHT_X = 3705 - 556;   // 菱形右边界
    static constexpr int TOP_Y = 264;            // 菱形上边界  
    static constexpr int BOTTOM_Y = 2545 - 471;  // 菱形下边界

public:
    /**
     * 将屏幕坐标转换为逻辑坐标
     * @param screenPos 屏幕坐标（原点：屏幕左下角）
     * @param backgroundSprite 背景精灵
     * @return 逻辑坐标（斜45度坐标系，原点：菱形网格左下角）
     */
    static Vec2 convertScreenToLogic(const Vec2& screenPos, Sprite* backgroundSprite)
    {
        if (!backgroundSprite)
        {
            log("[ConvertTest] 错误：backgroundSprite为空");
            return Vec2::ZERO;
        }

        // 1. 计算菱形区域尺寸
        float diamond_width = static_cast<float>(RIGHT_X - LEFT_X);    // 2482
        float diamond_height = static_cast<float>(BOTTOM_Y - TOP_Y);   // 1810

        // 2. 获取背景精灵当前变换
        float scale = backgroundSprite->getScale();
        Vec2 background_pos = backgroundSprite->getPosition();

        // 3. 屏幕坐标 → 背景精灵局部坐标
        Vec2 local_pos = screenPos - background_pos;

        // 4. 局部坐标 → 图像像素坐标（考虑缩放）
        Vec2 image_pos_relative_to_center = Vec2(
            local_pos.x / scale,
            local_pos.y / scale
        );

        // 5. 计算菱形中心在图像坐标系中的位置
        Vec2 diamond_center_absolute = Vec2(
            (LEFT_X + RIGHT_X) / 2.0f,
            (TOP_Y + BOTTOM_Y) / 2.0f
        );
        Vec2 diamond_center = diamond_center_absolute -
            Vec2(backgroundSprite->getContentSize().width / 2,
                backgroundSprite->getContentSize().height / 2);

        // 6. 转换为菱形坐标系（原点：菱形中心）
        Vec2 diamond_pos = image_pos_relative_to_center - diamond_center;

        // 7. 关键转换：从菱形坐标系 → 斜45度逻辑坐标系
        //    平移：+ HORIZONTAL_SPACING * CENTER_OFFSET_X
        //    旋转：顺时针45度
        Vec2 bridge1 = Vec2(
            diamond_pos.x + HORIZONTAL_SPACING * CENTER_OFFSET_X,
            diamond_pos.y
        );

        float x_new = COS_45 * bridge1.x - SIN_45 * bridge1.y;
        float y_new = SIN_45 * bridge1.x + COS_45 * bridge1.y;

        Vec2 logic_pos = Vec2(x_new, y_new);


        return logic_pos;
    }

    /**
     * 将逻辑坐标转换为显示坐标
     * @param logicPos 逻辑坐标（斜45度坐标系）
     * @param backgroundSprite 背景精灵
     * @return 显示坐标（屏幕坐标）
     */
    static Vec2 convertLogicToDisplay(const Vec2& logicPos, Sprite* backgroundSprite)
    {
        if (!backgroundSprite)
        {
            log("[ConvertTest] 错误：backgroundSprite为空");
            return Vec2::ZERO;
        }

        // 1. 计算菱形区域尺寸
        float diamond_width = static_cast<float>(RIGHT_X - LEFT_X);
        float diamond_height = static_cast<float>(BOTTOM_Y - TOP_Y);

        // 2. 计算菱形中心在图像坐标系中的位置
        Vec2 diamond_center_absolute = Vec2(
            (LEFT_X + RIGHT_X) / 2.0f,
            (TOP_Y + BOTTOM_Y) / 2.0f
        );
        Vec2 diamond_center = diamond_center_absolute -
            Vec2(backgroundSprite->getContentSize().width / 2,
                backgroundSprite->getContentSize().height / 2);

        // 3. 关键转换：从斜45度逻辑坐标系 → 菱形坐标系
        //    旋转：逆时针45度
        //    平移：- HORIZONTAL_SPACING * CENTER_OFFSET_X
        float x_rotated = COS_45 * logicPos.x + SIN_45 * logicPos.y;
        float y_rotated = -SIN_45 * logicPos.x + COS_45 * logicPos.y;

        Vec2 bridge1 = Vec2(
            x_rotated - HORIZONTAL_SPACING * CENTER_OFFSET_X,
            y_rotated
        );

        Vec2 diamond_pos = bridge1;

        // 4. 菱形坐标 → 图像像素坐标（相对于图像中心）
        Vec2 image_pos_relative_to_center = diamond_pos + diamond_center;

        // 5. 考虑背景缩放
        float scale = backgroundSprite->getScale();
        Vec2 local_pos = Vec2(
            image_pos_relative_to_center.x * scale,
            image_pos_relative_to_center.y * scale
        );

        // 6. 加上背景位置，得到场景坐标
        Vec2 display_pos = local_pos + backgroundSprite->getPosition();

        return display_pos;
    }

};

#endif // _CONVERT_TEST_