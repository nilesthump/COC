#ifndef __ELIXIR_COLLECTOR_H__
#define __ELIXIR_COLLECTOR_H__

#include "Building.h"

class ElixirCollector : public Building
{
protected:
    bool initSprite(const std::string& textureName)override;
public:
    // 静态创建函数（Cocos推荐方式）
    bool ElixirCollector::init(const std::string& textureName, int hp,int lv, float generateSpeed, float x0, float y0,int max,int current)override;

    void produceToStock(int elixir);                          // 生产圣水到库存（带上限）
    int collectStock();                                     // 收集库存（清空并返回数量）

    void update()override {
        //公有属性
        level += 1;
        _hp += 500;
        _textureName = StringUtils::format("ElixirCollectorLv%d.png", level);
        //私有属性
        _generateSpeed += 1.0;
        maxSize += 100;
    }

    static ElixirCollector* create(const std::string& textureName, int hp = 100, int lv=1, float goldSpeed = 1.0f, float x0 = 667.0f, float y0 = 2074.0f,int max=50, int current=0);
};

#endif
