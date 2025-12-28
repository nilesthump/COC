#include "BomberBehavior.h"
#include "BattleUnit.h"
#include "BattleManager.h"
#include "AttackerData.h"

void BomberBehavior::OnAttack(BattleUnit* attacker, BattleUnit* defender)
{
    if (!defender || !defender->IsAlive() || _hasExploded || _isFusing) return;

    // 炸弹人核心逻辑：获取数据中的引信时间

    auto& state = attacker->GetState();

    auto baseData = state.GetBaseData();
    auto aData = dynamic_cast<const AttackerData*>(baseData);

    if (aData && aData->bomber_data)
    {
        _isFusing = true;
        _fuseTimer = (float)aData->bomber_data->fuse_time;
    }
}

void BomberBehavior::OnUpdate(BattleUnit* unit, float dt)
{
    if (!_isFusing || _hasExploded) return;

    _fuseTimer -= dt;
    if (_fuseTimer <= 0)
    {
        _isFusing = false;
        ExecuteExplosion(unit, true); // 主动炸
        unit->TakeDamage(99999.0f, nullptr); // 自杀
    }
}

void BomberBehavior::OnDeath(BattleUnit* unit)
{
    // 如果死的时候还没爆，触发死亡爆炸
    if (!_hasExploded)
    {
        ExecuteExplosion(unit, false);
    }
}

void BomberBehavior::ExecuteExplosion(BattleUnit* owner, bool isActive)
{
    if (_hasExploded) return;
    _hasExploded = true;

    // 获取数据指针
    auto baseData = owner->GetState().GetBaseData();
    auto aData = dynamic_cast<const AttackerData*>(baseData);
    if (!aData || !aData->bomber_data) return;

    auto& bData = aData->bomber_data;
    float radius = isActive ? (float)bData->active_radius : (float)bData->death_radius;
    float baseDamage = isActive ? (float)bData->active_damage : (float)bData->death_damage;

    // 获取敌人并造成伤害
    auto enemies = BattleManager::getInstance()->GetEnemiesFor(owner);
    cocos2d::Vec2 myPos = owner->GetLogicalPosition();

    for (auto target : enemies)
    {
        if (!target || !target->IsAlive()) continue;

        float dist = myPos.distance(target->GetLogicalPosition());
        if (dist <= radius)
        {
            float finalDamage = baseDamage;
            if (target->GetState().IsWall())
            {
                finalDamage *= bData->wall_damage_multiplier;
            }
            target->TakeDamage(finalDamage, owner);
        }
    }
}