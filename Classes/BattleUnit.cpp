#include "BattleUnit.h"
#include "cocos2d.h"

using namespace cocos2d;

BattleUnit::BattleUnit()
	: target_(nullptr),
	unit_sprite_(nullptr),
	health_bar_bg_(nullptr),
	health_bar_(nullptr),
	parent_node_(nullptr)
{}

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
			//播放攻击音效
			PlayAttackSound();
		}
	}

	//5.更新状态
	state_.UpdateCoolDowns(deltaTime);

	//6.更新视觉表现
	UpdateSpritePosition();
	UpdateHealthBar();
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
			// 播放死亡音效
			PlayDeathSound();
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
	// 同时更新精灵位置
	UpdateSpritePosition();
	UpdateHealthBar();
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

UnitNavigation* BattleUnit::GetNavigation() const
{
	return navigation_;
}

UnitBehavior* BattleUnit::GetBehavior() const
{
	return behavior_;
}

//视觉组件设置
void BattleUnit::SetSprite(cocos2d::Sprite* sprite, cocos2d::Node* parent)
{
	if (unit_sprite_ && unit_sprite_->getParent())
	{
		unit_sprite_->removeFromParent();
	}

	unit_sprite_ = sprite;

	if (unit_sprite_ && parent)
	{
		// 检查精灵是否已经有父节点
		if (unit_sprite_->getParent())
		{
			unit_sprite_->removeFromParent();
		}
		parent->addChild(unit_sprite_);
		unit_sprite_->setPosition(
			Vec2(state_.GetPositionX(), state_.GetPositionY()));
	}
}

Sprite* BattleUnit::GetSprite() const
{
	return unit_sprite_;
}

void BattleUnit::SetupHealthBar(Node* parent)
{
	parent_node_ = parent;

	// 先移除旧的健康条（如果存在）
	if (health_bar_bg_ && health_bar_bg_->getParent())
	{
		health_bar_bg_->removeFromParent();
		health_bar_bg_ = nullptr;
	}

	if (health_bar_ && health_bar_->getParent())
	{
		health_bar_->removeFromParent();
		health_bar_ = nullptr;
	}
	// 创建血条背景
	health_bar_bg_ = Sprite::create("btn_disabled.png");
	if (health_bar_bg_)
	{
		health_bar_bg_->setScale(0.5f);
		parent_node_->addChild(health_bar_bg_, 10); // 设置较高的Z轴顺序
	}

	// 创建血条
	Sprite* health_sprite = Sprite::create("btn_normal.png");
	if (health_sprite)
	{
		health_bar_ = ProgressTimer::create(health_sprite);
		health_bar_->setType(ProgressTimer::Type::BAR);
		health_bar_->setMidpoint(Vec2(0, 0.5f));
		health_bar_->setBarChangeRate(Vec2(1, 0));
		health_bar_->setScale(0.5f);
		health_bar_->setPercentage(100.0f);
		parent_node_->addChild(health_bar_, 11); // 设置更高的Z轴顺序
	}
}

void BattleUnit::UpdateSpritePosition()
{
	if (unit_sprite_)
	{
		unit_sprite_->setPosition(Vec2(state_.GetPositionX(), state_.GetPositionY()));
	}
}

void BattleUnit::UpdateHealthBar()
{
	if (!state_.IsAlive())
	{
		// 单位死亡时隐藏血条
		if (health_bar_bg_)
			health_bar_bg_->setVisible(false);
		if (health_bar_)
			health_bar_->setVisible(false);
		return;
	}

	double healthPercent = GetHealthPercent() * 100.0f;

	// 更新血条位置
	Vec2 unitPos = Vec2(state_.GetPositionX(), state_.GetPositionY());
	if (health_bar_bg_)
	{
		health_bar_bg_->setPosition(unitPos + Vec2(0, 30));
	}
	if (health_bar_)
	{
		health_bar_->setPosition(unitPos + Vec2(0, 30));
		health_bar_->setPercentage(healthPercent);
	}
}

void BattleUnit::RemoveSprite()
{
	if (unit_sprite_ && unit_sprite_->getParent())
	{
		unit_sprite_->removeFromParent();
		unit_sprite_ = nullptr;
	}

	if (health_bar_bg_ && health_bar_bg_->getParent())
	{
		health_bar_bg_->removeFromParent();
		health_bar_bg_ = nullptr;
	}

	if (health_bar_ && health_bar_->getParent())
	{
		health_bar_->removeFromParent();
		health_bar_ = nullptr;
	}
}

//音效组件设置
void BattleUnit::SetAttackSound(const std::string& sound)
{
	attack_sound_ = sound;
}

void BattleUnit::SetDeathSound(const std::string& sound)
{
	death_sound_ = sound;
}

void BattleUnit::PlayAttackSound()
{
	if (!attack_sound_.empty())
	{
		// 播放攻击音效
		// SimpleAudioEngine::getInstance()->playEffect(attack_sound_.c_str());
	}
}

void BattleUnit::PlayDeathSound()
{
	if (!death_sound_.empty())
	{
		// 播放死亡音效
		// SimpleAudioEngine::getInstance()->playEffect(death_sound_.c_str());
	}
}

Node* BattleUnit::GetParentNode() const
{
	return parent_node_;
}
