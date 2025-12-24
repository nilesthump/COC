#include "UnitState.h"
#include "AttackerData.h"
#include "DefenderData.h"
#include <string>

UnitState::UnitState()
    : current_health_(0.0)
    , position_X_(0.0)
    , position_Y_(0.0)
    , attack_cool_down_(0.0)
    , is_alive_(false){}

bool UnitState::IsAttacker() const
{
    return base_data_ptr_ && base_data_ptr_->IsAttacker();
}

bool UnitState::IsDefender() const
{
    return base_data_ptr_ && base_data_ptr_->IsDefender();
}

float UnitState::GetPositionX() const
{
    return position_X_;
}

float UnitState::GetPositionY() const
{
    return position_Y_;
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
    if (!base_data_ptr_ || base_data_ptr_->health <= 0)
        return 0.0;

    return current_health_ / base_data_ptr_->health;
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
    return current_health_ > 0;
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
    return attack_cool_down_ <= 0;
}

void UnitState::ResetAttackCooldown()
{
    if (base_data_ptr_)
    {
        attack_cool_down_ = base_data_ptr_->attack_interval;
    }
}

float UnitState::GetAttackCooldown() const
{
    return attack_cool_down_;
}

float UnitState::GetMoveSpeed() const
{
    return base_data_ptr_ ? base_data_ptr_->move_speed : 0.0;
}

float UnitState::GetAttackDistance() const
{
    return base_data_ptr_ ? base_data_ptr_->attack_distance : 0.0;
}

float UnitState::GetDamage() const
{
    return base_data_ptr_ ?(base_data_ptr_->damage) : 0.0;
}

float UnitState::GetAttackInterval() const
{
    return base_data_ptr_ ? base_data_ptr_->attack_interval : 1.0;
}

float UnitState::GetMaxHealth() const
{
    return base_data_ptr_ ? (base_data_ptr_->health) : 0.0;
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

int UnitState::GetTileWidth() const
{
    return base_data_ptr_ ? base_data_ptr_->GetTileWidth() : 1;
}

int UnitState::GetTileHeight() const
{
    return base_data_ptr_ ? base_data_ptr_->GetTileHeight() : 1;
}

TargetPriority UnitState::GetPreferredTarget() const
{
    return base_data_ptr_ ? base_data_ptr_->GetPreferredTarget() : TargetPriority::ANY;
}

bool UnitState::IsResourceBuilding()const
{
    return base_data_ptr_ ? base_data_ptr_->IsResourceBuilding() : false;
}