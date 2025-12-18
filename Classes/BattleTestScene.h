#ifndef _BATTLE_TEST_SCENE_H_
#define _BATTLE_TEST_SCENE_H_

#include "cocos2d.h"
#include "BattleManager.h"
#include "BattleUnit.h"
#include <memory>
#include <vector>

class BattleManager;
class GridSystem;
class BattleUnit;

class BattleTestScene : public cocos2d::Layer
{
private:
    std::unique_ptr<BattleManager> battleMgr_;

    // 新增：游戏世界容器
    cocos2d::Node* gameWorld_;          // 所有游戏元素的父节点

    // 地图相关
    cocos2d::Sprite* background_sprite_;    // 背景精灵

    // 拖拽相关
    bool is_dragging_;
    cocos2d::Vec2 drag_start_position_;
    cocos2d::Vec2 world_start_position_;    // world_start_position_

    // 缩放相关
    bool is_scaling_;
    float previous_distance_;
    cocos2d::Vec2 scale_center_;
    float current_scale_;    // 新增：当前缩放比例
    float min_scale_;      // 最小缩放比例
    float max_scale_;      // 最大缩放比例

public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    void update(float delta);  // ⭐ 改为float

    //  新增方法
    void zoomAtPosition(const cocos2d::Vec2& screenPos, float scaleFactor);

    // 事件处理方法
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);

    void onMouseScroll(cocos2d::EventMouse* event);
    void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    void onTouchesCancelled(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

    // 地图辅助方法
    void updateBackgroundBounds();

    CREATE_FUNC(BattleTestScene);
};

#endif