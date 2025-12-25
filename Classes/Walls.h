#ifndef __WALLS_H__
#define __WALLS_H__

#include "Building.h"

class Walls : public Building
{
protected:
    bool initSprite(const std::string& textureName)override;
public:
    // 静态创建函数（Cocos推荐方式）
    bool Walls::init(const std::string& textureName, int hp, int lv, float x0, float y0)override;

    void update()override {
        //公有属性
        level += 1;
        _hp += 1000;
        //换图
        _textureName = StringUtils::format("WallsLv%d.png", level);
        updateTexture(_textureName);
    }

    static Walls* create(const std::string& textureName, int hp = 1000, int lv = 1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif 