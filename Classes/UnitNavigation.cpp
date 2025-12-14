/*
  12.9
  以下坐标均表示单元格的相对坐标，非像素点坐标
*/

#if 0

#include "UnitNavigation.h"
#include <vector>
#include <queue>
#include <cmath>
#include <map>
#include <set>


//曼哈顿距离，用于加权
int ManhattanDistance(const VecWithValue& current_cell, const VecWithValue& target_cell)
{
	return abs(current_cell.x_ - target_cell.x_) + abs(current_cell.y_ - target_cell.y_);
}


//重载优先队列比较函数
class Compare {
public:
	bool operator()(const VecWithValue& v1,const VecWithValue& v2)
	{
		return v1.min_distance_ > v2.min_distance_;
	}
};

std::stack<VecWithValue> UnitNavigationLogic::NavigationWithAStar(const VecWithValue& target_cell,
	const VecWithValue& current_cell,
	bool is_defensive_unit,
	const AttackerData* offensive_unit)
{
	//方向搜索数组
	int tx[] = { -1,-1,-1,0,0,1,1,1 }, ty[] = { -1,0,1,-1,1,-1,0,1 };

	//假设破墙时间，实际由兵种伤害决定
	const double OBSTACLE_DESTRUCTION_TIME = 5.0;

	std::map<VecWithValue, double>distance_map;
	std::map<VecWithValue, VecWithValue> parent_map;
	std::set<VecWithValue> visited;

	std::priority_queue<VecWithValue,
		std::vector<VecWithValue>, Compare> open_list;

	//起点入队
	auto start_cell = current_cell;
	start_cell.min_distance_ = ManhattanDistance(start_cell, target_cell);
	distance_map[start_cell] = 0.0;
	open_list.push(start_cell);

	bool found_path = false;

	//A*算法主循环
	while (!open_list.empty())
	{
		auto current = open_list.top();
		open_list.pop();

		//到达目标
		if (current == target_cell)
		{
			found_path = true;
			break;
		}

		//已访问则跳过
		if (visited.count(current) > 0)
			continue;

		//标记为已访问
		visited.insert(current);
		for (int i = 0; i < 8; i++)
		{
			VecWithValue next_cell(current.x_ + tx[i], current.y_ + ty[i]);
			if (visited.count(next_cell) > 0)
				continue;
			double move_cost = 1.0;
			if (offensive_unit != nullptr && offensive_unit->CanMove())
				move_cost /= offensive_unit->move_speed;

			double new_cost = distance_map[current] + move_cost;
			//更新距离与父节点
			if (distance_map.find(next_cell) == distance_map.end() || new_cost < distance_map[next_cell])
			{
				distance_map[next_cell] = new_cost;
				parent_map[next_cell] = current;
				next_cell.min_distance_ = new_cost + ManhattanDistance(next_cell, target_cell);
				open_list.push(next_cell);
			}
		}
	}
	//构建路径
	std::stack<VecWithValue> path;
	if (found_path)
	{
		auto current = target_cell;
		while (true)
		{
			path.push(current);
			if (current == start_cell)
				break;
			current = parent_map[current];
		}
	}

	return path;
}


#endif