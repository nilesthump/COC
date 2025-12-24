#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include "UnitEnums.h" 

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
		SetupForbiddenZones();
	}

	/*
	 *根据逻辑坐标标记占位
	 *logX, logY: 逻辑中心点 (-0.5 到 49.5)
	 * w, h: 占用格子宽度和高度
	 */
	void MarkOccupiedByLogicalPos(float logX, float logY, int w, int h, GridStatus status)
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
		if (s == GridStatus::FREE || s == GridStatus::FORBIDDEN) return true;

		// 2. 关于墙的逻辑判断：
		//! 炸弹人和巨人通常也会“走”过墙的索引，气球兵无视，只是移动代价(Cost)极高
		// 这里暂时由外部传入 canBreakWall 来决定是否视为路
		if (s == GridStatus::WALL && canBreakWall) return true;

		return false;
	}

	void Clear()
	{
		for (auto& row : _indexGrid) std::fill(row.begin(), row.end(), GridStatus::FREE);
		SetupForbiddenZones();
	}

private:
	int _mapSize;
	std::vector<std::vector<GridStatus>> _indexGrid;

	bool IsWithinBounds(int x, int y) const
	{
		return x >= 0 && x < _mapSize && y >= 0 && y < _mapSize;
	}

	void SetupForbiddenZones()
	{
		for (int x = 0; x < _mapSize; ++x)
		{
			for (int y = 0; y < _mapSize; ++y)
			{
				// 外圈 2 格逻辑
				if (x < 2 || x > 47 || y < 2 || y > 47)
				{
					_indexGrid[x][y] = GridStatus::FORBIDDEN;
				}
			}
		}
	}
};