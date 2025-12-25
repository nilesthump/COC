#ifndef __GOLD_STORAGE_H__
#define __GOLD_STORAGE_H__

#include "Building.h"

class GoldStorage : public Building
{
protected:
    int addSize = 1000;//额外储量
    bool initSprite(const std::string& textureName)override;
public:
    // 静态创建函数（Cocos推荐方式）
    bool GoldStorage::init(const std::string& textureName, int hp, int lv, float x0, float y0)override;

    void update()override {
        //公有属性
        level += 1;
        _hp += 500;
        _textureName = StringUtils::format("GoldStorageLv%d.png", level);
        updateTexture(_textureName);
        //私有属性
        addSize += 1000;
    }
    int getMaxStock() const override {
        return addSize;
    }

    static GoldStorage* create(const std::string& textureName, int hp = 100, int lv = 1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif