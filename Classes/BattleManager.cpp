#include "BattleManager.h"
#include <vector>
#include <memory>
#include <cmath>


// BattleManager.cpp
void BattleManager::PrepareBattle(const BattleStartParams& params)
{
	// 1. 基本清理和网格初始化
	this->clear();
	this->initIndexSystem(50);

	// 2. 记账初始化 (只记必要的数据)
	current_score_.total_buildings = 0;
	current_score_.destroyed_buildings = 0;
	current_score_.gold_collected = 0;
	current_score_.elixir_collected = 0;
	current_score_.town_hall_destroyed = false;

	for (const auto& b : params.buildings)
	{
		//只有非墙建筑才计入
		if (b.type != BuildingType::WALL)
		{
			current_score_.total_buildings++;
		}
	}

	// 3. 兵力初始化
	this->SetInitialUnits(params.army);
}

void BattleManager::clear()
{
	// 1. 必须先停止所有单位的动作，再清空容器
	for (auto& unit : all_units_)
	{
		if (unit)
		{
			unit->ClearTarget(); // 切断目标引用
			unit->RemoveSprite();
		}
	}

	// 2. 清空容器，unique_ptr 会自动调用析构
	all_units_.clear();
	attackers_.clear();
	defenders_.clear();
	army_pool_.clear();

	// 3. 重置所有胜负判定和时间
	battle_active_ = false;
	battle_time_elapsed_ = 0.0f;
	total_heroes_ = 0;
	heroes_deployed = 0;
	battle_result_ = BattleResult::NONE;

	// 4. 重置网格，防止旧建筑残留“隐形墙”
	if (index_system_) index_system_->Clear();
}

//添加战斗单位
void BattleManager::AddUnit(std::unique_ptr<BattleUnit> unit, bool is_attacker)
{
	BattleUnit* raw = unit.get();
	bool battle_just_started = false;

	if (is_attacker)
	{
		attackers_.push_back(raw);
		heroes_deployed++;
		if (!battle_active_)
		{
			battle_active_ = true;
			battle_time_elapsed_ = 0;
			battle_just_started = true;
		}
	}
	else
	{
		defenders_.push_back(raw);
		auto& state = unit->GetState();
		int w = state.GetTileWidth();
		int h = state.GetTileHeight();

		//获取逻辑位置
		Vec2 pos = unit->GetLogicalPosition();

		// 自动判定状态
		GridStatus status = state.IsWall() ? GridStatus::WALL : GridStatus::BLOCKED;

		// 核心：经理自己告诉自己的网格系统，这里被占了
		index_system_->MarkOccupiedByLogicalPos(pos.x, pos.y, w, h, status,raw);
	}

	all_units_.push_back(std::move(unit));
	if (battle_just_started)
	{
		//战斗刚开始，拉所有单位参战
		for (auto& u : all_units_)
		{
			u->OnBattleStart();
		}
	}
	else if (battle_active_)
	{
		raw->OnBattleStart();
	}
}

//初始化时传入英雄及其数量
void BattleManager::SetInitialUnits(const std::vector<Army>& army)
{
	army_pool_.clear();
	total_heroes_ = 0;

	for (auto const& unit : army)
	{
		army_pool_[unit.type] = unit;
		total_heroes_ += unit.amount;
	}
}

//获取敌人的列表
std::vector<BattleUnit*> BattleManager::GetEnemiesFor(BattleUnit* unit) const
{
	//简单判断：如果在进攻方列表中，敌人就是防守方
	for (auto attacker : attackers_)
	{
		if (attacker == unit)
		{
			return defenders_;
		}
	}
	return attackers_; //否则是防守方，敌人是进攻方
}

//主更新循环
void BattleManager::update(double deltaTime)
{
	if (!battle_active_)
		return;

	battle_time_elapsed_ += deltaTime;
	//更新所有单位
	for (auto& unit : all_units_)
	{
		if (unit->IsAlive())
		{
			std::vector<BattleUnit*> enemies = GetEnemiesFor(unit.get());
			unit->update(deltaTime, enemies);
		}
	}

	RemoveDeadUnits();
	battle_result_ = EvaluateBattleResult();
	//检查战斗是否结束
	if (battle_result_ != BattleResult::NONE)
	{
		battle_active_ = false;
	}

}

