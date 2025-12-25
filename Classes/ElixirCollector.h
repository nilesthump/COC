#ifndef __ELIXIR_COLLECTOR_H__
#define __ELIXIR_COLLECTOR_H__

#include "Building.h"

class ElixirCollector : public Building
{
protected:
    float _generateSpeed = 1.0f;//生产速度
    int maxSize = 50, currentSize = 0;//最大储量
    bool initSprite(const std::string& textureName)override;
public:
    // 静态创建函数（Cocos推荐方式）
    bool ElixirCollector::init(const std::string& textureName, int hp,int lv, float x0, float y0)override;

    void update()override {
        //公有属性
        level += 1;
        _hp += 500;
        _textureName = StringUtils::format("ElixirCollectorLv%d.png", level);
        updateTexture(_textureName);
        //私有属性
        _generateSpeed += 1.0;
        maxSize += 50;
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
    void updateCurrentStock() override {
        if (currentSize + _generateSpeed <= maxSize) {
            currentSize += _generateSpeed;
        }
        else {
            currentSize = maxSize;
        }
    }

    static ElixirCollector* create(const std::string& textureName, int hp = 100, int lv=1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif
