#include "UnitNavigationLogic.h"
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <cmath>

struct AStarNode
{
	int ix, iy;      // 索引坐标 [0-49]
	float g;        // 起点到当前的实际代价
	float h;        // 当前到终点的预估代价
	AStarNode* parent;

	AStarNode(int ix, int iy, float g, float h, AStarNode* p = nullptr)
		: ix(ix), iy(iy), g(g), h(h), parent(p)
	{}

	float f() const { return g + h; }
};

//内部使用的 A* 节点比较器，用于优先队列排序
struct NodeComparator
{
	bool operator()(AStarNode* a, AStarNode* b)
	{
		return a->f() > b->f(); // F值越小优先级越高
	}
};

//辅助函数：计算欧几里得距离（作为 H 启发值）
static float GetHeuristic(int x1, int y1, int x2, int y2)
{
	return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}

std::vector<Vec2> UnitNavigationLogic::FindPath(const Vec2& startLogPos, const Vec2& endLogPos, const IndexSystem* indexSys, bool canBreakWall)
{
	//坐标转换：逻辑位置 (-0.5~49.5) -> 数组索引 (0~49)，因为a*是基于格子这种的，不应该是连续
	int startX = IndexSystem::PosToIndex(startLogPos.x);
	int startY = IndexSystem::PosToIndex(startLogPos.y);
	int endX = IndexSystem::PosToIndex(endLogPos.x);
	int endY = IndexSystem::PosToIndex(endLogPos.y);

	//如果起点终点重合，直接返回
	if (startX == endX && startY == endY) return { startLogPos };

	// 只有当目标点是 BLOCKED (1000.0f) 时才需要扩散
	// 不要使用 IsIndexWalkable，因为它会排除掉墙
	if (indexSys->GetStepCost(endX, endY) >= COST_BLOCKED)
	{
		bool foundNearby = false;
		for (int r = 1; r <= 5 && !foundNearby; ++r)
		{
			for (int dx = -r; dx <= r; ++dx)
			{
				for (int dy = -r; dy <= r; ++dy)
				{
					int nx = endX + dx, ny = endY + dy;
					if (indexSys->IsWithinBounds(nx, ny) && indexSys->GetStepCost(nx, ny) < 1000.0f)
					{
						endX = nx; endY = ny;
						foundNearby = true;
						break;
					}
				}
				if (foundNearby) break;
			}
		}
		if (!foundNearby) return {};
	}

	//算法初始化
	std::priority_queue<AStarNode*, std::vector<AStarNode*>, NodeComparator> openList;
	std::unordered_map<int, AStarNode*> allNodes; // 用于记录所有生成的节点以便后续内存释放和查找
	auto getID = [](int x, int y) { return y * 50 + x; };

	bool closedList[50][50] = { false };

	AStarNode* startNode = new AStarNode(startX, startY, 0, GetHeuristic(startX, startY, endX, endY));
	openList.push(startNode);
	allNodes[getID(startX, startY)] = startNode;

	AStarNode* targetNode = nullptr;

	//A* 主循环
	while (!openList.empty())
	{
		AStarNode* current = openList.top();
		openList.pop();

		if (closedList[current->ix][current->iy]) continue;
		closedList[current->ix][current->iy] = true;

		//检查是否到达（修正后的）终点
		if (current->ix == endX && current->iy == endY)
		{
			targetNode = current;
			break;
		}

		for (int dx = -1; dx <= 1; ++dx)
		{
			for (int dy = -1; dy <= 1; ++dy)
			{
				if (dx == 0 && dy == 0) continue;

				int nx = current->ix + dx;
				int ny = current->iy + dy;

				if (!indexSys->IsWithinBounds(nx, ny)) continue;

				// --- 核心修改：使用权重计算代价 ---
				float weight = indexSys->GetStepCost(nx, ny);

				// 如果单位不能开墙，且目标格是墙，则视为不可走
				if (!canBreakWall && weight == COST_WALL) continue;
				// 如果是绝对阻塞（建筑中心），则不可走
				if (weight >= COST_BLOCKED) continue;

				if (closedList[nx][ny]) continue;

				// 斜向移动的穿模检查（如果侧边有绝对阻塞，则不能斜走）
				if (dx != 0 && dy != 0)
				{
					if (indexSys->GetStepCost(current->ix + dx, current->iy) >= COST_BLOCKED ||
						indexSys->GetStepCost(current->ix, current->iy + dy) >= COST_BLOCKED)
					{
						continue;
					}
				}

				// 计算步长：直线 1.0, 斜向 1.414
				float stepDist = (dx == 0 || dy == 0) ? 1.0f : 1.414f;
				// 新的 G = 当前 G + (步长 * 权重)
				float newG = current->g + (stepDist * weight);

				int id = getID(nx, ny);
				if (allNodes.find(id) == allNodes.end() || newG < allNodes[id]->g)
				{
					float h = GetHeuristic(nx, ny, endX, endY);
					if (allNodes.find(id) == allNodes.end())
					{
						AStarNode* neighbor = new AStarNode(nx, ny, newG, h, current);
						allNodes[id] = neighbor;
						openList.push(neighbor);
					}
					else
					{
						allNodes[id]->g = newG;
						allNodes[id]->parent = current;
						openList.push(allNodes[id]);
					}
				}
			}
		}
	}

	//路径回溯
	std::vector<Vec2> finalPath;
	if (targetNode)
	{
		AStarNode* curr = targetNode;
		while (curr)
		{
			//将索引坐标转回逻辑中心坐标
			finalPath.push_back(Vec2((float)curr->ix, (float)curr->iy));
			curr = curr->parent;
		}
		std::reverse(finalPath.begin(), finalPath.end());
	}

	//内存释放
	for (auto& pair : allNodes)
	{
		delete pair.second;
	}

	return finalPath;
}