#include "BattleUnit.h"
#include "BattleManager.h"
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
	CCLOG("BattleUnit destructor called");

	// 逻辑清理
	target_ = nullptr;

	// ===== 渲染清理：只需removeFromParent，父节点会自动管理内存 =====

	// 单位精灵
	if (unit_sprite_)
	{
		unit_sprite_->stopAllActions();  // 停止动画防止回调崩溃
		unit_sprite_->removeFromParent();
		unit_sprite_ = nullptr;
	}

	// 血条背景 - 不需要release
	if (health_bar_bg_)
	{
		health_bar_bg_->stopAllActions();
		health_bar_bg_->removeFromParent();
		health_bar_bg_ = nullptr;
	}

	// 血条进度条 - 不需要release
	if (health_bar_)
	{
		health_bar_->stopAllActions();
		health_bar_->removeFromParent();
		health_bar_ = nullptr;
	}

	// 建筑组件清理
	if (building_)
	{
		auto root = building_->GetRootNode();
		if (root)
		{
			root->removeFromParent();
		}
	}
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

void BattleUnit::update(float deltaTime, std::vector<BattleUnit*>& enemies)
{
	static int logFrame = 0;
	logFrame++;
	if (!in_battle_ || !state_.IsAlive()) return;

	// A. 环境获取：从上帝那里拿到唯一的网格
	auto indexSys = BattleManager::getInstance()->GetIndexSystem();

	// B. 状态更新
	state_.UpdateCoolDowns(deltaTime);
	// C. 目标管理
	if (navigation_)
	{
		bool shouldFindNew = false;
		bool isDefender = (state_.IsDefenderBuilding() || state_.IsResourceBuilding() || state_.IsWall());
		// 情况 1: 没目标或者目标死了
		if (!target_ || !target_->IsAlive())
		{
			shouldFindNew = true;
		}
		// 情况 2: (核心改进) 目标虽然活着，但已经跑出我的射程了
		else if (isDefender)
		{
			float currentDist = navigation_->CalculateDistance(this, target_);
			if (currentDist > state_.GetSearchRange())
			{
				shouldFindNew = true;
				target_ = nullptr; // 强制清除旧目标
			}
		}
		if (shouldFindNew)
		{
			auto enemies = BattleManager::getInstance()->GetEnemiesFor(this);
			float searchRadius = state_.GetSearchRange();
			target_ = navigation_->FindTarget(this, enemies, searchRadius);
		}
	}

	// D. 导航与移动
	if (target_ && navigation_)
	{
		bool in_range = navigation_->IsInAttackRange(this, target_);
		if (!in_range)
		{
			// 没够着，移动
			if (state_.IsAttacker())
			{
				navigation_->CalculateMove(this, target_, deltaTime, indexSys);
			}
		}
		else
		{
			//够着了，尝试执行 Behavior 里的攻击逻辑
			if (state_.CanAttack() && behavior_)
			{
				behavior_->OnAttack(this, target_);
				state_.ResetAttackCooldown();
				// PlayAttackSound(); // 如果每个兵音效不一样，这行也可以移入 behavior
			}
		}
	}

	// E. 行为补充更新 (如特效更新等)
	if (behavior_)
		behavior_->OnUpdate(this, deltaTime);

	// F. 视觉同步
	if (state_.IsAttacker())
		UpdateSpritePosition();
	UpdateHealthBar();
}

//这里的takedamage组合了减伤+受伤反应（暂时没用)+死亡处理，behavior需要再考虑
void BattleUnit::TakeDamage(float damage, BattleUnit* source)
{
	if (!state_.IsAlive()) return; // 已经死了就不处理
	state_.TakeDamage(damage);

	//行为钩子 (用于播放动画、特效、爆钱粒子)
	if (behavior_)
	{
		behavior_->OnDamageTaken(this, damage, source);

		if (!state_.IsAlive())
		{
			behavior_->OnDeath(this);
			PlayDeathSound();
		}
	}
}

