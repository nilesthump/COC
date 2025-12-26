#ifndef __BUILDER_HUT_H__
#define __BUILDER_HUT_H__

#include "Building.h"

class BuilderHut : public Building
{
protected:
    int cost[2] = { 20,10 };
    bool initSprite(const std::string& textureName)override;
public:
    // 静态创建函数（Cocos推荐方式）
    bool BuilderHut::init(const std::string& textureName, int hp, int lv, float x0, float y0)override;

    void update()override {
        //公有属性
        level += 1;
        _hp += 1000;
        //换图
        _textureName = StringUtils::format("BuilderHutLv%d.png", level);
        updateTexture(_textureName);

    }
    int getGoldCost() const override {
        return cost[0];
    }
    int getElixirCost() const override {
        return cost[1];
    }

    static BuilderHut* create(const std::string& textureName, int hp = 1000, int lv = 1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif 