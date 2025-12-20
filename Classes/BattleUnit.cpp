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

void BattleUnit::Update(float deltaTime, std::vector<BattleUnit*>& enemies)
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
			float damage = behavior_->CalculateDamage(this, target_);
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

void BattleUnit::TakeDamage(float damage, BattleUnit* source)
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

float BattleUnit::GetPositionX() const
{
	return state_.GetPositionX();
}

float BattleUnit::GetPositionY() const
{
	return state_.GetPositionY();
}

float BattleUnit::GetHealthPercent() const
{
	return state_.GetHealthPercent();
}

float BattleUnit::GetCurrentHealth() const
{
	return state_.GetCurrentHealth();
}

float BattleUnit::GetMoveSpeed() const
{
	return state_.GetMoveSpeed();
}

float BattleUnit::GetAttackDistance() const
{
	return state_.GetAttackDistance();
}

float BattleUnit::GetDamage() const
{
	return state_.GetDamage();
}

float BattleUnit::GetAttackInterval() const
{
	return state_.GetAttackInterval();
}

float BattleUnit::GetMaxHealth() const
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
	parent_node_ = parent;
	if (unit_sprite_ && parent)
	{
		// 检查精灵是否已经有父节点
		if (unit_sprite_->getParent())
		{
			unit_sprite_->removeFromParent();
		}
		parent->addChild(unit_sprite_);
		UpdateSpritePosition();
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

void BattleUnit::UpdateSpritePosition()
{
	if (unit_sprite_ && background_sprite_)
	{
		cocos2d::Vec2 sprite_position;
		const cocos2d::Vec2 logic_position = Vec2(state_.GetPositionX(), state_.GetPositionY());
		sprite_position = ConvertTest::convertLogicToDisplay(logic_position, background_sprite_);
		unit_sprite_->setPosition(sprite_position);
	}
}

void BattleUnit::UpdateHealthBar()
{
	if (!state_.IsAlive())
	{
		if (health_bar_bg_) health_bar_bg_->setVisible(false);
		if (health_bar_) health_bar_->setVisible(false);
		return;
	}

	// 如果没有背景精灵，直接返回
	if (!background_sprite_)
		return;

	cocos2d::Vec2 unit_display_pos;
	const cocos2d::Vec2 logic_position = Vec2(state_.GetPositionX(), state_.GetPositionY());
	unit_display_pos = ConvertTest::convertLogicToDisplay(logic_position, background_sprite_);


	// 更新血条位置（在单位上方）
	Vec2 health_bar_offset(0, 30);  // 血条在单位上方30像素
	Vec2 health_bar_pos = unit_display_pos + health_bar_offset;

	if (health_bar_bg_)
	{
		health_bar_bg_->setPosition(health_bar_pos);
	}
	if (health_bar_)
	{
		health_bar_->setPosition(health_bar_pos);
		float healthPercent = GetHealthPercent() * 100.0f;
		health_bar_->setPercentage(healthPercent);
	}
}

void BattleUnit::SetPosition(float x, float y)
{
	state_.SetPosition(x, y);
	UpdateSpritePosition();  // 这里会自动进行坐标转换
	UpdateHealthBar();
}

void BattleUnit::SetBackgroundSprite(cocos2d::Sprite* background_sprite)
{
	background_sprite_ = background_sprite;
}