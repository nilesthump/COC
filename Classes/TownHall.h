#ifndef __TOWN_HALL_H__
#define __TOWN_HALL_H__

#include "Building.h"

class TownHall : public Building
{
protected:
    bool initSprite(const std::string& textureName)override;
    int maxGoldNum = 10000, maxElixirNum = 10000;
public:
    // 静态创建函数（Cocos推荐方式）
    bool TownHall::init(const std::string& textureName, int hp, int lv, float generateSpeed, float x0, float y0, int max, int current)override;
    int getMaxGoldNum()override {
        return maxGoldNum;
    }
    int getMaxElixirNum() override{
        return maxElixirNum;
    }
    void update()override {
        //公有属性
        level += 1;
        _hp += 500;
        _textureName= StringUtils::format("TownHallLv%d.png", level);
        //私有属性
        maxGoldNum += 50000;
        maxElixirNum += 25000;
    }


    static TownHall* create(const std::string& textureName, int hp = 1000, int lv = 1, float goldSpeed = 0.0f, float x0 = 667.0f, float y0 = 2074.0f, int max = 100, int current = 0);
};

#endif 