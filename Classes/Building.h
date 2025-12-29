#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "cocos2d.h"
#include"ConvertTest.h"
#include "SessionManager.h"

class Building : public cocos2d::Node {
protected:
	// 公共核心属性
	int hp;                  // 血量
	int level;                // 等级
	std::string texture_name; // 纹理名称
	cocos2d::Sprite* building_sprite; // 图像精灵
	float x, y;               //世界坐标
	int size;        //尺寸
	//升级相关
	bool is_upgrade = false;
	// 初始化精灵（内部调用）
	virtual bool InitSprite(const std::string& textureName) = 0;
public:
	// 初始化函数
	virtual bool init(const std::string& textureName, int original_hp, int lv, float x0, float y0) = 0;
	//建造相关
	virtual int GetGoldCost() const { return 0; }
	virtual int GetElixirCost() const { return 0; }
	//升级相关
	virtual bool CanUpgrade() { return 0; }
	virtual int GetUpgradeGoldCost() const{ return 0; }
	virtual int GetUpgradeElixirCost()const { return 0; }
	virtual void update() { return; }
	virtual void CutTime() { return; }
	virtual int GetRemainTime() {
		if (!is_upgrade) return 0;

		UpgradeData upgradeData;
		if (SessionManager::getInstance()->getUpgradeDataForBuilding(x, y, upgradeData)) {
			return upgradeData.remainingTime;
		}

		return 0;
	}
	virtual int GetUpgradeDuration() const { return 0; }
	virtual void FinishUpgrade() { return; }
	bool GetIsUpgrade() {
		return is_upgrade;
	}
	void SetIsUpgrade(bool upgrade) {
		is_upgrade = upgrade;
	}
	void StartUpgrade() {
		is_upgrade = true;
	}
	void UpdateTexture(const std::string& newTextureName) {
		if (building_sprite) {
			// 尝试加载新纹理
			auto newTexture = Director::getInstance()->getTextureCache()->addImage(newTextureName);
			if (newTexture) {
				building_sprite->setTexture(newTexture);
			}
			else {
				return;
			}
		}
	}
	//更新
	void UpdatePosition(const cocos2d::Vec2& newPos) {
		this->setPosition(newPos);
		x = newPos.x;
		y = newPos.y;
	}
	virtual void UpdateCurrentStock(int n) { return; }
	//接口
	float GetX() {
		return x;
	}
	float GetY() {
		return y;
	}
	int GetSize() {
		return size;
	}
	int GetLevel() {
		return level;
	}
	int GetHp() {
		return hp;
	}
	void SetHp(int original_hp) {
		hp = original_hp;
	}
	//兵营相关
	virtual int GetArmy(int i)const { return 0; }
	virtual int GetArmySize(int i)const { return 0; }
	virtual void UpdateNum(int i){ return; }
	//大本营相关
	virtual int GetMaxGoldNum() {return 0;}
	virtual int GetMaxElixirNum() {return 0 ;}
	//获取最大容量和现容量
	virtual int GetSpeed() const { return 1; }
	virtual int GetCurrentStock() const { return 0; }
	virtual void ClearCurrentStock() { return; }
	virtual int GetMaxStock() const { return 0; }  
	virtual void AddCurrent(int n) { return; }
	// 设置/获取位置
	void SetMinePosition(const cocos2d::Vec2& pos) { this->setPosition(pos); }
	//cocos2d::Vec2 GetMinePosition() const { return this->getPosition(); }
	cocos2d::Sprite* GetSprite() const { return building_sprite; }


	// 视觉反馈：闪烁（成功放置）
	void PlaySuccessBlink() { this->runAction(cocos2d::Blink::create(0.5f, 2)); }
	// 视觉反馈：红色闪烁+销毁（失败放置）
	void PlayFailBlinkAndRemove();
	// 视觉反馈：红色闪烁
	void PlayFailBlink();

	// 获取建筑类型名称（用于持久化存储）
	virtual std::string GetBuildingType() const { return "Building"; }
};

#endif