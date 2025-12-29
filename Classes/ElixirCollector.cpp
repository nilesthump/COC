#include "ElixirCollector.h"
#include "SessionManager.h"

using namespace cocos2d;

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

ElixirCollector* ElixirCollector::create(const std::string& textureName, int original_hp, int lv, float x0, float y0)
{
    ElixirCollector* mine = new (std::nothrow) ElixirCollector();
    if (mine && mine->init(textureName, original_hp, lv, x0, y0))
    {
        mine->autorelease();
        return mine;
    }
    CC_SAFE_DELETE(mine);
    return nullptr;
}

bool ElixirCollector::init(const std::string& textureName, int original_hp,int lv, float x0, float y0)
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

    // 初始化储量属性
    maxSize = 100 + (level - 1) * 50;
    currentSize = 0;

    // 不再在建筑初始化时同步生产数据，改为由 SecondScene 统一管理
    // 这样可以避免重复同步和数据竞争问题

    size = 3;
    this->setPosition(Vec2(x0, y0));

    // 初始化精灵（关键：类内管理图像）
    if (!InitSprite(textureName))
    {
        return false;
    }

    // 设置锚点（这里设为中心，你可以根据需求改，比如(0,0)对齐网格）
    this->setAnchorPoint(Vec2(0.5f, 0.5f));
    building_sprite->setAnchorPoint(Vec2(0.5f, 0.5f));

    return true;
}

bool ElixirCollector::InitSprite(const std::string& textureName)
{
    building_sprite = Sprite::create(textureName);
    if (!building_sprite)
    {
        problemLoading("'goldMineLv1.png'");
        return false;
    }

    this->addChild(building_sprite);
    // 精灵缩放
    building_sprite->setScale(1.0f);

    return true;
}