#ifndef __BUILDER_HUT_H__
#define __BUILDER_HUT_H__

#include "Building.h"
extern int g_goldCount, g_elixirCount,maxLevel;

class BuilderHut : public Building
{
protected:
    int establishCost[2] = { 10,10 };
    int upgradeCost[2] = { 5,5 };
    int upgradeTime = 1;
    bool initSprite(const std::string& textureName)override;
public:
    // 静态创建函数（Cocos推荐方式）
    bool BuilderHut::init(const std::string& textureName, int hp, int lv, float x0, float y0)override;

    int getUpgradeGoldCost()const override {
        return upgradeCost[0];
    }
    int getUpgradeElixirCost() const override {
        return upgradeCost[1];
    }
    bool canUpgrade()override {
        if (g_goldCount >= upgradeCost[0] && g_elixirCount >= upgradeCost[1] && level < maxLevel&& level < 7) {
            return true;
        }
        else {
            return false;
        }
    }
    void update()override {
        //公有属性
        level += 1;
        _hp += 1000;     
        upgradeTime = 1;//每次升级完成后，需要的升级时间对应延长
        isUpgrade = false;
        //换图
        _textureName = StringUtils::format("BuilderHutLv%d.png", level);
        updateTexture(_textureName);
        playSuccessBlink();
    }
    int getGoldCost() const override {
        return establishCost[0];
    }
    int getElixirCost() const override {
        return establishCost[1];
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

    static BuilderHut* create(const std::string& textureName, int hp = 1000, int lv = 1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif 