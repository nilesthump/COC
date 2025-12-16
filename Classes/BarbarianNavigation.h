#ifndef _BARBARIAN_NAVIGATION_H_
#define _BARBARIAN_NAVIGATION_H_
#include "UnitNavigation.h"
#include "BattleUnit.h"
#include <cmath>
#include <climits>
#include <string>
class BarbarianNavigation : public UnitNavigation
{
public:
	BarbarianNavigation();
	~BarbarianNavigation();
	BattleUnit* FindTarget(BattleUnit* self,const std::vector<BattleUnit*>& allTargets) override;
	void CalculateMove(BattleUnit* self, BattleUnit* target, double deltaTime) override;
	bool IsInAttackRange(BattleUnit* self, BattleUnit* target) override;
	std::string GetNavigationType() const;
private:
	double CalculateDistance(BattleUnit* a, BattleUnit* b);
};
#endif