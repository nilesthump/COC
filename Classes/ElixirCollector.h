#ifndef __ELIXIR_COLLECTOR_H__
#define __ELIXIR_COLLECTOR_H_

#include "Building.h"

class ElixirCollector : public Building
{
protected:
    bool initSprite(const std::string& textureName)override;
public:
    // 静态创建函数（Cocos推荐方式）
    bool ElixirCollector::init(const std::string& textureName, int hp, float generateSpeed, float x0, float y0)override;

    static ElixirCollector* create(const std::string& textureName, int hp = 100, float goldSpeed = 1.0f, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif
