#include "BuildingComponentYZL.h"
#include "ConvertTest.h"

USING_NS_CC;

BuildingComponent::BuildingComponent(int tileW, int tileH)
    : tile_w_(tileW), tile_h_(tileH)
{
}

Vec2 BuildingComponent::GetCenterOffset() const
{
    return Vec2(
        (tile_w_ % 2 == 0) ? 0.5f : 0.0f,
        (tile_h_ % 2 == 0) ? 0.5f : 0.0f
    );
}

void BuildingComponent::AttachTo(Node* parent)
{
    root_ = Node::create();
    root_->setAnchorPoint(Vec2(0.5f, 0.5f));
    parent->addChild(root_);

    preview_ = DrawNode::create();
    root_->addChild(preview_, -1);
    
}

void BuildingComponent::SetGridPosition(float gridX, float gridY, Sprite* background)
{
    Vec2 pos = Vec2(gridX, gridY) + GetCenterOffset();
    Vec2 local = ConvertTest::convertGridToLocal(pos.x, pos.y, background);
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

    Color4F color = valid
        ? Color4F(0, 1, 0, 0.35f)
        : Color4F(1, 0, 0, 0.35f);

    // 注意：这里用的是像素尺寸
    float w = tile_w_ * 56.0f;
    float h = tile_h_ * 42.0f;

    Vec2 origin(-w / 2, -h / 2);

    preview_->drawSolidRect(
        origin,
        origin + Vec2(w, h),
        color
    );
}

void BuildingComponent::HidePreview()
{
    if (preview_)
        preview_->clear();
}
