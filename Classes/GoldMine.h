#ifndef __GOLD_MINE_H__
#define __GOLD_MINE_H__

#include "cocos2d.h"

class GoldMine : public cocos2d::Node
{
private:
    // 核心属性
    int _hp;                  // 血量
    float _goldGenerateSpeed; // 产金速度
    std::string _textureName; // 纹理名称（用于区分不同等级金矿）
    cocos2d::Sprite* _sprite; // 图像精灵
    float x, y;
    // 初始化精灵（内部调用）
    bool initSprite(const std::string& textureName);

public:
    // 静态创建函数（Cocos推荐方式）
    static GoldMine* create(const std::string& textureName, int hp = 100, float goldSpeed = 1.0f, float x0=667.0f, float y0 = 2074.0f);
    // 初始化函数
    virtual bool init(const std::string& textureName, int hp, float goldSpeed, float x0, float y0);

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

    // 测试：获取产金速度（验证对象属性）
    float getGoldSpeed() const { return _goldGenerateSpeed; }
};

#endif // __GOLD_MINE_H__