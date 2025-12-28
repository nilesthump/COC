#pragma once
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include "UnitEnums.h" 

const float COST_WALKABLE = 1.0f;    // 普通地面
const float COST_WALKABLE_FORBIDDEN = 1.2f;    // 普通地面
const float COST_WALL = 50.0f;        // 围墙代价（相当于绕路25格）
const float COST_BLOCKED = 1000.0f;  // 建筑中心或地图外（绝对不可走）

class BattleUnit;

class IndexSystem
{
public:
	// 核心转换：逻辑位置 -> 数组索引
	static int PosToIndex(float pos)
	{
		return std::round(pos);
	}

	IndexSystem(int size = 50) : _mapSize(size)
	{
		// 统一使用 _indexGrid 作为变量名
		_indexGrid.assign(size, std::vector<GridStatus>(size, GridStatus::FREE));
		_unitGrid.assign(size, std::vector<BattleUnit*>(size, nullptr));
		_deployableMap.assign(size, std::vector<bool>(size, true)); // 默认全地图可放
		SetupForbiddenZones();
	}

	void MarkOccupiedByLogicalPos(float logX, float logY, int w, int h, GridStatus status, BattleUnit* unit)
	{
		// 纠正公式：根据中心点计算覆盖的整数索引范围
		int xMin = std::round(logX - (w - 1) / 2.0f);
		int xMax = std::round(logX + (w - 1) / 2.0f);
		int yMin = std::round(logY - (h - 1) / 2.0f);
		int yMax = std::round(logY + (h - 1) / 2.0f);

		for (int x = xMin; x <= xMax; ++x)
		{
			for (int y = yMin; y <= yMax; ++y)
			{
				if (IsWithinBounds(x, y))
				{
					_indexGrid[x][y] = status;
					_unitGrid[x][y] = unit;
				}
			}
		}
	}

	// A* 寻路用的判断：基于索引
	bool IsIndexWalkable(int x, int y, bool canBreakWall = false) const
	{
		if (!IsWithinBounds(x, y)) return false;
		GridStatus s = _indexGrid[x][y];

		// 1. 空地和草坪永远可走
		if (s == GridStatus::FREE || s == GridStatus::GRASS) return true;

		// 2. 关于墙的逻辑判断：
		//! 炸弹人和巨人通常也会“走”过墙的索引，气球兵无视，只是移动代价(Cost)极高
		// 这里暂时由外部传入 canBreakWall 来决定是否视为路
		if (s == GridStatus::WALL && canBreakWall) return true;

		return false;
	}

	void Clear()
	{
		for (auto& row : _indexGrid) std::fill(row.begin(), row.end(), GridStatus::FREE);
		for (auto& row : _unitGrid) std::fill(row.begin(), row.end(), nullptr);
		SetupForbiddenZones();
	}

	//计算代价
	float GetStepCost(int x, int y) const
	{
		if (!IsWithinBounds(x, y)) return COST_BLOCKED;

		GridStatus status = _indexGrid[x][y];

		switch (status)
		{
		case GridStatus::FREE:
			return COST_WALKABLE;
		case GridStatus::GRASS:
			return COST_WALKABLE_FORBIDDEN;
		case GridStatus::WALL:
			return COST_WALL;
		case GridStatus::BLOCKED:
			return COST_BLOCKED;
		default:
			return COST_WALKABLE;
		}
	}

	BattleUnit* GetUnitAtIndex(int x, int y) const
	{
		if (IsWithinBounds(x, y))
		{
			return _unitGrid[x][y];
		}
		return nullptr;
	}

	bool IsWithinBounds(int x, int y) const
	{
		return x >= 0 && x < _mapSize && y >= 0 && y < _mapSize;
	}

