/*
 * UnitState.h - 单位状态数据类
 * 核心职责：战斗单位的实时状态数据容器（纯数据类）
 * 设计模式：数据容器模式 + 值对象模式
 *
 * 设计原则：
 * 1. 纯数据类：只包含数据，不包含行为
 * 2. 值语义：可以被复制，没有多态性
 *
 * 数据流：
 * 静态配置 → 动态状态 → 实时显示
 *   (Data)    (State)     (UI)
 * 
 * 关注：
 * 1.UnitState提供了部分接口来获取静态数据，原因是在实际战斗中，Unit个体的寻路、
 *	 战斗表现需要关注静态数据的移动速度等，故提供部分接口。但是该接口传给BattleUnit进行二层传递
 *   最终unit寻路攻击表现可以直接在BattleUnit中调用得到静态数据
 */
#ifndef UNITSTATE_H
#define UNITSTATE_H
#include "BaseUnitData.h"
#include "cocos2d.h"
#include <memory>

class UnitState
{
private:
	std::unique_ptr<BaseUnitData> base_data_ptr_;

	// --- 实时动态状态 (战斗中会改变的量) ---
	float current_health_;
	float max_health_;          // 缓存最大血量，方便 UI 计算比例
	float position_X_, position_Y_;
	float attack_cool_down_;
	bool is_alive_;
	float search_range_;

	// 动态资源库存：重点！
	// 战斗开始时从 Data 拷贝初始值，之后随受击减少
	float current_gold_inventory_ = 0;
	float current_elixir_inventory_ = 0;

	bool is_attacker_ = false;

public:
	//构造函数
	UnitState():
		current_health_(0), max_health_(0),
		position_X_(0), position_Y_(0),
		attack_cool_down_(0), is_alive_(false),
		search_range_(0)
	{}
	~UnitState() = default;
	UnitState(const UnitState&) = delete;
	UnitState& operator=(const UnitState&) = delete;
	UnitState(UnitState&&) = default;
	UnitState& operator=(UnitState&&) = default;

	//初始化接口
	//模板化的init函数，可以接受任何派生自BaseUnitData的类型
	template<typename T>
	void init(const T& data)
	{
		// 1. 深度拷贝静态配置
		base_data_ptr_ = std::make_unique<T>(data);

		// 2. 初始化动态数值快照
		current_health_ = data.health;
		max_health_ = current_health_;
		search_range_ = data.search_range;
		is_alive_ = true;
		attack_cool_down_ = 0;
		position_X_ = 0;
		position_Y_ = 0;

		// 3. 区分处理建筑与角色
		if constexpr (std::is_same_v<T, DefenderData>)
		{
			is_attacker_ = false;
			// 提取建筑初始资源
			current_gold_inventory_ = static_cast<float>(data.gold_reward);
			current_elixir_inventory_ = static_cast<float>(data.elixir_reward);
		}
		else
		{
			is_attacker_ = true;
			current_gold_inventory_ = 0;
			current_elixir_inventory_ = 0;
		}
	}
	
	//类型判断
	bool IsAttacker() const;
	bool IsResourceBuilding()const;
	bool IsDefenderBuilding()const;
	bool IsWall()const;
	bool IsTownHall()const;

	//资源
	void ConsumeResources(float gold, float elixir);

	//位置
	float GetPositionX() const;
	float GetPositionY() const;
	cocos2d::Vec2 GetLogicalPosition()const;
	void SetPosition(float x, float y);

	//血量
	float GetCurrentHealth() const;
	float GetHealthPercent() const;
	void TakeDamage(float damage);
	bool IsAlive() const;

	//冷却管理部分
	void UpdateCoolDowns(float dt);
	bool CanAttack() const;
	void ResetAttackCooldown();
	float GetAttackCooldown() const;

	//部分静态数据访问
	const BaseUnitData* GetBaseData() const;
	float GetMoveSpeed() const;
	float GetAttackDistance() const;
	float GetDamage() const;
	float GetAttackInterval() const;
	float GetMaxHealth() const;
	float GetSearchRange()const;
	UnitType GetUnitType()const;					//直接获取自己是谁(角色）
	BuildingType GetBuildingType()const;					//直接获取自己是谁(建筑）
	AttackTargetType GetAttackTargetType() const;   //攻击目标类型（地面/空中/两者）
	UnitTargetType GetUnitTargetType()const;		//获取自身类型（空中/地面）
	AttackType GetAttackType() const;				//攻击类型（单体/范围/连锁）
	CombatType GetCombatType() const;				//近战/远程
	TargetPriority GetTargetPriority() const;		//偏好
	int GetTileWidth()const;
	int GetTileHeight()const;
	int GetTotalGoldStatic() const;
	int GetTotalElixirStatic() const;
	int GetCurrentGoldInventory() const;
	int GetCurrentElixirInventory() const;
};

#endif