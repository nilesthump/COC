#include "UnitNavigation.h"
#include "BattleUnit.h"

bool UnitNavigation::CanPhysicallyAttack(BattleUnit* attacker, BattleUnit* target)
{
    if (!target || !target->IsAlive()) return false;

    auto myAbility = attacker->GetAttackTargetType();   // 攻击者的打击能力 (GROUND/AIR/BOTH)
    auto targetType = target->GetUnitTargetType();      // 目标的物理存在 (GROUND/AIR)

    if (myAbility == AttackTargetType::BOTH) return true;
    if (myAbility == AttackTargetType::GROUND && targetType == UnitTargetType::GROUND) return true;
    if (myAbility == AttackTargetType::AIR && targetType == UnitTargetType::AIR) return true;

    return false;
}

float UnitNavigation::CalculateDistance(BattleUnit* a, BattleUnit* b)
{
    cocos2d::Vec2 posA(a->GetPositionX(), a->GetPositionY());
    cocos2d::Vec2 posB(b->GetPositionX(), b->GetPositionY());

    // 如果目标是防御建筑，计算点到矩形边缘的距离
    if (b->GetState().IsDefender())
    {
        // 注意：COC中距离计算通常是基于格子的。这里tile_width 是占地格子数
        float halfW = b->GetState().GetTileWidth() * 0.5f;
        float halfH = b->GetState().GetTileHeight() * 0.5f;

        float dx = std::max(0.0f, std::abs(posA.x - posB.x) - halfW);
        float dy = std::max(0.0f, std::abs(posA.y - posB.y) - halfH);
        return std::sqrt(dx * dx + dy * dy);
    }
    return posA.distance(posB);
}