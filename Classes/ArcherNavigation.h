#ifndef _ARCHER_NAVIGATION_H_
#define _ARCHER_NAVIGATION_H_

#include "UnitNavigation.h"
#include "BattleUnit.h"
#include <cmath>
#include <climits>
#include <string>
class ArcherNavigation : public UnitNavigation
{
public:
	ArcherNavigation();
	~ArcherNavigation();
	BattleUnit* FindTarget(BattleUnit* self, const std::vector<BattleUnit*>& allTargets) override;
	void CalculateMove(BattleUnit* self, BattleUnit* target, float deltaTime) override;
	bool IsInAttackRange(BattleUnit* self, BattleUnit* target) override;
	std::string GetNavigationType() const;
	float CalculateDistance(BattleUnit* a, BattleUnit* b);

};
#endif