#include "BarbarianNavigation.h"
#include "BattleUnit.h"
#include "cocos2d.h"
#include <cmath>
#include <vector>
#include <string>

BarbarianNavigation::BarbarianNavigation()
{}

BarbarianNavigation::~BarbarianNavigation()
{}

BattleUnit* BarbarianNavigation::FindTarget(BattleUnit* self,
	const std::vector<BattleUnit*>& allTargets)
{
	BattleUnit* nearest = nullptr;
	double minDistance = FLT_MAX;

	for (auto target : allTargets)
	{
		if (!target->IsAlive()) continue;

		double dist = CalculateDistance(self, target);
		if (dist < minDistance)
		{
			minDistance = dist;
			nearest = target;
		}
	}
	return nearest;
}

void BarbarianNavigation::CalculateMove(BattleUnit* self, BattleUnit* target, double deltaTime)
{
	if (!target)
		return;

	// 获取自身和目标的屏幕坐标
	cocos2d::Vec2 selfScreenPos(self->GetPositionX(), self->GetPositionY());
	cocos2d::Vec2 targetScreenPos(target->GetPositionX(), target->GetPositionY());

	// 计算屏幕坐标之间的移动向量
	double dx = targetScreenPos.x - selfScreenPos.x;
	double dy = targetScreenPos.y - selfScreenPos.y;
	double distance = sqrt(dx * dx + dy * dy);

	if (distance > 0.01f)
	{
		double speed = self->GetMoveSpeed();
		double moveX = (dx / distance) * speed * deltaTime;
		double moveY = (dy / distance) * speed * deltaTime;
		// 计算新的屏幕坐标
		cocos2d::Vec2 newScreenPos = selfScreenPos + cocos2d::Vec2(moveX, moveY);

		// 设置新位置
		self->SetPosition(newScreenPos.x, newScreenPos.y);
	}
}

bool BarbarianNavigation::IsInAttackRange(BattleUnit* self, BattleUnit* target)
{
	if (!target) return false;

	double distance = CalculateDistance(self, target);
	double attackRange = self->GetAttackDistance();
	return distance <= attackRange;
}

std::string BarbarianNavigation::GetNavigationType() const
{
	return "Barbarian";
}

double BarbarianNavigation::CalculateDistance(BattleUnit* a, BattleUnit* b)
{
	if (!a || !b)
		return std::numeric_limits<double>::max();

	// 直接使用屏幕坐标计算欧几里得距离
	double dx = a->GetPositionX() - b->GetPositionX();
	double dy = a->GetPositionY() - b->GetPositionY();
	return sqrt(dx * dx + dy * dy);
}