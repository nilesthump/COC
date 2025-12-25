#ifndef __ELIXIR_STORAGE_H__
#define __ELIXIR_STORAGE_H__

#include "Building.h"

class ElixirStorage : public Building
{
protected:
    int addSize = 1000;
    bool initSprite(const std::string& textureName)override;
public:
    // 静态创建函数（Cocos推荐方式）
    bool ElixirStorage::init(const std::string& textureName, int hp, int lv, float x0, float y0)override;

    void update()override {
        //公有属性
        level += 1;
        _hp += 500;
        _textureName = StringUtils::format("ElixirStorageLv%d.png", level);
        updateTexture(_textureName);
        //私有属性
        addSize += 1000;    
        //全局属性需要增加
    }
    int getMaxStock() const override {
        return addSize;
    }

    static ElixirStorage* create(const std::string& textureName, int hp = 100, int lv = 1, float x0 = 667.0f, float y0 = 2074.0f);
};

#endif