//返回战斗结果
BattleResult BattleManager::EvaluateBattleResult()
{
	//还没开始战斗，不可能结束
	if (!battle_active_)
		return BattleResult::NONE;

	//达成 3 星，战斗由于“全毁”而结束
	if (CalculateStars() == BattleStar::THREE_STARS)
	{
		return BattleResult::ALL_DESTROYED;
	}

	//时间到
	if (battle_time_elapsed_ >= MAX_BATTLE_TIME_)
	{
		return BattleResult::TIME_UP;
	}

	//没兵了：兵放完了且场上没活人
	bool attackers_alive = false;
	for (auto a : attackers_)
	{
		if (a->IsAlive())
		{
			attackers_alive = true;
			break;
		}
	}
	if (!attackers_alive && heroes_deployed == total_heroes_)
	{
		return BattleResult::UNITS_EXHAUSTED;
	}

	return BattleResult::NONE;
}

void BattleManager::SetBattleActive(bool s)
{
	battle_active_ = s;
}

//移除死亡单位
void BattleManager::RemoveDeadUnits()
{
	// 遍历所有单位，找出死亡的单位
	for (auto it = all_units_.begin(); it != all_units_.end(); )
	{
		BattleUnit* unit = it->get();
		if (!unit->IsAlive())
		{
			for (auto& other : all_units_)
			{
				if (other->GetTarget() == unit)
				{
					other->ClearTarget();
				}
			}
			// 从attackers_或defenders_中移除对应的指针
			auto remove_from_vector = [unit](std::vector<BattleUnit*>& vec)
				{
					vec.erase(std::remove(vec.begin(), vec.end(), unit), vec.end());
				};

			remove_from_vector(attackers_);
			remove_from_vector(defenders_);

			// 移除视觉元素
			unit->RemoveSprite();

			// 从all_units_中移除
			it = all_units_.erase(it);
		}
		else
		{
			++it;
		}
	}
}

//实现单例的 getInstance 函数
BattleManager* BattleManager::getInstance()
{
	static BattleManager instance; // 静态局部变量确保只初始化一次
	return &instance;
}

//实时资源掉落
void BattleManager::HandleResourceDrop(BattleUnit* building, float damageTaken)
{
	auto& state = building->GetState();
	float maxHP = state.GetMaxHealth();
	if (maxHP <= 0 || !building->IsAlive()) return;

	// 1. 算出这一刀该掉多少 (基于静态总额)
	float actualDamage = std::min(damageTaken, state.GetCurrentHealth());
	float ratio = actualDamage / maxHP;
	float goldToDrop = state.GetTotalGoldStatic() * ratio;
	float elixirToDrop = state.GetTotalElixirStatic() * ratio;

	
	// 2. 账本记账
	current_score_.gold_accumulated += goldToDrop;
	current_score_.elixir_accumulated += elixirToDrop;

	CCLOG("have add");
	// 3. 核心：把 UnitState 里的“库存”减掉！
	// 这样当建筑彻底碎掉时，库存正好减到 0，不会出现账目对不上的情况。
	state.ConsumeResources(goldToDrop, elixirToDrop);

	// 更新 UI 用的整数
	current_score_.gold_collected = static_cast<int>(std::round(current_score_.gold_accumulated));
	current_score_.elixir_collected = static_cast<int>(std::round(current_score_.elixir_accumulated));
}

//摧毁百分比统计
void BattleManager::OnUnitDestroyed(BattleUnit* unit)
{
	//记账逻辑 (百分比与大本营)
	if (!unit->GetState().IsAttacker() && !unit->GetState().IsWall())
	{
		current_score_.destroyed_buildings++;
		if (unit->GetState().IsTownHall())
		{
			current_score_.town_hall_destroyed = true;
		}
	}
	// 兜底：如果建筑死的时候库存还没扣完（精度误差），全给玩家
	auto& state = unit->GetState();
	if (state.IsResourceBuilding() || state.IsTownHall())
	{
		current_score_.gold_accumulated += state.GetCurrentGoldInventory();
		current_score_.elixir_accumulated += state.GetCurrentElixirInventory();
		state.ConsumeResources(state.GetCurrentGoldInventory(), state.GetCurrentElixirInventory());
	}
	if (!unit->GetState().IsAttacker())
	{
		onBuildingDestroyed(
			unit->GetPositionX(),
			unit->GetPositionY(),
			unit->GetState().GetTileWidth(),
			unit->GetState().GetTileHeight()
		);
	}
}

BattleStar BattleManager::CalculateStars()
{
	float percent = current_score_.getPercent();
	bool hasTH = current_score_.town_hall_destroyed;

	if (percent >= 1.0f) return BattleStar::THREE_STARS;

	if (percent >= 0.5f && hasTH) return BattleStar::TWO_STARS;

	if (percent >= 0.5f || hasTH) return BattleStar::ONE_STAR;

	return BattleStar::ZERO;
}