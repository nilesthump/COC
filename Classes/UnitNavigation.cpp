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

//曼哈顿距离，用于加权
int ManhattanDistance(const VecWithValue& current_cell, const VecWithValue& target_cell)
{
	return abs(current_cell.x_ - target_cell.x_) + abs(current_cell.y_ - target_cell.y_);
}


//重载优先队列比较函数
class Compare {
public:
	bool operator()(std::pair<VecWithValue, VecWithValue> v1, std::pair<VecWithValue, VecWithValue> v2)
	{
		return v1.second.min_distance_ > v2.second.min_distance_;
	}
};

std::stack<VecWithValue> UnitNavigationLogic::NavigationWithAStar(CharacterData offensive_unit,
	const VecWithValue& target_cell, const VecWithValue& current_cell)
{
	//方向搜索数组
	int tx[] = { -1,-1,-1,0,0,1,1,1 }, ty[] = { -1,0,1,-1,1,-1,0,1 };

	std::vector<std::vector<double>> graph_2d(2 * offensive_unit.visible_range_ - 1,
		std::vector<double>(2 * offensive_unit.visible_range_ - 1, 1.0 / offensive_unit.move_speed));
	graph_2d[offensive_unit.visible_range_ - 1][offensive_unit.visible_range_ - 1] = 0;

	//标记坐标是否被访问
	std::vector<std::vector<bool>> visit_2d(2 * offensive_unit.visible_range_ - 1,
		std::vector<bool>(2 * offensive_unit.visible_range_ - 1, false);

	/*
		此处添加搜索范围内城墙并为graph_2d对应位置加上破墙所需时间
	*/

	std::map<VecWithValue, VecWithValue> road;//用于记录某个坐标的前置坐标，构建逆序通路

	int nx = target_cell.x_ - current_cell.x_;
	int ny = target_cell.y_ - current_cell.y_;
	std::pair<VecWithValue, VecWithValue> start = { offensive_unit.visible_range_ - 1, offensive_unit.visible_range_ - 1, 0 ,
	offensive_unit.visible_range_ - 1, offensive_unit.visible_range_ - 1, 0 };

	VecWithValue target(start.x + nx, start.y + ny);//目标单元格坐标记录

	std::priority_queue<std::pair<VecWithValue, VecWithValue>,
		std::vector<std::pair<VecWithValue, VecWithValue>>, Compare> search_2d;

	search_2d.push({ start,start });


	/*
		每次取出一对(first为起点，second为终点)，判断second是否已到达，
		否则加入路径并继续搜索，已到达过则跳过。
		向八个方向搜索，将其赋权距离加入队列
		判定当前出队的是目标坐标则退出循环
	*/
	while (!search_2d.empty())
	{
		std::pair<VecWithValue, VecWithValue> current = search_2d.top();
		search_2d.pop();
		if (current.second.x_ < 0 || current.second.x_ >= 2 * offensive_unit.visible_range_ - 1 ||
			current.second.y_ < 0 || current.second.y_ >= 2 * offensive_unit.visible_range_ - 1 ||
			visit[current.second.x_][current.second.y_])
			continue;
		visit[current.second.x_][current.second.y_] = true;
		road[current.second] = current.first;
		if (current.second.x_ == target.x_ && current.second.y_ == target.y_)
			break;
		for (int i = 0; i < 8; i++)
		{
			std::pair<VecWithValue, VecWithValue> next = { 
				current.second.x_,current.second.y_,current.second.min_distance_
				current.second.x_ + tx[i],current.second.y_ + ty[i],
				graph_2d[current.second.x_ + tx[i]][current.second.y_ + ty[i]] };

			next.second.min_distance_ += ManhattanDistance(next.second, target);

			search_2d.push(next);
		}
	}

	std::stack<VecWithValue> target_road;
	target_road.push(target);

	while (target != start.first)
	{
		target = road[target];
		target_road.push(target);
	}

	return target_road;

}


#endif