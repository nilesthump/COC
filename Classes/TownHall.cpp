#include "TownHall.h"

using namespace cocos2d;

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

TownHall* TownHall::create(const std::string& textureName, int original_hp, int lv, float x0, float y0)
{
    TownHall* mine = new (std::nothrow) TownHall();
    if (mine && mine->init(textureName, original_hp, lv, x0, y0))
    {
        mine->autorelease();
        return mine;
    }
    CC_SAFE_DELETE(mine);
    return nullptr;
}

bool TownHall::init(const std::string& textureName, int original_hp, int lv, float x0, float y0)
{
    if (!Node::init())
    {
        return false;
    }

    // 初始化核心属性
    hp = original_hp;
    texture_name = textureName;
    x = x0;
    y = y0;
    level = lv;
    size = 4;
    this->setPosition(Vec2(x0, y0));
    // 初始化精灵
    if (!InitSprite(textureName))
    {
        return false;
    }

    // 设置锚点
    this->setAnchorPoint(Vec2(0.5f, 0.5f));
    building_sprite->setAnchorPoint(Vec2(0.5f, 0.5f));

    return true;
}

bool TownHall::InitSprite(const std::string& textureName)
{
    building_sprite = Sprite::create(textureName);
    if (!building_sprite)
    {
        problemLoading("'TownHallLv1.png'");
        return false;
    }

    this->addChild(building_sprite);
    // 精灵缩放
    building_sprite->setScale(1.0f);

    return true;
}

