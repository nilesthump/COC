#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "cocos2d.h"
#include"ConvertTest.h"

class Building : public cocos2d::Node {
protected:
    // 核心属性
    int _hp;                  // 血量
    int level;
    float _generateSpeed;     // 生产速度
    std::string _textureName; // 纹理名称
    cocos2d::Sprite* _sprite; // 图像精灵
    float x, y;               //世界坐标
    float size = 3.0f;        //尺寸
    int maxSize, currentSize; //最大容量，当前容量
    // 初始化精灵（内部调用）
    virtual bool initSprite(const std::string& textureName) = 0;
public:
    
    // 初始化函数
    virtual bool init(const std::string& textureName, int hp, int lv, float generateSpeed, float x0, float y0, int maxSize,int currentSize)=0;

    //修改Building坐标
    void changeX(float t) {
        x = t;
    }
    void changeY(float t) {
        y = t;
    }
    //升级
    void updateLv() {
        level++;
    }
    void updateHp() {
        _hp = _hp + 50 * level;
    }
    void updateSpeed() {
        _generateSpeed = _generateSpeed * level;
    }
    void updateSize() {
        maxSize = maxSize + maxSize * level;
    }
    void updateTexture(const std::string& newTextureName) {
        if (_sprite) {
            // 尝试加载新纹理
            auto newTexture = Director::getInstance()->getTextureCache()->addImage(newTextureName);
            if (newTexture) {
                _sprite->setTexture(newTexture);
            }
            else {
                return;
            }
        }
    }
    //更新元素
    void updatePosition(const cocos2d::Vec2& newPos) {
        this->setPosition(newPos);
        x = newPos.x;
        y = newPos.y;
    }
    //获取网格坐标
    float getXX() {
        Vec2 you = ConvertTest::myConvertLocalToGrid(Vec2(x, y));
        return you.x;
    }
    float getYY() {
        Vec2 you = ConvertTest::myConvertLocalToGrid(Vec2(x, y));
        return you.y;
    }
    //获取世界坐标
    float getX() {
        return x;
    }
    float getY() {
        return y;
    }
    float getSize() {
        return size;
    }
    int getLv() {
        return level;
    }
    int getHp() {
        return _hp;
    }

    // 新增：库存相关接口
    int getCurrentStock() const { return currentSize; }   // 获取当前库存
    int getMaxStock() const { return maxSize; }           // 获取库存上限

    // 设置/获取位置（复用Node的position，锚点内部管理）
    void setMinePosition(const cocos2d::Vec2& pos) { this->setPosition(pos); }
    cocos2d::Vec2 getMinePosition() const { return this->getPosition(); }

    // 获取精灵（仅用于特殊操作，尽量封装在类内）
    cocos2d::Sprite* getSprite() const { return _sprite; }

    // 视觉反馈：闪烁（成功放置）
    void playSuccessBlink() { this->runAction(cocos2d::Blink::create(1.0f, 2)); }
    // 视觉反馈：红色闪烁+销毁（失败放置）
    void playFailBlinkAndRemove();
    // 视觉反馈：红色闪烁
    void playFailBlink();

    float getSpeed() const { return _generateSpeed; }
};

#endif