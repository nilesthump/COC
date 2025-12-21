#ifndef _BUILDING_
#define _BUILDING_
#include "cocos2d.h"

class GoldMine : public cocos2d::Node {
private:
    cocos2d::Vec2 _position;  // 坐标（不过Node本身有position，这里可以复用或统一）
    int Hp;                   // 血量
    int X, Y;                 // 网格坐标
    int level;                // 等级
    float goldGenerateSpeed;  // 金币产生速度
    // 新增：图像精灵（承载金矿的显示）
    cocos2d::Sprite* _mineSprite;

    // 内部初始化精灵的函数
    bool initSprite(const std::string& texturePath);

public:
    // Cocos标准创建函数（传入你的参数）
    static GoldMine* create(int gridX, int gridY, int initHp, int initLevel, float initGoldSpeed, const std::string& texturePath);
    // 初始化函数
    virtual bool init(int gridX, int gridY, int initHp, int initLevel, float initGoldSpeed, const std::string& texturePath);

    // --- 功能接口 ---
    // 血量操作
    int getHp() const { return Hp; }
    void takeDamage(int damage);  // 扣血（血量为0时隐藏）
    // 等级/产金速度
    int getLevel() const { return level; }
    void upgradeLevel();          // 升级（提升产金速度）
    float getGoldSpeed() const { return goldGenerateSpeed; }
    // 产金计算
    int generateGold(float time) const { return static_cast<int>(goldGenerateSpeed * time); }
    // 网格坐标
    cocos2d::Vec2 getGridPos() const { return cocos2d::Vec2(X, Y); }
    // 获取精灵（方便外部修改图像）
    cocos2d::Sprite* getSprite() const { return _mineSprite; }
};

#endif