#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "cocos2d.h"
#include"ConvertTest.h"

class Building : public cocos2d::Node {
protected:
	// 公共核心属性
	int _hp;                  // 血量
	int level;                // 等级
	std::string _textureName; // 纹理名称
	cocos2d::Sprite* _sprite; // 图像精灵
	float x, y;               //世界坐标
	int size;        //尺寸
	//升级相关
	bool isUpgrade = false;
	// 初始化精灵（内部调用）
	virtual bool initSprite(const std::string& textureName) = 0;
public:
	// 初始化函数
	virtual bool init(const std::string& textureName, int hp, int lv, float x0, float y0) = 0;
	//修改Building坐标
	void changeX(float t) {
		x = t;
	}
	void changeY(float t) {
		y = t;
	}
	//建造相关
	virtual int getGoldCost() const { return 0; }
	virtual int getElixirCost() const { return 0; }
	//升级相关
	virtual bool canUpgrade() { return 0; }
	virtual int getUpgradeGoldCost() const{ return 0; }
	virtual int getUpgradeElixirCost()const { return 0; }
	virtual void update() { return; }
	virtual void cutTime() { return; }
	virtual int getRemainTime() { return 0; }
	virtual void finishUpgrade() {return;}
	bool getIsUpgrade() {
		return isUpgrade;
	}
	void startUpgrade() {
		isUpgrade = true;
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
	//更新
	void updatePosition(const cocos2d::Vec2& newPos) {
		this->setPosition(newPos);
		x = newPos.x;
		y = newPos.y;
	}
	virtual void updateCurrentStock(int n) { return; }
	//接口
	float getX() {
		return x;
	}
	float getY() {
		return y;
	}
	int getSize() {
		return size;
	}
	int getLv() {
		return level;
	}
	int getHp() {
		return _hp;
	}
	float getXX() {
		Vec2 you = ConvertTest::myConvertLocalToGrid(Vec2(x, y));
		return you.x;
	}
	float getYY() {
		Vec2 you = ConvertTest::myConvertLocalToGrid(Vec2(x, y));
		return you.y;
	}
	//兵营相关
	virtual int getArmy(int i)const { return 0; }
	virtual int getArmySize(int i)const { return 0; }
	virtual void updateNum(int i){ return; }
	//大本营相关
	virtual int getMaxGoldNum() {return 0;}
	virtual int getMaxElixirNum() {return 0 ;}
	//获取最大容量和现容量
	virtual int getSpeed() const { return 1; }
	virtual int getCurrentStock() const { return 0; }
	virtual void clearCurrentStock() { return; }
	virtual int getMaxStock() const { return 0; }  
	virtual void addCurrent(int n) { return; }
	// 设置/获取位置（复用Node的position，锚点内部管理）
	void setMinePosition(const cocos2d::Vec2& pos) { this->setPosition(pos); }
	cocos2d::Vec2 getMinePosition() const { return this->getPosition(); }
	// 获取精灵（仅用于特殊操作，尽量封装在类内）
	cocos2d::Sprite* getSprite() const { return _sprite; }


	// 视觉反馈：闪烁（成功放置）
	void playSuccessBlink() { this->runAction(cocos2d::Blink::create(0.5f, 2)); }
	// 视觉反馈：红色闪烁+销毁（失败放置）
	void playFailBlinkAndRemove();
	// 视觉反馈：红色闪烁
	void playFailBlink();

	// 获取建筑类型名称（用于持久化存储）
	virtual std::string getBuildingType() const { return "Building"; }
};

#endif