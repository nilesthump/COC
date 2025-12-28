#include "GoldMine.h"

using namespace cocos2d;

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

GoldMine* GoldMine::create(const std::string& textureName, int hp,int lv, float x0, float y0)
{
    GoldMine* mine = new (std::nothrow) GoldMine();
    if (mine && mine->init(textureName, hp,lv, x0, y0))
    {
        mine->autorelease();
        return mine;
    }
    CC_SAFE_DELETE(mine);
    return nullptr;
}

bool GoldMine::init(const std::string& textureName, int hp,int lv, float x0, float y0)
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
    size = 3;
    this->setPosition(Vec2(x0, y0));

    // 初始化精灵（关键：类内管理图像）
    if (!initSprite(textureName))
    {
        return false;
    }

    // 设置锚点（这里设为中心，你可以根据需求改，比如(0,0)对齐网格）
    this->setAnchorPoint(Vec2(0.5f, 0.5f));
    _sprite->setAnchorPoint(Vec2(0.5f, 0.5f));

    return true;
}

bool GoldMine::initSprite(const std::string& textureName)
{
    _sprite = Sprite::create(textureName);
    if (!_sprite)
    {
        problemLoading("'GoldMineLv1.png'");
        return false;
    }

    // 精灵作为子节点挂载到金矿节点
    this->addChild(_sprite);
    // 精灵缩放（根据你的需求调整，这里设为1.0f）
    _sprite->setScale(1.0f);

    return true;
}

