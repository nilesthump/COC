#ifndef __CANNON_H__
#define __CANNON_H__

#include "Building.h"
extern int global_gold_count, global_elixir_count, max_level;

class Cannon : public Building
{
protected:
    int establishCost[2] = { 50,50 };
    int upgradeCost[2] = { 25,25 };
    int upgradeTime = 10;
    bool InitSprite(const std::string& textureName)override;
public:
    bool Cannon::init(const std::string& textureName, int original_hp, int lv, float x0, float y0)override;

    int GetUpgradeGoldCost()const override {
        return upgradeCost[0];
    }
    int GetUpgradeElixirCost() const override {
        return upgradeCost[1];
    }
    bool CanUpgrade()override {
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
        hp += 1000;
        upgradeTime = level * 3;//每次升级完成后，需要的升级时间对应延长
        is_upgrade = false;
        //换图
        texture_name = StringUtils::format("CannonLv%d.png", level);
        UpdateTexture(texture_name);
        PlaySuccessBlink();
    }
    int GetGoldCost() const override {
        return establishCost[0];
    }
    int GetElixirCost() const override {
        return establishCost[1];
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
    std::string GetBuildingType() const override {
        return "Cannon";
    }
    static Cannon* create(const std::string& textureName, int original_hp = 1000, int lv = 1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif 