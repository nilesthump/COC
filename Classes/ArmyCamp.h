#ifndef __ARMY_CAMP_H__
#define __ARMY_CAMP_H__

#include "Building.h"

class ArmyCamp : public Building
{
protected:
    bool initSprite(const std::string& textureName)override;
public:
    // 静态创建函数（Cocos推荐方式）
    bool ArmyCamp::init(const std::string& textureName, int hp, int lv, float generateSpeed, float x0, float y0, int max, int current)override;

    void produceToStock(int gold);                          // 生产金币到库存（带上限）
    int collectStock();                                     // 收集库存（清空并返回数量）

    static ArmyCamp* create(const std::string& textureName, int hp = 100, int lv = 1, float goldSpeed = 0.0f, float x0 = 667.0f, float y0 = 2074.0f, int max = 120, int current = 0);
};

#endif // __GOLD_MINE_H__