	/**
	 * 在所有建筑/墙放置完毕后，调用此函数生成“下兵红线”
	 */
	void UpdateDeploymentMap()
	{
		_deployableMap.assign(_mapSize, std::vector<bool>(_mapSize, false));

		// 1. 第一步：暴力膨胀闭合（用2格半径堵住3格缝隙）
		std::vector<std::vector<bool>> bigBarrier(_mapSize, std::vector<bool>(_mapSize, false));
		for (int x = 0; x < _mapSize; ++x)
		{
			for (int y = 0; y < _mapSize; ++y)
			{
				GridStatus s = _indexGrid[x][y];
				if (s == GridStatus::BLOCKED || s == GridStatus::WALL)
				{
					for (int dx = -2; dx <= 2; ++dx)
					{
						for (int dy = -2; dy <= 2; ++dy)
						{
							int nx = x + dx, ny = y + dy;
							if (IsWithinBounds(nx, ny)) bigBarrier[nx][ny] = true;
						}
					}
				}
			}
		}

		// 2. 灌水算法 (Flood Fill)：找出外部可通达区域
		std::vector<std::vector<bool>> isOutside(_mapSize, std::vector<bool>(_mapSize, false));
		std::queue<std::pair<int, int>> q;

		// 从地图最外圈边缘注入“水流”
		for (int i = 0; i < _mapSize; ++i)
		{
			if (!bigBarrier[i][0]) q.push({ i, 0 });
			if (!bigBarrier[i][_mapSize - 1]) q.push({ i, _mapSize - 1 });
		}
		for (int j = 1; j < _mapSize - 1; ++j)
		{
			if (!bigBarrier[0][j]) q.push({ 0, j });
			if (!bigBarrier[_mapSize - 1][j]) q.push({ _mapSize - 1, j });
		}

		while (!q.empty())
		{
			auto curr = q.front(); q.pop();
			if (isOutside[curr.first][curr.second]) continue;
			isOutside[curr.first][curr.second] = true;

			int dx[] = { 1, -1, 0, 0 }, dy[] = { 0, 0, 1, -1 };
			for (int i = 0; i < 4; ++i)
			{
				int nx = curr.first + dx[i], ny = curr.second + dy[i];
				if (IsWithinBounds(nx, ny) && !bigBarrier[nx][ny] && !isOutside[nx][ny])
				{
					q.push({ nx, ny });
				}
			}
		}

		// 3. 第三步：最终判定逻辑
		for (int x = 0; x < _mapSize; ++x)
		{
			for (int y = 0; y < _mapSize; ++y)
			{
				// --- 核心逻辑 A：物理 1 格判定（最高优先级） ---
				bool nearBuilding = false;
				for (int dx = -1; dx <= 1; ++dx)
				{
					for (int dy = -1; dy <= 1; ++dy)
					{
						int nx = x + dx, ny = y + dy;
						if (IsWithinBounds(nx, ny))
						{
							GridStatus s = _indexGrid[nx][ny];
							if (s == GridStatus::BLOCKED || s == GridStatus::WALL)
							{
								nearBuilding = true;
								break;
							}
						}
					}
					if (nearBuilding) break;
				}

				// 如果在 1 格范围内，不论水能不能流到，铁定不能放
				if (nearBuilding)
				{
					_deployableMap[x][y] = false;
					continue;
				}

				// --- 核心逻辑 B：针对“外圈变肥”的关键修正 ---
				// 此时我们已经在 1 格物理范围外了。
				// 如果 2 格处水流被挡住了，但我们周围 1 格内有格子是“外部(isOutside)”，
				// 那说明我们其实是处于“外海”的边缘，而不是被围死的内部。

				bool canReachWater = false;
				if (isOutside[x][y])
				{
					canReachWater = true;
				}
				else
				{
					// 尝试向周围看一圈，如果邻居有水，我也算外海
					for (int dx = -1; dx <= 1; ++dx)
					{
						for (int dy = -1; dy <= 1; ++dy)
						{
							int nx = x + dx, ny = y + dy;
							if (IsWithinBounds(nx, ny) && isOutside[nx][ny])
							{
								canReachWater = true;
								break;
							}
						}
						if (canReachWater) break;
					}
				}

				_deployableMap[x][y] = canReachWater;
			}
		}
	}

	bool CanDeployAt(float gridX, float gridY) const
	{

		int x = std::floor(gridX + 0.5f);
		int y = std::floor(gridY + 0.5f);

		if (x < 0 || x >= _mapSize || y < 0 || y >= _mapSize)
		{
			return false;
		}
		return _deployableMap[x][y];
	}

private:
	int _mapSize;
	std::vector<std::vector<GridStatus>> _indexGrid;
	std::vector<std::vector<BattleUnit*>> _unitGrid; // 存储单位指针的网格
	std::vector<std::vector<bool>> _deployableMap; // 下兵掩码
	void SetupForbiddenZones()
	{
		for (int x = 0; x < _mapSize; ++x)
		{
			for (int y = 0; y < _mapSize; ++y)
			{
				// 外圈 2 格逻辑
				if (x < 2 || x > 47 || y < 2 || y > 47)
				{
					_indexGrid[x][y] = GridStatus::GRASS;
				}
			}
		}
	}
};