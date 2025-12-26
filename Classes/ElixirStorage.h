#ifndef __ELIXIR_STORAGE_H__
#define __ELIXIR_STORAGE_H__

#include "Building.h"
extern int g_goldCount, g_elixirCount,maxLevel,maxElixirVolum;

class ElixirStorage : public Building
{
protected:
    int addSize = 1000,currentSize = 0;
    int establishCost[2] = { 100,100 };
    int upgradeCost[2] = { 50,50 };
    int upgradeTime = 10;
    bool initSprite(const std::string& textureName)override;
public:
    // 静态创建函数（Cocos推荐方式）
    bool ElixirStorage::init(const std::string& textureName, int hp, int lv, float x0, float y0)override;


    //功能相关
    int getMaxStock() const override {
        return addSize;
    }
    int getCurrentStock() const override{ 
        return currentSize;
    }
    void clearCurrentStock()override {
        if (g_elixirCount + currentSize <= maxElixirVolum) {
            currentSize = 0;
            g_elixirCount += currentSize;
        }
        else {
            currentSize -= (maxElixirVolum - g_elixirCount);
            g_elixirCount = maxElixirVolum;
        }
    }
    void addCurrent(int n) {
        currentSize += n;
    }
    //升级相关
    int getUpgradeGoldCost()const override {
        return upgradeCost[0];
    }
    int getUpgradeElixirCost() const override {
        return upgradeCost[1];
    }
    bool canUpgrade()override {
        if (g_goldCount >= upgradeCost[0] && g_elixirCount >= upgradeCost[1] && level < maxLevel) {
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
        addSize += 1000;
        //全局属性需要增加
        upgradeTime = level * 10;//每次升级完成后，需要的升级时间对应延长
        isUpgrade = false;
        //换图
        _textureName = StringUtils::format("ElixirStorageLv%d.png", level);
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

    static ElixirStorage* create(const std::string& textureName, int hp = 100, int lv = 1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif
