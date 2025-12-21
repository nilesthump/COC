#include "GoldMine.h"
#include "cocos2d.h"

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

GoldMine* GoldMine::create(int gridX, int gridY, int initHp, int initLevel, float initGoldSpeed, const std::string& texturePath) {
    GoldMine* mine = new (std::nothrow) GoldMine();
    if (mine && mine->init(gridX, gridY, initHp, initLevel, initGoldSpeed, texturePath)) {
        mine->autorelease();
        return mine;
    }
    CC_SAFE_DELETE(mine);
    return nullptr;
}

bool GoldMine::init(int gridX, int gridY, int initHp, int initLevel, float initGoldSpeed, const std::string& texturePath) {
    if (!Node::init()) {
        return false;
    }

    // 初始化你的private成员
    X = gridX;
    Y = gridY;
    Hp = initHp;
    level = initLevel;
    goldGenerateSpeed = initGoldSpeed;
    // Node的position可以和网格坐标关联（比如网格转世界坐标）
    _position = cocos2d::Vec2(X * 64, Y * 64);  // 假设每个网格64像素
    this->setPosition(_position);  // 用Node自带的position管理显示坐标

    // 初始化图像
    if (!initSprite(texturePath)) {
        return false;
    }

    return true;
}

bool GoldMine::initSprite(const std::string& texturePath) {
    _mineSprite = cocos2d::Sprite::create(texturePath);
    if (!_mineSprite) {
        problemLoading("'btn_pressed.png' (作为金币图标的替代)");
        return false;
    }
    this->addChild(_mineSprite);  // 精灵作为子节点挂载
    _mineSprite->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));  // 居中显示
    return true;
}

void GoldMine::takeDamage(int damage) {
    Hp = std::max(0, Hp - damage);
    if (Hp <= 0) {
        this->setVisible(false);  // 血量为0时隐藏金矿
    }
}

void GoldMine::upgradeLevel() {
    level++;
    goldGenerateSpeed *= 1.5f;  // 升级后产金速度提升50%
}