//切换目标，遇到墙时调用（attackernavigation）
void BattleUnit::ForceAttackTarget(BattleUnit* target)
{
	if (!target || !target->IsAlive()) return;
	target_ = target;
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

Vec2 BattleUnit::GetLogicalPosition()const
{
	return state_.GetLogicalPosition();
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

TargetPriority BattleUnit::GetTargetPriority() const
{
	return state_.GetTargetPriority();
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

BuildingComponent* BattleUnit::GetBuildingComponent() const
{
	return building_.get();
}

//视觉组件设置
void BattleUnit::SetSprite(cocos2d::Sprite* sprite, cocos2d::Node* parent)
{
	if (unit_sprite_ && unit_sprite_->getParent())
	{
		unit_sprite_->stopAllActions();
		unit_sprite_->removeFromParent();
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
	if (unit_sprite_) return unit_sprite_;

	// 如果是建筑，从组件里获取精灵
	if (building_)
	{
		return building_->GetSprite();
	}

	return nullptr;
}

void BattleUnit::SetupHealthBar(Node* parent)
{
	parent_node_ = parent;

	// 先清理旧的血条（如果存在）
	if (health_bar_bg_)
	{
		health_bar_bg_->removeFromParent();
		health_bar_bg_ = nullptr;
	}
	if (health_bar_)
	{
		health_bar_->removeFromParent();
		health_bar_ = nullptr;
	}

	health_bar_bg_ = Sprite::create("healthbar1.png");
	if (health_bar_bg_)
	{
		health_bar_bg_->setScale(0.05f);
		parent_node_->addChild(health_bar_bg_, 10);  // 父节点会自动管理
	}

	Sprite* health_sprite = Sprite::create("healthbar0.png");
	if (health_sprite)
	{
		health_bar_ = ProgressTimer::create(health_sprite);
		health_bar_->setType(ProgressTimer::Type::BAR);
		health_bar_->setMidpoint(Vec2(0, 0.5f));
		health_bar_->setBarChangeRate(Vec2(1, 0));
		health_bar_->setScale(0.05f);
		health_bar_->setPercentage(100.0f);
		parent_node_->addChild(health_bar_, 11);  // 父节点会自动管理
	}
	if (health_bar_bg_) health_bar_bg_->setVisible(false);
	if (health_bar_) health_bar_->setVisible(false);
}

void BattleUnit::RemoveSprite()
{
	if (unit_sprite_)
	{
		// 核心修改：增加对运行状态的判断
		// isRunning() 确保节点还在活动的场景树中且没有被销毁
		if (unit_sprite_->isRunning())
		{
			unit_sprite_->stopAllActions();
		}

		if (unit_sprite_->getParent() != nullptr)
		{
			unit_sprite_->removeFromParentAndCleanup(true);
		}
		unit_sprite_ = nullptr;
	}

	if (health_bar_bg_)
	{
		health_bar_bg_->removeFromParent();
		health_bar_bg_ = nullptr;
	}

	if (health_bar_)
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
	if (!unit_sprite_) return;
	Vec2 local_pos = ConvertTest::convertGridToLocal(
		state_.GetPositionX(),
		state_.GetPositionY(),
		background_sprite_
	);

	// 从 userData 获取高度偏移量 (如果有的话)
	float extraHeight = (float)(uintptr_t)unit_sprite_->getUserData();

	// 设置精灵位置（逻辑位置 + 视觉高度）
	unit_sprite_->setPosition(local_pos + Vec2(0, extraHeight));
}

void BattleUnit::UpdateHealthBar()
{
	if (!health_bar_bg_ || !health_bar_ || !background_sprite_) 
		return;

	if (!state_.IsAlive())
	{
		health_bar_bg_->setVisible(false);
		health_bar_->setVisible(false);
		return;
	}

	float healthPercent = GetHealthPercent();
	bool shouldShow = true;
	if (!state_.IsAttacker() && healthPercent >= 0.99f)
	{
		shouldShow = false;
	}
	health_bar_bg_->setVisible(shouldShow);
	health_bar_->setVisible(shouldShow);

	
	if (shouldShow)
	{
		Vec2 unit_local_pos = ConvertTest::convertGridToLocal(
			state_.GetPositionX(),
			state_.GetPositionY(),
			background_sprite_
		);

		float flightHeight = 0.0f;
		if (state_.GetUnitType() == UnitType::BALLOON)
		{
			flightHeight = 60.0f; 
		}

		Vec2 health_bar_pos = unit_local_pos + Vec2(0, flightHeight + 40.0f);

		health_bar_bg_->setPosition(health_bar_pos);
		health_bar_->setPosition(health_bar_pos);
		health_bar_->setPercentage(healthPercent * 100.0f);
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