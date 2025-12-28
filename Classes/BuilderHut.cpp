#include "BuilderHut.h"

using namespace cocos2d;

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

BuilderHut* BuilderHut::create(const std::string& textureName, int hp, int lv, float x0, float y0)
{
    BuilderHut* mine = new (std::nothrow) BuilderHut();
    if (mine && mine->init(textureName, hp, lv, x0, y0))
    {
        mine->autorelease();
        return mine;
    }
    CC_SAFE_DELETE(mine);
    return nullptr;
}

bool BuilderHut::init(const std::string& textureName, int hp, int lv, float x0, float y0)
{
    if (!Node::init())
    {
        return false;
    }

    // 初始化核心属性
    _hp = hp;
    _textureName = textureName;
    x = x0;
    y = y0;
    level = lv;
    size = 2;
    this->setPosition(Vec2(x0, y0));
    // 初始化精灵
    if (!initSprite(textureName))
    {
        return false;
    }

    // 设置锚点
    this->setAnchorPoint(Vec2(0.5f, 0.5f));
    _sprite->setAnchorPoint(Vec2(0.5f, 0.5f));

    return true;
}

bool BuilderHut::initSprite(const std::string& textureName)
{
    _sprite = Sprite::create(textureName);
    if (!_sprite)
    {
        problemLoading("'BuilderHut.png'");
        return false;
    }

    this->addChild(_sprite);
    // 精灵缩放
    _sprite->setScale(1.0f);

    return true;
}

