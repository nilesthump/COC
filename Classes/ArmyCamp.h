#ifndef __ARMY_CAMP_H__
#define __ARMY_CAMP_H__

#include "Building.h"

class ArmyCamp : public Building
{
protected:
    int maxSize = 20,currentSize = 0;
    int army[6];//代表六个兵种
    int size[6] = { 1,1,5,1,2,5 }, cost[2] = { 200,200 };
    int upgradeTime = 10;
    bool initSprite(const std::string& textureName)override;
public:
    // 静态创建函数（Cocos推荐方式）
    bool ArmyCamp::init(const std::string& textureName, int hp, int lv, float x0, float y0)override;

    void update()override {
        //公有属性
        level += 1;
        _hp += 500;
        //私有属性
        maxSize += 20;
        upgradeTime = level * 10;//每次升级完成后，需要的升级时间对应延长
        isUpgrade = false;
        //换图
        _textureName = StringUtils::format("ArmyCampLv%d.png", level);
        updateTexture(_textureName);
        playSuccessBlink();
    }
    int getMaxStock() const override {
        return maxSize;
    }
    int getCurrentStock()const override {
        return currentSize;
    }
    int getArmy(int i)const override {
        return army[i];
    }
    int getArmySize(int i)const override {
        return size[i];
    }
    void updateNum(int i) override {
        army[i]+=1;
        currentSize +=size[i];
    }
    int getGoldCost() const override {
        return cost[0];
    }
    int getElixirCost() const override {
        return cost[1];
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
    int getRemainTime() override{
        return upgradeTime;
    }

    static ArmyCamp* create(const std::string& textureName, int hp = 100, int lv = 1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif // __GOLD_MINE_H__