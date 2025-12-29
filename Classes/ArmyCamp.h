#ifndef __ARMY_CAMP_H__
#define __ARMY_CAMP_H__

#include "Building.h"
extern int global_gold_count, global_elixir_count, max_level;

class ArmyCamp : public Building
{
protected:
    int max_size = 20,current_size = 0;
    int army[6];//代表六个兵种
    int space[6] = { 1,1,5,1,2,5 }, establish_cost[2] = { 200,200 };
    int upgrade_cost[2] = { 100,100 };
    int upgrade_time = 10;
    bool InitSprite(const std::string& textureName)override;
public:
    bool ArmyCamp::init(const std::string& textureName, int original_hp, int lv, float x0, float y0)override;

    int GetUpgradeGoldCost()const override {
        return upgrade_cost[0];
    }
    int GetUpgradeElixirCost() const override {
        return upgrade_cost[1];
    }
    bool CanUpgrade()override {
        if (global_gold_count >= upgrade_cost[0] && global_elixir_count >= upgrade_cost[1] && level < max_level) {
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
        max_size += 20;
        upgrade_time = level * 10;//每次升级完成后，需要的升级时间对应延长
        is_upgrade = false;
        //换图
        texture_name = StringUtils::format("ArmyCampLv%d.png", level);
        UpdateTexture(texture_name);
        PlaySuccessBlink();
    }
    int GetMaxStock() const override {
        return max_size;
    }
    int GetCurrentStock()const override {
        return current_size;
    }
    int GetArmy(int i)const override {
        return army[i];
    }
    int GetArmySize(int i)const override {
        return space[i];
    }
    void UpdateNum(int i) override {
        army[i]+=1;
        current_size +=space[i];
    }
    int GetGoldCost() const override {
        return establish_cost[0];
    }
    int GetElixirCost() const override {
        return establish_cost[1];
    }
    void FinishUpgrade()override {
        update();
    }
    void CutTime()override {
        upgrade_time--;
        if (upgrade_time <= 0) {
            update();
        }
    }
    int GetRemainTime() override{
        return upgrade_time;
    }
    int GetUpgradeDuration() const override {
        return upgrade_time;
    }
    std::string GetBuildingType() const override {
        return "ArmyCamp";
    }
    static ArmyCamp* create(const std::string& textureName, int original_hp = 100, int lv = 1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif 