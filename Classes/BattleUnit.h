/*
 * BattleUnit.h - 战斗单位实体类
 *
 * 核心职责：战斗系统的实体容器，协调各个组件的工作
 * 设计模式：组合模式 + 策略模式
 *
 * 1. 这是战斗系统的最小可操作单元
 * 2. 每个BattleUnit代表战场上的一个独立实体（如一个野蛮人、一个箭塔）
 * 3. 采用组合模式，不继承具体功能，而是持有功能组件
 * 
 * 生命周期：
 * 1. 创建：UnitFactory根据配置创建
 * 2. 初始化：设置初始位置、血量等状态
 * 3. 战斗：每帧Update，协调各组件工作
 * 4. 销毁：死亡或战斗结束后删除
 * 
 * 12/16
 * Update:BattleUnit = UnitState + UnitBehavior + UnitNavigation + VisualComponet
 * BattleUnit作为一个战斗单位，血条/音效/图片/死亡效果都跟着角色走
 * 而且visual部分可以根据实际快速更新，注意和战斗逻辑分离即可
 */

#ifndef BATTLEUNIT_H
#define BATTLEUNIT_H
#include "cocos2d.h"
#include "UnitState.h"
#include "UnitBehavior.h"
#include "UnitNavigation.h"
#include "ConvertTest.h"
#include <vector>
#include <string>
#include <functional>

class BattleUnit
{
private:
	//unit本身核心组件
	UnitState state_;
	std::unique_ptr<UnitBehavior> behavior_;    
	std::unique_ptr<UnitNavigation> navigation_;

	//战斗状态
	bool in_battle_ = false;
	BattleUnit* target_;

	//视觉组件
	cocos2d::Sprite* unit_sprite_;
	cocos2d::Sprite* health_bar_bg_;
	cocos2d::Sprite* background_sprite_;
	cocos2d::ProgressTimer* health_bar_;
	cocos2d::Node* parent_node_;

	//音效组件
	std::string attack_sound_;
	std::string death_sound_;

public:
	BattleUnit();
	~BattleUnit();

	//禁止拷贝（避免浅拷贝问题）
	BattleUnit(const BattleUnit&) = delete;
	BattleUnit& operator=(const BattleUnit&) = delete;

	//初始化
	template<typename T>
	void Init(const T&data)
	{
		state_.Init(data);
	}

	//设置组件
	void SetBehavior(std::unique_ptr<UnitBehavior> behavior);
	void SetNavigation(std::unique_ptr<UnitNavigation> navigation);
	void SetBackgroundSprite(cocos2d::Sprite* background_sprite);

	//更新函数
	void Update(float deltaTime, std::vector<BattleUnit*>& enemies);

	//受到伤害
	void TakeDamage(float damage, BattleUnit* source);

	//Getter接口
	UnitState& GetState();
	const UnitState& GetState() const;

	bool IsAlive() const;
	float GetPositionX() const;
	float GetPositionY() const;
	void SetPosition(float x, float y);

	float GetHealthPercent() const;
	float GetCurrentHealth() const;
	float GetMaxHealth()const;
	float GetMoveSpeed() const;
	float GetAttackDistance() const;
	float GetDamage() const;
	float GetAttackInterval() const;

	UnitType GetTargetType() const;
	AttackType GetAttackType() const;
	CombatType GetCombatType() const;
	BattleUnit* GetTarget() const;
	UnitNavigation* GetNavigation() const;
	UnitBehavior* GetBehavior() const;

	//获取parent
	cocos2d::Node* GetParentNode() const;

	//视觉方法
	void SetSprite(cocos2d::Sprite* sprite, cocos2d::Node* parent);
	cocos2d::Sprite* GetSprite() const;
	void SetupHealthBar(cocos2d::Node* parent);
	void UpdateSpritePosition();
	void UpdateHealthBar();
	void RemoveSprite();

	// 反馈动画
	void PlayPlacementFailAnimation();

	//音效方法
	void SetAttackSound(const std::string& sound);
	void SetDeathSound(const std::string& sound);
	void PlayAttackSound();
	void PlayDeathSound();

	void OnBattleStart();
	bool IsInBattle() const { return in_battle_; }

};

#endif