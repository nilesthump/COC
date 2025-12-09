/*
  12.9
  以下坐标均表示单元格的相对坐标，非像素点坐标
*/

#if 0

#include "UnitNavigation.h"
#include <vector>
#include <queue>
#include <cmath>

#define INF 0x7fffffff

//曼哈顿距离，用于加权
int ManhattanDistance(/*单元格1*/,/*单元格2*/)
{
	return abs(/*单元格1横坐标-单元格2横坐标*/)+abs(/*单元格1纵坐标-单元格2纵坐标*/)
}

//带权坐标向量
struct  VecWithValue {
	int x_ = 0, y_ = 0;
	double min_distance_ = INF;//最小加权距离
	VecWithValue(int x, int y) :
		x_(x), y_(y) {
	}
	VecWithValue(int x, int y, double min_distance) :
		x_(x), y_(y), min_distance_(min_distance) {
	}
};


//重载优先队列比较函数
class Compare {
public:
	bool operator()(VecWithValue v1, VecWithValue v2)
	{
		return v1.min_distance_ > v2.min_distance_;
	}
};

/*目标单元格*/ UnitNavigationLogic::NavigationWithAStar(CharacterData offensive_unit,/*目标建筑类*/,/*当前单元格*/)
{
	//方向搜索数组
	int tx[] = { -1,-1,-1,0,0,1,1,1 }, ty[] = { -1,0,1,-1,1,-1,0,1 };

	if (目标建筑类中心单元格横坐标 != 当前单元格横坐标 && 目标建筑类中心单元格纵坐标 != 当前单元格纵坐标)
	{
		//一般二维图的情况
		vector<vector<double>> graph_2d(/*abs(目标建筑类中心单元格横坐标-当前单元格横坐标)*/,
			vector<double>(/*abs(目标建筑类中心单元格纵坐标-当前单元格纵坐标)*/,INF));
		priority_queue<VecWithValue.vector<VecWithValue>, Compare> search_2d;

		VecWithValue current(0,0,0);//当前单元格坐标记录

		//define n 目标建筑类中心单元格横坐标-当前单元格横坐标
		//define m 目标建筑类中心单元格纵坐标-当前单元格纵坐标
		if (n * m > 0)
		{
			graph_2d[0][0] = graph_2d[abs(n) - 1][abs(m) - 1] = 0;
			if (n > 0)
				current.x = current.y = 0;
			else
				current.x = abs(n) - 1, current.y = abs(m) - 1;
		}
		else
		{
			graph_2d[abs(n) - 1][0] = graph_2d[0][abs(m) - 1] = 0;
			if (n > 0)
				current.x = abs(n) - 1;
			else
				current.y = abs(m) - 1;
		}


	}
	else
	{
		//在同一行或同一列的情况
		vector<double> graph_1d(/*abs(目标建筑类中心单元格横或纵坐标-当前单元格横或纵坐标)*/, INF);
		priority_queue<VecWithValue, vector<VecWithValue>, Compare> search_1d;

		//define n abs(目标建筑类中心单元格横或纵坐标-当前单元格横或纵坐标)
		graph_1d[0] = graph[n - 1] = 0;
	}

}


#endif