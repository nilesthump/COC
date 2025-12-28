#include "BuildingComponentYZL.h"
#include "ConvertTest.h"

USING_NS_CC;

BuildingComponent::BuildingComponent(int tileW, int tileH)
    : tile_w_(tileW), tile_h_(tileH)
{
}

void BuildingComponent::AttachTo(Node* parent)
{
    if (!parent) return;
    root_ = Node::create();
    root_->setAnchorPoint(Vec2(0.5f, 0.5f));
    parent->addChild(root_);

    preview_ = DrawNode::create();
    root_->addChild(preview_, -1);
}

void BuildingComponent::SetGridPosition(float gridX, float gridY, Sprite* background)
{
    // 直接使用传入的坐标，不要再 + GetCenterOffset()
    // 因为 b.logicalPos 已经是对的了
    Vec2 local = ConvertTest::convertGridToLocal(gridX, gridY, background);
    root_->setPosition(local);
}

void BuildingComponent::AttachSprite(Sprite* sprite)
{
    sprite_ = sprite;
    if (!sprite_ || !root_) return;

    sprite_->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprite_->setPosition(Vec2::ZERO);
    root_->addChild(sprite_);
    
}

void BuildingComponent::FitSpriteToFootprint()
{
    if (!sprite_) return;

    // 目标占地尺寸（像素）
    float targetW = tile_w_ * 56.0f;
    float targetH = tile_h_ * 42.0f;

    // 图片原始尺寸
    float spriteW = sprite_->getContentSize().width;
    float spriteH = sprite_->getContentSize().height;

    // 计算两个方向的缩放比例
    float scaleX = targetW / spriteW;
    float scaleY = targetH / spriteH;

    // 使用较小的缩放比例，保持图片不变形
    float finalScale = std::min(scaleX, scaleY);

    sprite_->setScale(finalScale);

}

// ====== 预览占地（先写好） ======

void BuildingComponent::ShowPreview(bool valid)
{
    if (!preview_) return;
    preview_->clear();

    Color4F color = valid ? Color4F(0, 1, 0, 0.35f) : Color4F(1, 0, 0, 0.35f);

    // 计算菱形的四个顶点（基于 root_ 中心点）
    float halfW = (tile_w_ * 56.0f) / 2.0f;
    float halfH = (tile_h_ * 42.0f) / 2.0f;

    // 定义菱形的四个角点
    Vec2 vertices[4] = {
        Vec2(0, halfH),   // 顶点
        Vec2(halfW, 0),   // 右点
        Vec2(0, -halfH),  // 底点
        Vec2(-halfW, 0)   // 左点
    };

    // 画一个实心的多边形（菱形）
    preview_->drawSolidPoly(vertices, 4, color);
}

void BuildingComponent::HidePreview()
{
    if (preview_)
        preview_->clear();
}
