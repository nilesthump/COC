#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "cocos2d.h"

class Building : public cocos2d::Node {
protected:
    // 核心属性
    int _hp;                  // 血量
    int level;
    float _generateSpeed;     // 生产速度
    std::string _textureName; // 纹理名称（用于区分不同等级圣水收集器）
    cocos2d::Sprite* _sprite; // 图像精灵
    float x, y;
    float size = 3.0f;
    // 初始化精灵（内部调用）
    virtual bool initSprite(const std::string& textureName) = 0;
public:
    
    // 初始化函数
    virtual bool init(const std::string& textureName, int hp, int lv, float generateSpeed, float x0, float y0)=0;

    // --- 对外接口 ---
    void changeX(float t) {
        x = t;
    }
    void changeY(float t) {
        y = t;
    }
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

    void updatePosition(const cocos2d::Vec2& newPos) {
        this->setPosition(newPos);
        x = newPos.x;
        y = newPos.y;
    }

    // 设置/获取位置（复用Node的position，锚点内部管理）
    void setMinePosition(const cocos2d::Vec2& pos) { this->setPosition(pos); }
    cocos2d::Vec2 getMinePosition() const { return this->getPosition(); }

    // 获取精灵（仅用于特殊操作，尽量封装在类内）
    cocos2d::Sprite* getSprite() const { return _sprite; }

    // 视觉反馈：闪烁（成功放置）
    void playSuccessBlink() { this->runAction(cocos2d::Blink::create(1.0f, 3)); }
    // 视觉反馈：红色闪烁+销毁（失败放置）
    void playFailBlinkAndRemove();

    float getSpeed() const { return _generateSpeed; }
};

#endif