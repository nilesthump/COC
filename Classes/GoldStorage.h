#ifndef __GOLD_STORAGE_H__
#define __GOLD_STORAGE_H__

#include "Building.h"

class GoldStorage : public Building
{
protected:
    int addSize = 1000;//额外储量
    int cost[2] = { 100,100 };
    int upgradeTime = 10;
    bool initSprite(const std::string& textureName)override;
public:
    // 静态创建函数（Cocos推荐方式）
    bool GoldStorage::init(const std::string& textureName, int hp, int lv, float x0, float y0)override;

    void update()override {
        //公有属性
        level += 1;
        _hp += 500;
        //私有属性
        addSize += 1000;
        //加速成本升级
        upgradeTime = level * 10;//每次升级完成后，需要的升级时间对应延长
        isUpgrade = false;
        //换图
        _textureName = StringUtils::format("GoldStorageLv%d.png", level);
        updateTexture(_textureName);
        playSuccessBlink();
    }
    int getMaxStock() const override {
        return addSize;
    }
    int getGoldCost() const override {
        return cost[0];
    }
    int getElixirCost() const override {
        return cost[1];
    }
    void finishUpgrade()override {
        update();
    }
    void cutTime()override {
        upgradeTime--;
        if (upgradeTime <= 0) {
            update();
        }
    }
    int getRemainTime() override {
        return upgradeTime;
    }
    static GoldStorage* create(const std::string& textureName, int hp = 100, int lv = 1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif