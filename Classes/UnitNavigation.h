/*
  12.9
  以下用A星寻路算法
  算法依赖场景二维图，应继承自场景参数类（暂未实现）
  目前先写个算法雏形，该算法不包含炸弹人和空军（空军没有城墙的概念），也不包含野猪骑士（如果有）

  ps：除寻路外，还应包括对目标建筑的选择与锁定，先略过
*/
#if 0

#ifndef _UNIT_NAVIGATION_H_
#define _UNIN_NAVIGATION_H_

#include <string>
#include "CharacterData.h"
#include "cocos2d.h"
#include <stack>

#define INF 0x7fffffff

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
	bool operator!=(VecWithValue& v1, VecWithValue& v2)
	{
		return v1.x_ != v2.x_ || v1.y_ != v2.y_;
	}
};

class UnitNavigationLogic
{
public:
	//返回一条坐标通路
	static std::stack<VecWithValue> NavigationWithAStar(CharacterData offensive_unit,/*目标建筑类*/,/*当前单元格*/);
};

#endif // _UNIT_NAVIGATION_H_

#endif

