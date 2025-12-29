#ifndef __TOWN_HALL_H__
#define __TOWN_HALL_H__

#include "Building.h"
extern int max_gold_volum, max_elixir_volum, max_level;
extern int global_gold_count, global_elixir_count;

class TownHall : public Building
{
protected:
    bool InitSprite(const std::string& textureName)override;
    int maxGoldNum = 1000, maxElixirNum = 1000;//最大储量
    int upgradeCost[2] = { 10,10 };
    int upgradeTime = 10;
public:
    bool TownHall::init(const std::string& textureName, int hp, int lv, float x0, float y0)override;
    int GetUpgradeGoldCost()const override {
        return upgradeCost[0];
    }
    int GetUpgradeElixirCost() const override{
        return upgradeCost[1];
    }
    bool CanUpgrade()override {
        if (global_gold_count >= upgradeCost[0] && global_elixir_count >= upgradeCost[1] && level < 15) {
            return true;
        }
        else {
            return false;
        }
    }
    void update()override {
        //公有属性
        level += 1;
        hp += 500;
        //私有属性
        maxGoldNum += 1000;
        maxElixirNum += 1000;
        //每次升级完成后，需要的升级时间对应延长
        upgradeTime = level * 15;
        is_upgrade = false;
        //更新全局变量
        max_gold_volum = maxGoldNum;
        max_elixir_volum = maxElixirNum;
        max_level = level;
        //换图
        texture_name = StringUtils::format("TownHallLv%d.png", level);
        UpdateTexture(texture_name);
        PlaySuccessBlink();
    }
    int GetMaxGoldNum()override {
        return maxGoldNum;
    }
    int GetMaxElixirNum() override {
        return maxElixirNum;
    }
    void FinishUpgrade()override {
        update();
    }
    void CutTime()override {
        upgradeTime--;
        if (upgradeTime <= 0) {
            update();         
        }
    }
    int GetRemainTime() override {
        return upgradeTime;
    }
    int GetUpgradeDuration() const override {
        return upgradeTime;
    }
    std::string GetBuildingType() const override {
        return "TownHall";
    }
    static TownHall* create(const std::string& textureName, int hp = 1000, int lv = 1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif 