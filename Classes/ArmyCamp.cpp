#include "ArmyCamp.h"

using namespace cocos2d;

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

ArmyCamp* ArmyCamp::create(const std::string& textureName, int hp, int lv, float goldSpeed, float x0, float y0, int max, int current)
{
    ArmyCamp* mine = new (std::nothrow) ArmyCamp();
    if (mine && mine->init(textureName, hp, lv, goldSpeed, x0, y0, max, current))
    {
        mine->autorelease();
        return mine;
    }
    CC_SAFE_DELETE(mine);
    return nullptr;
}

bool ArmyCamp::init(const std::string& textureName, int hp, int lv, float generateSpeed, float x0, float y0, int max, int current)
{
    if (!Node::init())
    {
        return false;
    }

    // 初始化核心属性
    _hp = hp;
    _generateSpeed = generateSpeed;
    _textureName = textureName;
    x = x0;
    y = y0;
    level = lv;
    maxSize = max;//maxSize为兵营容量
    currentSize = current;//现存量
    this->setPosition(Vec2(x0, y0));
    std::fill(army, army + 6, 0);
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

// 生产金币到库存（核心：判断上限）
void ArmyCamp::produceToStock(int gold)
{
    if (currentSize + gold <= maxSize)
    {
        currentSize += gold;
    }
    else
    {
        currentSize = maxSize; // 达到上限则填满
    }
}

// 收集库存（玩家点击金矿时调用）
int ArmyCamp::collectStock()
{
    int collected = currentSize;
    currentSize = 0; // 清空库存
    return collected;
}

bool ArmyCamp::initSprite(const std::string& textureName)
{
    _sprite = Sprite::create(textureName);
    if (!_sprite)
    {
        problemLoading("'ArmyCampLv1.png'");
        return false;
    }

    // 精灵作为子节点挂载到金矿节点
    this->addChild(_sprite);
    // 精灵缩放（根据你的需求调整，这里设为1.0f）
    _sprite->setScale(1.0f);

    return true;
}

