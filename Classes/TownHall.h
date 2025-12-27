#ifndef __TOWN_HALL_H__
#define __TOWN_HALL_H__

#include "Building.h"
extern int maxGoldVolum, maxElixirVolum, maxLevel;
extern int g_goldCount, g_elixirCount;

class TownHall : public Building
{
protected:
    bool initSprite(const std::string& textureName)override;
    int maxGoldNum = 1000, maxElixirNum = 1000;//最大储量
    int upgradeCost[2] = { 10,10 };
    int upgradeTime = 10;
public:
    // 静态创建函数（Cocos推荐方式）
    bool TownHall::init(const std::string& textureName, int hp, int lv, float x0, float y0)override;
    int getUpgradeGoldCost()const override {
        return upgradeCost[0];
    }
    int getUpgradeElixirCost() const override{
        return upgradeCost[1];
    }
    bool canUpgrade()override {
        if (g_goldCount >= upgradeCost[0] && g_elixirCount >= upgradeCost[1] && level < 15) {
            return true;
        }
        else {
            return false;
        }
    }
    void update()override {
        //公有属性
        level += 1;
        _hp += 500;
        //私有属性
        maxGoldNum += 1000;
        maxElixirNum += 1000;
        //每次升级完成后，需要的升级时间对应延长
        upgradeTime = level * 15;
        isUpgrade = false;
        //换图
        _textureName = StringUtils::format("TownHallLv%d.png", level);
        updateTexture(_textureName);
        playSuccessBlink();
    }
    int getMaxGoldNum()override {
        return maxGoldNum;
    }
    int getMaxElixirNum() override {
        return maxElixirNum;
    }
    void finishUpgrade()override {
        update();
    }
    void cutTime()override {
        upgradeTime--;
        if (upgradeTime <= 0) {
            update();
            //更新全局变量
            maxGoldVolum = maxGoldNum;
            maxElixirVolum = maxElixirNum;
            maxLevel = level;
        }
    }
    int getRemainTime() override {
        return upgradeTime;
    }
    std::string getBuildingType() const override {
        return "TownHall";
    }
    static TownHall* create(const std::string& textureName, int hp = 1000, int lv = 1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif 