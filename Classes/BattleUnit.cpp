#include "BattleUnit.h"

BattleUnit::BattleUnit() : target_(nullptr) {}

void BattleUnit::SetBehavior(UnitBehavior* behavior)
{
	behavior_ = behavior;
}

void BattleUnit::SetNavigation(UnitNavigation* navigation)
{
	navigation_ = navigation;
}

void BattleUnit::Update(double deltaTime, std::vector<BattleUnit*>& enemies)
{
	if (!state_.IsAlive())
		return;

	//1.行为更新
	if (behavior_)
	{
		behavior_->OnUpdate(this, deltaTime);
	}

	//2.寻找目标
	if (navigation_ && (!target_ || !target_->IsAlive()))
	{
		target_ = navigation_->FindTarget(this, enemies);
	}

	//3.移动
	if (navigation_ && target_ && !navigation_->IsInAttackRange(this, target_))
	{
		navigation_->CalculateMove(this, target_, deltaTime);
	}

	//4.攻击
	if (behavior_ && target_ && navigation_ &&
		navigation_->IsInAttackRange(this, target_) &&
		state_.CanAttack())
	{
		if (behavior_->CanAttack(this, target_))
		{
			double damage = behavior_->CalculateDamage(this, target_);
			target_->TakeDamage(damage, this);
			behavior_->OnAttack(this, target_);
			state_.ResetAttackCooldown();
		}
	}

	//5.更新状态
	state_.UpdateCoolDowns(deltaTime);
}

void BattleUnit::TakeDamage(double damage, BattleUnit* source)
{
	state_.TakeDamage(damage);
	if (behavior_)
	{
		behavior_->OnDamageTaken(this, damage, source);
		if (!state_.IsAlive())
		{
			behavior_->OnDeath(this);
		}
	}
}

//Getter接口实现
UnitState& BattleUnit::GetState()
{
	return state_;
}

const UnitState& BattleUnit::GetState() const
{
	return state_;
}

bool BattleUnit::IsAlive() const
{
	return state_.IsAlive();
}

double BattleUnit::GetPositionX() const
{
	return state_.GetPositionX();
}

double BattleUnit::GetPositionY() const
{
	return state_.GetPositionY();
}

void BattleUnit::SetPosition(double x, double y)
{
	state_.SetPosition(x, y);
}

double BattleUnit::GetHealthPercent() const
{
	return state_.GetHealthPercent();
}

double BattleUnit::GetCurrentHealth() const
{
	return state_.GetCurrentHealth();
}

double BattleUnit::GetMoveSpeed() const
{
	return state_.GetMoveSpeed();
}

double BattleUnit::GetAttackDistance() const
{
	return state_.GetAttackDistance();
}

double BattleUnit::GetDamage() const
{
	return state_.GetDamage();
}

double BattleUnit::GetAttackInterval() const
{
	return state_.GetAttackInterval();
}

double BattleUnit::GetMaxHealth() const
{
	return state_.GetMaxHealth();
}

UnitType BattleUnit::GetTargetType() const
{
	return state_.GetTargetType();
}

AttackType BattleUnit::GetAttackType() const
{
	return state_.GetAttackType();
}

CombatType BattleUnit::GetCombatType() const
{
	return state_.GetCombatType();
}

BattleUnit* BattleUnit::GetTarget() const
{
	return target_;
}

bool BattleUnit::IsResourceBuilding() const
{
	//暂时返回 false，未来可以根据 AttackerData 判断
	return false;
}
