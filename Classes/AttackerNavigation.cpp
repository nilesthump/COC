#include "AttackerNavigation.h"
#include "UnitNavigationLogic.h"
#include "DefenderData.h"
#include "BattleManager.h"

void AttackerNavigation::CalculateMove(BattleUnit* self, BattleUnit* target, float deltaTime, IndexSystem* indexSys)
{
	if (!target || !target->IsAlive())
	{
		_currentPath.clear();
		_lastTarget = nullptr;
		return;
	}

	// 1. 攻击范围判定：采用磁滞/容差判定，防止边缘抖动
	if (this->IsInAttackRange(self, target))
	{
		_currentPath.clear();
		return;
	}

	_pathRefreshTimer -= deltaTime;
	cocos2d::Vec2 myLogPos = self->GetLogicalPosition(); // 使用纯逻辑坐标

	// 2. 动态扰动：根据单位内存地址偏移目标点，解决扎堆问题
	unsigned int seed = (unsigned int)(uintptr_t)self + (unsigned int)(_pathRefreshTimer * 1000);
	float offsetX = ((seed % 13) - 6) * 0.35f;  // 范围 [-2.1, 2.1]
	float offsetY = ((seed % 17) - 8) * 0.35f;  // 范围 [-2.8, 2.8]
	cocos2d::Vec2 targetLogPos(target->GetPositionX() + offsetX, target->GetPositionY() + offsetY);

	// 3. 寻路决策
	if (target != _lastTarget || _currentPath.empty() || _pathRefreshTimer <= 0)
	{
		_pathRefreshTimer = REFRESH_INTERVAL;
		_lastTarget = target;

		// A* 允许穿墙计算路径（canBreakWall = true），但实际移动会进行物理拦截
		auto newPath = UnitNavigationLogic::FindPath(myLogPos, targetLogPos, indexSys, true);

		if (!newPath.empty())
		{
			// 路径剪枝：如果第一个点就在身后或脚下，直接切除，保证启动丝滑
			if (newPath.front().distance(myLogPos) < 0.6f)
			{
				newPath.erase(newPath.begin());
			}
			_currentPath = newPath;
		}
	}

	// 兜底逻辑：如果路径仍为空（如距离极近），直接指向目标
	if (_currentPath.empty())
	{
		_currentPath.push_back(cocos2d::Vec2(target->GetPositionX(), target->GetPositionY()));
	}

	// 4. 执行移动与物理拦截
	cocos2d::Vec2 nextWaypoint = _currentPath.front();

	// 终点冲锋：若为最后一个路点，直接对准建筑中心而非格中心
	if (_currentPath.size() == 1)
	{
		nextWaypoint = cocos2d::Vec2(target->GetPositionX(), target->GetPositionY());
	}

	// --- 物理拦截检查 (飞行单位免检) ---
	if (self->GetState().GetUnitType()!=UnitType::BALLOON)
	{
		int wpX = IndexSystem::PosToIndex(nextWaypoint.x);
		int wpY = IndexSystem::PosToIndex(nextWaypoint.y);

		if (indexSys->GetStepCost(wpX, wpY) == COST_WALL)
		{
			//找到该格子的墙实体 (需要你实现这个接口)
			auto wallUnit = indexSys->GetUnitAtIndex(wpX, wpY);
			if (wallUnit && wallUnit->IsAlive())
			{
				// 重点：不要 self->ClearTarget()，这会导致发呆
				// 而是调用一个新方法，让单位锁定这堵墙并进入攻击状态
				_currentPath.clear();
				self->ForceAttackTarget(wallUnit);
			}
			return;
		}
	}

	cocos2d::Vec2 direction = nextWaypoint - myLogPos;
	float distToWayPoint = direction.length();
	float moveDist = self->GetState().GetMoveSpeed() * deltaTime;

	if (moveDist >= distToWayPoint)
	{
		self->SetPositionAttacker(nextWaypoint.x, nextWaypoint.y);
		if (!_currentPath.empty()) _currentPath.erase(_currentPath.begin());
	}
	else
	{
		direction.normalize();
		cocos2d::Vec2 newPos = myLogPos + direction * moveDist;

		// 步进式拦截检查
		if (self->GetState().GetUnitType() != UnitType::BALLOON)
		{
			int nextX = IndexSystem::PosToIndex(newPos.x);
			int nextY = IndexSystem::PosToIndex(newPos.y);

			if (indexSys->GetStepCost(nextX, nextY) == COST_WALL)
			{
				// 获取当前这一格的墙
				auto wallUnit = indexSys->GetUnitAtIndex(nextX, nextY);
				if (wallUnit && wallUnit->IsAlive())
				{
					_currentPath.clear();
					self->ForceAttackTarget(wallUnit); // 关键：这里也要 ForceAttackTarget
					return;
				}
			}
		}
		self->SetPositionAttacker(newPos.x, newPos.y);
	}
}

BattleUnit* AttackerNavigation::FindTarget(BattleUnit* self, const std::vector<BattleUnit*>& allTargets, float searchRadius)
{
	if (!self || allTargets.empty()) return nullptr;

	auto priority = self->GetState().GetTargetPriority();
	BattleUnit* bestTarget = nullptr;
	float minDistance = FLT_MAX;

	// --- 第一轮：扫描半径内的“非墙”偏好目标 ---
	for (auto target : allTargets)
	{
		if (!target || !target->IsAlive() || target->GetState().IsWall()) continue;
		if (!CanPhysicallyAttack(self, target)) continue;

		float dist = CalculateDistance(self, target);
		if (dist > searchRadius) continue;

		bool matchesPreference = (priority == TargetPriority::ANY);
		if (priority == TargetPriority::DEFENSES && target->GetState().IsDefenderBuilding()) matchesPreference = true;
		if (priority == TargetPriority::RESOURCES && target->GetState().IsResourceBuilding()) matchesPreference = true;

		if (matchesPreference && dist < minDistance)
		{
			minDistance = dist;
			bestTarget = target;
		}
	}

	// --- 第二轮：退化逻辑（全局寻找非墙目标，解决发呆） ---
	if (!bestTarget)
	{
		minDistance = FLT_MAX;
		for (auto target : allTargets)
		{
			if (!target || !target->IsAlive() || target->GetState().IsWall()) continue;
			if (!CanPhysicallyAttack(self, target)) continue;

			float dist = CalculateDistance(self, target);
			if (dist < minDistance)
			{
				minDistance = dist;
				bestTarget = target;
			}
		}
	}

	// --- 第三轮：特殊拦截逻辑（仅当被堵路且周围没目标时，打眼前的墙） ---
	if (!bestTarget)
	{
		minDistance = FLT_MAX;
		for (auto target : allTargets)
		{
			if (!target || !target->IsAlive() || !target->GetState().IsWall()) continue;

			float dist = CalculateDistance(self, target);
			// 只有离得非常近（撞上了）才打墙，防止连环拆墙
			if (dist < 1.2f && dist < minDistance)
			{
				minDistance = dist;
				bestTarget = target;
			}
		}
	}

	return bestTarget;
}

bool AttackerNavigation::IsInAttackRange(BattleUnit* self, BattleUnit* target)
{
	if (!target) return false;

	float realDist = CalculateDistance(self, target);
	float attackRange = self->GetAttackDistance();

	//近战给予微小容差，保证贴合感
	float tolerance = (attackRange <= 1.0f) ? -0.2f : 0.2f;
	return realDist <= (attackRange + tolerance);
}