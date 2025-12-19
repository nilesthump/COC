#include "BarbarianNavigation.h"
#include "BattleUnit.h"
#include "cocos2d.h"
#include <cmath>
#include <vector>
#include <string>

//目前都是基于菱形网格用曼哈顿距离来计算的，但是感觉还是不太准确
//试着虽然是菱形网格但是变成正常的格数来看
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

void BarbarianNavigation::CalculateMove(BattleUnit* self, BattleUnit* target, float deltaTime)
{
    if (!target)
        return;

    // 使用地图坐标（菱形网格坐标）
    float selfX = self->GetPositionX();
    float selfY = self->GetPositionY();
    float targetX = target->GetPositionX();
    float targetY = target->GetPositionY();

    // 计算方向
    float dx = targetX - selfX;
    float dy = targetY - selfY;

    // 计算曼哈顿距离
    float manhattanDist = fabs(dx) + fabs(dy);

    if (manhattanDist > 0.01f)
    {
        // 移动速度：网格单位/秒
        float speed = self->GetMoveSpeed();  // 2.0格/秒
        float moveAmount = speed * deltaTime;  // 本次可移动的网格数

        float newX = selfX;
        float newY = selfY;

        // 在菱形网格中移动：优先移动X方向
        if (fabs(dx) > 0)
        {
            // 计算X方向移动量
            float moveX = (dx > 0) ?
                std::min(moveAmount, fabs(dx)) :
                -std::min(moveAmount, fabs(dx));
            newX += moveX;
            moveAmount -= fabs(moveX);
        }

        // 如果还有移动量，移动Y方向
        if (moveAmount > 0 && fabs(dy) > 0)
        {
            float moveY = (dy > 0) ?
                std::min(moveAmount, fabs(dy)) :
                -std::min(moveAmount, fabs(dy));
            newY += moveY;
        }

        // 设置新的地图坐标位置
        self->SetPosition(newX, newY);
    }
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

float BarbarianNavigation::CalculateDistance(BattleUnit* a, BattleUnit* b)
{
	// 保持兼容性，但使用曼哈顿距离
	return CalculateManhattanDistance(a, b);
}

// 新的曼哈顿距离计算方法
float BarbarianNavigation::CalculateManhattanDistance(BattleUnit* a, BattleUnit* b)
{
	if (!a || !b)
		return std::numeric_limits<float>::max();

	// 曼哈顿距离：|dx| + |dy|
	float dx = a->GetPositionX() - b->GetPositionX();
	float dy = a->GetPositionY() - b->GetPositionY();
	return fabs(dx) + fabs(dy);
}