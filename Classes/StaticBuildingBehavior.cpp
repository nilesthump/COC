// StaticBuildingBehavior.cpp
#include "StaticBuildingBehavior.h"
#include "BattleUnit.h"
#include "BattleManager.h"

void StaticBuildingBehavior::OnUpdate(BattleUnit* unit, float deltaTime)
{
    //静态建筑每帧不需要动脑子，留空即可
}

void StaticBuildingBehavior::OnDamageTaken(BattleUnit* unit, float damage, BattleUnit* source)
{
    CCLOG("OnDamageTaken triggered for: %s", unit->GetState().IsTownHall() ? "TownHall" : "ResourceBuilding");
    //只有资源类建筑（金矿、圣水罐、大本营）才会在受损时掉钱
    if (unit->GetState().IsResourceBuilding() || unit->GetState().IsTownHall())
    {
        //告诉经理：我受伤了，请按伤害比例扣除我身上的资源给玩家
        BattleManager::getInstance()->HandleResourceDrop(unit, (float)damage);
        if (unit->GetState().IsTownHall())
        {
            CCLOG("HERE?");
            showLootAnimation(unit, ResourceType::GOLD);
            showLootAnimation(unit, ResourceType::ELIXIR);
        }
        // 如果是普通资源建筑，按需跳动
        else
        {
            if (unit->GetState().GetTotalGoldStatic() > 0)
                showLootAnimation(unit, ResourceType::GOLD);
            if (unit->GetState().GetTotalElixirStatic() > 0)
                showLootAnimation(unit, ResourceType::ELIXIR);
        }
    }
}

void StaticBuildingBehavior::OnDeath(BattleUnit* unit)
{
    auto bm = BattleManager::getInstance();
    bm->OnUnitDestroyed(unit);
    // 1. 播放建筑倒塌/爆炸动画
    //! unit->PlayDeathAnimation(); 
    
}

void StaticBuildingBehavior::showLootAnimation(BattleUnit* unit, ResourceType type)
{
    auto parent = unit->GetParentNode();
    auto comp = unit->GetBuildingComponent();
    if (!parent || !comp) return;

    //根据资源类型选择图片
    std::string frameName = (type == ResourceType::GOLD) ? "GoldCoin.png" : "Elixir.png";
    auto icon = Sprite::create(frameName);

    if (!icon) return;

    //初始位置：建筑中心加一点点随机偏移，让连击时图标不重叠
    Vec2 startPos = comp->GetRootNode()->getPosition();
    startPos.x += (rand() % 40 - 20);
    startPos.y += (rand() % 40 - 20);
    icon->setPosition(startPos);
    icon->setScale(0.3f); 
    parent->addChild(icon, 999);

    // 组合动作：跳跃 + 旋转 + 消失
    // JumpBy 让它有一个弧线跳跃感
    auto jump = JumpBy::create(0.8f, Vec2(0, 40), 30, 1);
    auto rotate = RotateBy::create(0.8f, 180);
    auto fadeOut = FadeOut::create(0.8f);
    auto remove = RemoveSelf::create();

    // 同时执行跳跃、旋转和淡出
    auto spawn = Spawn::create(jump, rotate, fadeOut, nullptr);
    icon->runAction(Sequence::create(spawn, remove, nullptr));
}