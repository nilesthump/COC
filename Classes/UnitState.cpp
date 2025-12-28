#include "UnitState.h"
#include "AttackerData.h"
#include "DefenderData.h"
#include <string>

bool UnitState::IsAttacker() const
{
	return is_attacker_;
}

float UnitState::GetPositionX() const
{
	return position_X_;
}

float UnitState::GetPositionY() const
{
	return position_Y_;
}

cocos2d::Vec2 UnitState::GetLogicalPosition()const
{
	return { position_X_, position_Y_ };
}

void UnitState::SetPosition(float x, float y)
{
	position_X_ = x;
	position_Y_ = y;
}

float UnitState::GetCurrentHealth() const
{
	return current_health_;
}

float UnitState::GetHealthPercent() const
{
	return max_health_ > 0 ? current_health_ / max_health_ : 0;
}

void UnitState::TakeDamage(float damage)
{
	current_health_ -= damage;
	if (current_health_ <= 0)
	{
		current_health_ = 0.0;
		is_alive_ = false;
	}
}

bool UnitState::IsAlive() const
{
	return is_alive_;
}

void UnitState::UpdateCoolDowns(float dt)
{
	if (attack_cool_down_ > 0)
		attack_cool_down_ -= dt;

	if (attack_cool_down_ < 0)
		attack_cool_down_ = 0;
}

bool UnitState::CanAttack() const
{
	return is_alive_ && attack_cool_down_ <= 0;
}

void UnitState::ResetAttackCooldown()
{
	attack_cool_down_ = GetAttackInterval();
}

void UnitState::ConsumeResources(float gold, float elixir)
{
	current_gold_inventory_ -= gold;
	current_elixir_inventory_ -= elixir;
	if (current_gold_inventory_ < 0) current_gold_inventory_ = 0;
	if (current_elixir_inventory_ < 0) current_elixir_inventory_ = 0;
}

float UnitState::GetAttackCooldown() const
{
	return attack_cool_down_;
}

float UnitState::GetMoveSpeed() const
{
	auto a = dynamic_cast<const AttackerData*>(base_data_ptr_.get());
	return a ? a->move_speed : 0.0f;
}

float UnitState::GetAttackDistance() const
{
	return base_data_ptr_ ? base_data_ptr_->attack_distance : 1.0f;
}

float UnitState::GetDamage() const
{
	return base_data_ptr_ ? (base_data_ptr_->damage) : 0.0;
}

float UnitState::GetAttackInterval() const
{
	return base_data_ptr_ ? base_data_ptr_->attack_interval : 1.0f;
}

float UnitState::GetMaxHealth() const
{
	return max_health_;
}

float UnitState::GetSearchRange() const
{
	return search_range_;
}

UnitType UnitState::GetUnitType() const
{
	if (!base_data_ptr_)
	{
		CCLOG("--- [UNIT ERROR] --- base_data_ptr_ is NULL!");
		return UnitType::NONE;
	}

	// 1. 尝试转换
	auto a = dynamic_cast<const AttackerData*>(base_data_ptr_.get());

	// 2. 如果失败，通过 RTTI 打印底层真实的类名
	if (!a)
	{
		// typeid(xxx).name() 可以直接告诉你底层到底是哪个类
		const char* realClassName = typeid(*base_data_ptr_.get()).name();

		CCLOG("--- [CAST FAILED] --- Name: %s | Real Class: %s",
			base_data_ptr_->name.c_str(),
			realClassName);

		// 顺便检查一下是不是被错当成了防御建筑数据
		auto d = dynamic_cast<const DefenderData*>(base_data_ptr_.get());
		if (d)
		{
			CCLOG("--- [INFO] --- This unit is actually using DefenderData!");
		}
	}
	else
	{
		// 如果成功，打印一次确认，防止是枚举值赋值错了
		CCLOG("--- [CAST SUCCESS] --- Unit: %s | TypeID: %d", base_data_ptr_->name.c_str(), (int)a->unit_type);
	}

	return a ? a->unit_type : UnitType::NONE;
}

BuildingType UnitState::GetBuildingType()const
{
	auto d = dynamic_cast<const DefenderData*>(base_data_ptr_.get());
	return d ? d->building_type : BuildingType::NONE;
}

AttackTargetType UnitState::GetAttackTargetType() const
{
	return base_data_ptr_ ? base_data_ptr_->attack_target_type : AttackTargetType::GROUND;
}

UnitTargetType UnitState::GetUnitTargetType() const
{
	return base_data_ptr_ ? base_data_ptr_->unit_target_type : UnitTargetType::GROUND;
}

AttackType UnitState::GetAttackType() const
{
	return base_data_ptr_ ? base_data_ptr_->attack_type : AttackType::SINGLE_TARGET;
}

CombatType UnitState::GetCombatType() const
{
	return base_data_ptr_ ? base_data_ptr_->combat_type : CombatType::MELEE;
}

TargetPriority UnitState::GetTargetPriority()const
{
	auto a = dynamic_cast<const AttackerData*>(base_data_ptr_.get());
	return a ? a->target_priority : TargetPriority::ANY;
}

int UnitState::GetTileWidth() const
{
	auto d = dynamic_cast<const DefenderData*>(base_data_ptr_.get());
	return d ? d->tile_width : 1;
}

int UnitState::GetTileHeight() const
{
	auto d = dynamic_cast<const DefenderData*>(base_data_ptr_.get());
	return d ? d->tile_height : 1;
}

int UnitState::GetTotalGoldStatic() const
{
	auto d = dynamic_cast<const DefenderData*>(base_data_ptr_.get());
	return d ? d->gold_reward : 0;
}

int UnitState::GetTotalElixirStatic() const
{
	auto d = dynamic_cast<const DefenderData*>(base_data_ptr_.get());
	return d ? d->elixir_reward : 0;
}

int UnitState::GetCurrentGoldInventory() const
{
	return static_cast<int>(current_gold_inventory_);
}

int UnitState::GetCurrentElixirInventory() const
{
	return static_cast<int>(current_elixir_inventory_);
}

bool UnitState::IsResourceBuilding()const
{
	auto d = dynamic_cast<const DefenderData*>(base_data_ptr_.get());
	return d ? d->is_resource_building : false;
}

bool UnitState::IsDefenderBuilding()const
{
	auto d = dynamic_cast<const DefenderData*>(base_data_ptr_.get());
	return d ? d->is_defender_building : false;
}

bool UnitState::IsWall()const
{
	return GetBuildingType() == BuildingType::WALL;
}

bool UnitState::IsTownHall()const
{
	return GetBuildingType() == BuildingType::TOWN_HALL;
}