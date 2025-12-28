#include "DefenderNormalBehavior.h"
#include "BattleUnit.h"
#include "BattleManager.h"
#include "StaticBuildingBehavior.h" 

void DefenderNormalBehavior::OnUpdate(BattleUnit* unit, float deltaTime)
{
    //auto target = unit->GetTarget();
    //if (target && unit->GetSprite())
    //{
    //    // 核心：旋转炮塔指向目标
    //    Vec2 direction = target->GetSprite()->getPosition() - unit->GetSprite()->getPosition();
    //    float angle = -CC_RADIANS_TO_DEGREES(direction.getAngle());

    //    // 这里的角度可能需要根据你的美术素材进行 +90 或 -90 的修正
    //    unit->GetSprite()->setRotation(angle);
    //}
}

void DefenderNormalBehavior::OnAttack(BattleUnit* attacker, BattleUnit* defender)
{
    // 这里是“加农炮”或“箭塔”发射的地方
    if (!defender || !defender->IsAlive()) return;

    // 1. 伤害应用
    defender->TakeDamage(attacker->GetDamage(), attacker);

    //// 2. 视觉反馈：开火闪烁或炮管后坐力
    //auto recoil = MoveBy::create(0.05f, -attacker->GetSprite()->getForwardVector() * 5);
    //auto recover = MoveBy::create(0.15f, attacker->GetSprite()->getForwardVector() * 5);
    //attacker->GetSprite()->runAction(Sequence::create(recoil, recover, nullptr));

    // 3. TODO: 这里以后可以生成一个 Projectile (子弹) 对象，而不是瞬间扣血
}

void DefenderNormalBehavior::OnDamageTaken(BattleUnit* unit, float damage, BattleUnit* attacker)
{
    //// 这里触发你之前写的“静态建筑行为”逻辑：爆钱动画和记账
    //StaticBuildingBehavior::OnDamageTaken(unit, damage, attacker);

    //// 建筑受击抖动
    //if (unit->GetSprite())
    //{
    //    auto shake = Sequence::create(
    //        MoveBy::create(0.05f, Vec2(2, 0)),
    //        MoveBy::create(0.05f, Vec2(-2, 0)),
    //        nullptr
    //    );
    //    unit->GetSprite()->runAction(shake);
    //}
}

void DefenderNormalBehavior::OnDeath(BattleUnit* unit)
{
    auto bm = BattleManager::getInstance();
    bm->OnUnitDestroyed(unit);
}