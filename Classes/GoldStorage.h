#ifndef __GOLD_STORAGE_H__
#define __GOLD_STORAGE_H__

#include "Building.h"
extern int global_gold_count, global_elixir_count,max_level,max_gold_volum;

class GoldStorage : public Building
{
protected:
    int addSize = 1000, currentSize = 0;//额外储量
    int establishCost[2] = { 100,100 };
    int upgradeCost[2] = { 50,50 };
    int upgradeTime = 10;
    bool initSprite(const std::string& textureName)override;
public:
    bool GoldStorage::init(const std::string& textureName, int hp, int lv, float x0, float y0)override;

    //功能相关
    int getMaxStock() const override {
        return addSize;
    }
    int getCurrentStock() const override {
        return currentSize;
    }
    void clearCurrentStock()override {
        if (global_gold_count + currentSize <= max_gold_volum) {
            currentSize = 0;
            global_gold_count += currentSize;
        }
        else {
            currentSize -= (max_gold_volum - global_gold_count);
            global_gold_count = max_gold_volum;
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
        if (global_gold_count >= upgradeCost[0] && global_elixir_count >= upgradeCost[1] && level < max_level) {
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
        //加速成本升级
        upgradeTime = level * 10;//每次升级完成后，需要的升级时间对应延长
        isUpgrade = false;
        //换图
        _textureName = StringUtils::format("GoldStorageLv%d.png", level);
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
    int getUpgradeDuration() const override {
        return upgradeTime;
    }
    std::string getBuildingType() const override {
        return "GoldStorage";
    }
    static GoldStorage* create(const std::string& textureName, int hp = 100, int lv = 1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif