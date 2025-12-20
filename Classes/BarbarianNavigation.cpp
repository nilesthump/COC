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
	float minDistance = FLT_MAX;

	for (auto target : allTargets)
	{
		if (!target->IsAlive()) continue;

		float dist = CalculateDistance(self, target);
		if (dist < minDistance)
		{
			minDistance = dist;
			nearest = target;
		}
	}
	return nearest;
}

//这里没有用A*，直接朝目标移动
void BarbarianNavigation::CalculateMove(BattleUnit* self, BattleUnit* target, float deltaTime)
{
    if (!target || !self)
        return;

    // 获取当前位置和目标位置
    float selfX = self->GetPositionX();
    float selfY = self->GetPositionY();
    float targetX = target->GetPositionX();
    float targetY = target->GetPositionY();

    // 计算到目标的距离
    float dx = targetX - selfX;
    float dy = targetY - selfY;
    float distance = sqrt(dx * dx + dy * dy);  // 直接计算欧几里得距离

    // 如果已经非常接近目标，就不需要移动
    if (distance < 0.01f)
    {
        return;
    }

    // 获取移动速度并计算本次移动量
    float speed = self->GetMoveSpeed();  // 网格单位/秒，如2.0
    float moveAmount = speed * deltaTime;

    // 如果本次移动会超过目标位置，则直接移动到目标位置
    if (moveAmount >= distance)
    {
        self->SetPosition(targetX, targetY);
        return;
    }

    // 计算移动方向（单位向量）
    float dirX = dx / distance;
    float dirY = dy / distance;

    // 计算新位置
    float newX = selfX + dirX * moveAmount;
    float newY = selfY + dirY * moveAmount;

    // 设置新的位置
    self->SetPosition(newX, newY);
}

bool BarbarianNavigation::IsInAttackRange(BattleUnit* self, BattleUnit* target)
{
	if (!target) return false;

	float distance = CalculateDistance(self, target);
	float attackRange = self->GetAttackDistance();
	return distance <= attackRange;
}

std::string BarbarianNavigation::GetNavigationType() const
{
	return "Barbarian";
}

//欧几里得距离
float BarbarianNavigation::CalculateDistance(BattleUnit* a, BattleUnit* b)
{
    if(!a || !b)
        return std::numeric_limits<float>::max();

    float dx = a->GetPositionX() - b->GetPositionX();
    float dy = a->GetPositionY() - b->GetPositionY();
    return sqrt(dx * dx + dy * dy);
}
