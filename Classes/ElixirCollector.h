#ifndef __ELIXIR_COLLECTOR_H__
#define __ELIXIR_COLLECTOR_H__

#include "Building.h"
extern int global_gold_count, global_elixir_count,max_level;

class ElixirCollector : public Building
{
protected:
    int generate_speed = 1;//生产速度
    int max_size = 100, current_size = 0;//最大储量
    int establish_cost[2] = { 0,100 };//消耗的金币和圣水数量
    int upgrade_cost[2] = { 50,50 };
    int upgrade_time = 5;
    bool InitSprite(const std::string& textureName)override;
public:

    bool ElixirCollector::init(const std::string& textureName, int original_hp,int lv, float x0, float y0)override;

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
        generate_speed += 1;
        max_size += 50;
        //加速成本升级
        upgrade_time = level * 5;//每次升级完成后，需要的升级时间对应延长
        is_upgrade = false;//结束升级ing状态
        //换图
        texture_name = StringUtils::format("ElixirCollectorLv%d.png", level);
        UpdateTexture(texture_name);
        PlaySuccessBlink();
    }
    int GetSpeed()const override {
        return generate_speed;
    }
    int GetMaxStock() const override {
        return max_size;
    }
    int GetCurrentStock()const override {
        return current_size;
    }
    void ClearCurrentStock() override {
        current_size = 0;
    }
    void UpdateCurrentStock(int n) override {
        current_size += n;
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
    int GetRemainTime() override {
        return upgrade_time;
    }
    int GetUpgradeDuration() const override {
        return upgrade_time;
    }
    std::string GetBuildingType() const override {
        return "ElixirCollector";
    }
    static ElixirCollector* create(const std::string& textureName, int original_hp = 100, int lv=1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif
