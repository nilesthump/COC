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

bool UnitState::IsDefense() const
{
    return base_data_ptr_ && base_data_ptr_->IsDefender();
}

double UnitState::GetPositionX() const
{
    return position_X_;
}

double UnitState::GetPositionY() const
{
    return position_Y_;
}

void UnitState::SetPosition(double x, double y)
{
    position_X_ = x;
    position_Y_ = y;
}

double UnitState::GetCurrentHealth() const
{
    return current_health_;
}

double UnitState::GetHealthPercent() const
{
    if (!base_data_ptr_ || base_data_ptr_->health <= 0)
        return 0.0;

    return current_health_ / base_data_ptr_->health;
}

void UnitState::TakeDamage(double damage)
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

void UnitState::UpdateCoolDowns(double dt)
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

double UnitState::GetAttackCooldown() const
{
    return attack_cool_down_;
}

double UnitState::GetMoveSpeed() const
{
    return base_data_ptr_ ? base_data_ptr_->move_speed : 0.0;
}

double UnitState::GetAttackDistance() const
{
    return base_data_ptr_ ? base_data_ptr_->attack_distance : 0.0;
}

double UnitState::GetDamage() const
{
    return base_data_ptr_ ? static_cast<double>(base_data_ptr_->damage) : 0.0;
}

double UnitState::GetAttackInterval() const
{
    return base_data_ptr_ ? base_data_ptr_->attack_interval : 1.0;
}

double UnitState::GetMaxHealth() const
{
    return base_data_ptr_ ? static_cast<double>(base_data_ptr_->health) : 0.0;
}

UnitType UnitState::GetTargetType() const
{
    return base_data_ptr_ ? base_data_ptr_->unit_type : UnitType::GROUND;
}

AttackType UnitState::GetAttackType() const
{
    return base_data_ptr_ ? base_data_ptr_->attack_type : AttackType::SINGLE_TARGET;
}

CombatType UnitState::GetCombatType() const
{
    return base_data_ptr_ ? base_data_ptr_->combat_type : CombatType::MELEE;
}
