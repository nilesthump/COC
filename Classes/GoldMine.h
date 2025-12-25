#ifndef __GOLD_MINE_H__
#define __GOLD_MINE_H__

#include "Building.h"

class GoldMine : public Building
{
protected:
    float _generateSpeed=1.0f;//金矿生产速度
    int maxSize = 100,currentSize=0;//金矿最大储量和现储量
    bool initSprite(const std::string& textureName)override;
public:
    // 静态创建函数（Cocos推荐方式）
    bool GoldMine::init(const std::string& textureName, int hp, int lv, float x0, float y0)override;

    void update()override {
        //公有属性
        level += 1;
        _hp += 500;
        _textureName = StringUtils::format("GoldMineLv%d.png", level);
        updateTexture(_textureName);
        //私有属性
        _generateSpeed += 1.0f;
        maxSize += 100;
    }
    int getSpeed()const override {
        return _generateSpeed;
    }
    int getMaxStock() const override {
        return maxSize;
    }
    int getCurrentStock()const override {
        return currentSize;
    }
    void updateCurrentStock() override {
        if (currentSize + _generateSpeed <= maxSize) {
            currentSize += _generateSpeed;
        }
        else {
            currentSize = maxSize;
        }
    }

    static GoldMine* create(const std::string& textureName, int hp = 100, int lv=1, float x0=667.0f, float y0 = 2074.0f);
};

#endif // __GOLD_MINE_H__