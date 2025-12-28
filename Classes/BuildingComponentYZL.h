//把建筑的世界规则从 BattleUnit 和 Sprite 中解耦出来
//它只做三件事：
//占多少格（tile_w / tile_h）
//图片如何对齐占地（自动 scale + 居中）
//(1) IndexSystem (逻辑层)：只管占坑
//它通过 round 把逻辑中心变成索引区间。
//(2) BuildingComponent(表现层)：只管显示
//它不需要再关心奇偶，因为它接收的就是中心点。

#ifndef BUILDING_COMPONENT_H
#define BUILDING_COMPONENT_H

#include "cocos2d.h"

class BuildingComponent
{
public:
    BuildingComponent(int tileW, int tileH);

    //root（node）代表建筑逻辑载体，不关心实际sprite多大
    void AttachTo(cocos2d::Node* parent);
    //接收逻辑格子坐标转成game_world本地坐标
    void SetGridPosition(float gridX, float gridY, cocos2d::Sprite* background);

    // === 视觉 ===
    //挂在node上
    void AttachSprite(cocos2d::Sprite* sprite);
    //自适应占地显示
    void FitSpriteToFootprint();

    // === 预览（先写好，不强制用） ===
    void ShowPreview(bool valid);
    void HidePreview();
    //建筑组件里原本就存着 sprite_，只需开放读取权限
    cocos2d::Sprite* GetSprite() const { return sprite_; }
    cocos2d::Node* GetRootNode() const { return root_; }

private:
    int tile_w_;
    int tile_h_;

    cocos2d::Node* root_ = nullptr;        // 建筑逻辑根节点
    cocos2d::Sprite* sprite_ = nullptr;    // 建筑图片
    cocos2d::DrawNode* preview_ = nullptr; // 占地预览
};

#endif
