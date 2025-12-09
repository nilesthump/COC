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

class UnitNavigationLogic
{
public:
	static /*目标单元格*/ NavigationWithAStar(CharacterData offensive_unit,/*目标建筑类*/,/*当前单元格*/);
};

#endif // _UNIT_NAVIGATION_H_

#endif

