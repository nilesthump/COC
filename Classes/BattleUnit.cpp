#include "BattleUnit.h"
#include "cocos2d.h"

using namespace cocos2d;

BattleUnit::BattleUnit()
	: target_(nullptr),
	unit_sprite_(nullptr),
	health_bar_bg_(nullptr),
	background_sprite_(nullptr),
	health_bar_(nullptr),
	parent_node_(nullptr)
{}

BattleUnit::~BattleUnit()
{
	if (building_)
	{
		if (auto root = building_->GetRootNode())
			root->removeFromParent();
	}
	//清理视觉组件
	RemoveSprite();

	target_ = nullptr;
	background_sprite_ = nullptr;
	parent_node_ = nullptr;
}

void BattleUnit::SetBehavior(std::unique_ptr<UnitBehavior> behavior)
{
	behavior_ = std::move(behavior);
}

void BattleUnit::SetNavigation(std::unique_ptr<UnitNavigation> navigation)
{
	navigation_ = std::move(navigation);  // 转移所有权
}

void BattleUnit::SetBuildingComponent(std::unique_ptr<BuildingComponent> comp)
{
	building_ = std::move(comp);
}

void BattleUnit::Update(float deltaTime, std::vector<BattleUnit*>& enemies)
{
	if (!in_battle_)
		return;
	if (!state_.IsAlive())
		return;
	
	//0.更新冷却
	state_.UpdateCoolDowns(deltaTime);

	//1.行为更新
	if (behavior_)
	{
		behavior_->OnUpdate(this, deltaTime);
	}

	//2.寻找目标
	if (navigation_ )
	{
		target_ = navigation_->FindTarget(this, enemies);
		has_target_in_range_ = false;
	}

	//3.移动
	if (state_.IsAttacker() && navigation_ && target_ && !navigation_->IsInAttackRange(this, target_))
	{
		navigation_->CalculateMove(this, target_, deltaTime);
	}

	//4.先做“是否进入攻击范围”的判定
	bool in_range = false;
	if (navigation_ && target_)
	{
		in_range = navigation_->IsInAttackRange(this, target_);
		if (in_range)
		{
			// 只要进过范围，就记住
			has_target_in_range_ = true;
		}
	}

	//5. 攻击：消耗预约
	if (behavior_ && target_ && in_range &&
		has_target_in_range_ &&
		state_.CanAttack())
	{
		if (behavior_->CanAttack(this, target_))
		{
			float damage = behavior_->CalculateDamage(this, target_);
			target_->TakeDamage(damage, this);
			behavior_->OnAttack(this, target_);
			state_.ResetAttackCooldown();
			PlayAttackSound();

			// 攻击完成，清掉预约
			has_target_in_range_ = false;
		}
	}

	// 6. 更新视觉
	if (state_.IsAttacker())
		UpdateSpritePosition();
	UpdateHealthBar();
}

//这里的takedamage组合了减伤+受伤反应（暂时没用)+死亡处理，behavior需要再考虑
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

AttackTargetType BattleUnit::GetAttackTargetType() const
{
	return state_.GetAttackTargetType();
}

UnitTargetType BattleUnit::GetUnitTargetType() const
{
	return state_.GetUnitTargetType();
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
	return navigation_.get();
}

UnitBehavior* BattleUnit::GetBehavior() const
{
	return behavior_.get();
}

//视觉组件设置
void BattleUnit::SetSprite(cocos2d::Sprite* sprite, cocos2d::Node* parent)
{
	if (unit_sprite_ && unit_sprite_->getParent())
	{
		unit_sprite_->removeFromParentAndCleanup(true);
		unit_sprite_ = nullptr;
	}

	unit_sprite_ = sprite;
	parent_node_ = parent;
	if (unit_sprite_ && parent)
	{
		// 检查精灵没经有父节点
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

//放置失败动画
void BattleUnit::PlayPlacementFailAnimation()
{
	if (!unit_sprite_)
		return;

	CCLOG("[BattleUnit] Playing placement fail animation");

	// 变红色
	unit_sprite_->setColor(Color3B::RED);

	// 闪烁3次 + 淡出
	auto blink = Blink::create(0.5f, 3);
	auto fadeOut = FadeOut::create(0.3f);

	// 动画结束后的回调
	auto cleanup = CallFunc::create([this]()
		{
			CCLOG("[BattleUnit] Fail animation completed, removing unit");
			// 从场景中移除（但不删除对象，由外部管理）
			RemoveSprite();
		});

	auto sequence = Sequence::create(blink, fadeOut, cleanup, nullptr);
	unit_sprite_->runAction(sequence);

	// 同时隐藏血条
	if (health_bar_bg_)
		health_bar_bg_->setVisible(false);
	if (health_bar_)
		health_bar_->setVisible(false);
}

Node* BattleUnit::GetParentNode() const
{
	return parent_node_;
}

void BattleUnit::UpdateSpritePosition()
{
	if (!unit_sprite_ || !background_sprite_ || !parent_node_)
		return;
	// 使用新的转换方法
	// state_中存储的是网格坐标(0-49, 0-49)
	Vec2 local_pos = ConvertTest::convertGridToLocal(
		state_.GetPositionX(),
		state_.GetPositionY(),
		background_sprite_
	);

	// 设置相对于game_world_的本地坐标
	// 这样当background缩放/移动时，单位会自动跟随
	unit_sprite_->setPosition(local_pos);
}

void BattleUnit::UpdateHealthBar()
{
	if (!state_.IsAlive())
	{
		if (health_bar_bg_) 
			health_bar_bg_->setVisible(false);
		if (health_bar_) 
			health_bar_->setVisible(false);
		return;
	}

	// 获取单位的本地坐标
	Vec2 unit_local_pos = ConvertTest::convertGridToLocal(
		state_.GetPositionX(),
		state_.GetPositionY(),
		background_sprite_
	);

	// 血条在单位上方30像素（本地坐标）
	Vec2 health_bar_pos = unit_local_pos + Vec2(0, 30);

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

void BattleUnit::SetPositionAttacker(float x, float y)
{
	state_.SetPosition(x, y);
	UpdateSpritePosition();  // 这里会自动进行坐标转换
	UpdateHealthBar();
}

void BattleUnit::SetPositionDefender(float x, float y)
{
	//这里采用建筑的放置规则，主要处理图标大小适配和中心2*2 3*3偏移问题
	auto building = building_.get();
	state_.SetPosition(x, y);
	//建筑就不让他动了
	building->SetGridPosition(x, y, background_sprite_);
	UpdateHealthBar();
}

void BattleUnit::SetBackgroundSprite(cocos2d::Sprite* background_sprite)
{
	background_sprite_ = background_sprite;
}

void BattleUnit::OnBattleStart()
{
	in_battle_ = true;

	// 行为系统参战
	if (behavior_)
		behavior_->OnBattleStart(this);

	if (navigation_)
		navigation_->OnBattleStart(this);

	// 清空旧目标（避免出生时锁死）
	target_ = nullptr;
}