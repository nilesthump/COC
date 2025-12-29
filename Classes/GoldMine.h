#ifndef __GOLD_MINE_H__
#define __GOLD_MINE_H__

#include "Building.h"
extern int global_gold_count, global_elixir_count,max_level;

class GoldMine : public Building
{
protected:
    int _generateSpeed=1;//金矿生产速度
    int maxSize = 100,currentSize=0;//金矿最大储量和现储量
    bool _productionDataSynced = false;
    int establishCost[2] = { 100,0 };//消耗的金币和圣水数量
    int upgradeCost[2] = { 50,50 };
    int upgradeTime = 5;
    bool initSprite(const std::string& textureName)override;
public:
    bool GoldMine::init(const std::string& textureName, int hp, int lv, float x0, float y0)override;

    int getUpgradeGoldCost()const override {
        return upgradeCost[0];
    }
    int getUpgradeElixirCost() const override {
        return upgradeCost[1];
    }
    bool canUpgrade()override {
        if (global_gold_count >= upgradeCost[0] && global_elixir_count >= upgradeCost[1]&&level < max_level) {
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
        _generateSpeed += 1;
        maxSize += 100;
        //加速成本升级
        upgradeTime = level * 5;//每次升级完成后，需要的升级时间对应延长
        isUpgrade = false;
        //换图
        _textureName = StringUtils::format("GoldMineLv%d.png", level);
        updateTexture(_textureName);
        playSuccessBlink();
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
    void clearCurrentStock() override {
        currentSize = 0;
    }
    void updateCurrentStock(int n) override {
        currentSize += n;
    }
    void addCurrent(int n) override {
        currentSize += n;
        if (currentSize > maxSize) {
            currentSize = maxSize;
        }
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
        return "GoldMine";
    }
    static GoldMine* create(const std::string& textureName, int hp = 100, int lv=1, float x0=667.0f, float y0 = 2074.0f);
};

#endif