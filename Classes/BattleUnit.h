//战斗单位实体，仅用来组合单位实体的rule、way以及状态变化，不实现具体
#ifndef BATTLEUNIT_H
#define BATTLEUNIT_H
#include "UnitState.h"
#include <vector>


class BattleUnit
{
private:
	UnitState state_;			//状态数据
	

public:
	void Update(double delta_time, const std::vector<BattleUnit*>& enemies)
	{
		
	}
};

#endif