#include "BomberBehavior.h"
#include "BattleUnit.h"
#include "BattleManager.h"
#include "AttackerData.h" // 必须包含这个来识别结构

void BomberBehavior::OnAttack(BattleUnit* attacker, BattleUnit* defender)
{
    if (!defender || !defender->IsAlive() || _hasExploded || _isFusing) return;

    // 炸弹人核心逻辑：获取数据中的引信时间
    // 效仿你 UnitState 里的 dynamic_cast 逻辑
    auto& state = attacker->GetState();

    // 这里需要通过某种方式拿到基类指针，如果 state 没提供接口，
    // 我们假设你的 AttackerData 结构在你的项目中是可访问的
    // 由于你之前的代码显示 CreateBomberData 返回的是 AttackerData
    // 我们这里直接通过转换来获取炸弹人特有的 behavior 数据

    // 注意：这里需要确保你能访问到 AttackerData 的 bomber_data 成员
    // 我们假设你把数据存在了基类的 base_data_ptr_ 里
    // 下面这段逻辑是关键：
    auto baseData = state.GetBaseData(); // 如果没有这个接口，请在 UnitState 加一个返回 base_data_ptr_.get() 的接口
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