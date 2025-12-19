#pragma once
#ifndef __BATTLE_TEST_LAYER_H__
#define __BATTLE_TEST_LAYER_H__

#include "cocos2d.h"
#include "BattleManager.h"
#include "ZoomScrollManager.h"
#include "DiamondGridManager.h"
#include <memory>

class BattleTestLayer : public cocos2d::Layer
{
private:
    // 核心系统
    std::unique_ptr<BattleManager> battle_manager_;
    DiamondGridManager* grid_manager_;
    ZoomScrollManager* zoom_manager_;

    // 视觉元素
    cocos2d::Sprite* background_sprite_;
    cocos2d::Node* game_world_;  // 世界容器，包含所有游戏对象

    // 加农炮单位指针（用于后续引用）
    BattleUnit* cannon_unit_;

public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    virtual void update(float delta);

    void menuFirstCallback(cocos2d::Ref* pSender);
    // 初始化函数
    void setupBattleScene();
    void placeCannonAtGridCenter();
    void placeBarbarianAtScreenPos(const cocos2d::Vec2& screenPos);

    // 触摸事件
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);

    // 坐标转换辅助函数
    cocos2d::Vec2 convertScreenToWorld(const cocos2d::Vec2& screenPos);

    CREATE_FUNC(BattleTestLayer);
};

#endif // __BATTLE_TEST_LAYER_H__