#ifndef __ELIXIR_STORAGE_H__
#define __ELIXIR_STORAGE_H__

#include "Building.h"
extern int global_gold_count, global_elixir_count,max_level,max_elixir_volum;

class ElixirStorage : public Building
{
protected:
    int add_size = 1000,current_size = 0;
    int establish_cost[2] = { 100,100 };
    int upgrade_cost[2] = { 50,50 };
    int upgrade_time = 10;
    bool InitSprite(const std::string& textureName)override;
public:

    bool ElixirStorage::init(const std::string& textureName, int original_hp, int lv, float x0, float y0)override;


    //功能相关
    int GetMaxStock() const override {
        return add_size;
    }
    int GetCurrentStock() const override{ 
        return current_size;
    }
    void ClearCurrentStock()override {
        if (global_elixir_count + current_size <= max_elixir_volum) {
            current_size = 0;
            global_elixir_count += current_size;
        }
        else {
            current_size -= (max_elixir_volum - global_elixir_count);
            global_elixir_count = max_elixir_volum;
        }
    }
    void AddCurrent(int n) {
        current_size += n;
    }
    //升级相关
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
        add_size += 1000;
        //全局属性需要增加
        upgrade_time = level * 10;//每次升级完成后，需要的升级时间对应延长
        is_upgrade = false;
        //换图
        texture_name = StringUtils::format("ElixirStorageLv%d.png", level);
        UpdateTexture(texture_name);
        PlaySuccessBlink();
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
        return "ElixirStorage";
    }
    static ElixirStorage* create(const std::string& textureName, int original_hp = 100, int lv = 1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif
