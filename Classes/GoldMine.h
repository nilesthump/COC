#ifndef __GOLD_MINE_H__
#define __GOLD_MINE_H__

#include "Building.h"

class GoldMine : public Building
{
protected:
    bool initSprite(const std::string& textureName)override;
public:
    // 静态创建函数（Cocos推荐方式）
    bool GoldMine::init(const std::string& textureName, int hp, int lv, float generateSpeed, float x0, float y0)override;

    static GoldMine* create(const std::string& textureName, int hp = 100, int lv=1, float goldSpeed = 1.0f, float x0=667.0f, float y0 = 2074.0f);
};

#endif // __GOLD_MINE_H__