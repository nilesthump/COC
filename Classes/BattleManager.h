/*
 * BattleManager.h - 战斗管理器类
 *
 * 核心职责：战斗场景的总控制器，协调所有战斗单位的行动
 * 设计模式：管理者模式 + 游戏循环模式
 *
 * 架构位置：
 *        BattleManager（战斗总控）
 *            ↓ 管理
 *   ┌────────┴────────┐
 *   ↓                 ↓
 * BattleUnit       BattleUnit
 *   ↓                 ↓
 * （进攻方）        （防守方）
 *
 * 类比说明：
 * BattleManager 就像是战场上的"导演"或"裁判"：
 * 1. 管理所有参战单位（演员）
 * 2. 控制战斗节奏（帧率）
 * 3. 判断胜负条件（规则）
 * 4. 协调单位互动（调度）
 */
#ifndef BATTLEMANAGER_H
#define BATTLEMANAGER_H
#include "BattleUnit.h"
#include "UnitFactory.h"
#include <vector>
#include <memory>

class BattleManager
{
private:
    std::vector<std::unique_ptr<BattleUnit>> allUnits_;
    std::vector<BattleUnit*> attackers_;
    std::vector<BattleUnit*> defenders_;
    bool battleActive_;

public:
    BattleManager() : battleActive_(false) {}

    // 添加单位
    void AddUnit(std::unique_ptr<BattleUnit> unit, bool isAttacker)
    {
        if (isAttacker)
        {
            attackers_.push_back(unit.get());
        }
        else
        {
            defenders_.push_back(unit.get());
        }
        allUnits_.push_back(std::move(unit));
    }

    // 获取敌人的列表
    std::vector<BattleUnit*> GetEnemiesFor(BattleUnit* unit) const
    {
        // 简单判断：如果在进攻方列表中，敌人就是防守方
        for (auto attacker : attackers_)
        {
            if (attacker == unit)
            {
                return defenders_;
            }
        }
        return attackers_; // 否则是防守方，敌人是进攻方
    }

    // 主更新循环
    void Update(float deltaTime)
    {
        if (!battleActive_) return;

        // 更新所有单位
        for (auto& unit : allUnits_)
        {
            if (unit->IsAlive())
            {
                std::vector<BattleUnit*> enemies = GetEnemiesFor(unit.get());
                unit->Update(deltaTime, enemies);
            }
        }

        // 清理死亡单位（可选）
        // RemoveDeadUnits();

        // 检查战斗是否结束
        if (IsBattleOver())
        {
            EndBattle();
        }
    }

    // 创建测试战斗
    void CreateTestBattle()
    {
        // 创建野蛮人
        auto barbarian = std::unique_ptr<BattleUnit>(UnitFactory::CreateBarbarian(1));
        barbarian->SetPosition(0.0f, 0.0f);
        AddUnit(std::move(barbarian), true);

        // 创建箭塔
        auto tower = std::unique_ptr<BattleUnit>(UnitFactory::CreateArcherTower(1));
        tower->SetPosition(15.0f, 0.0f); // 距离15格
        AddUnit(std::move(tower), false);

        battleActive_ = true;
        printf("测试战斗创建完成！\n");
        printf("野蛮人血量: %.0f, 箭塔血量: %.0f\n",
            attackers_[0]->GetState().GetHealth(),
            defenders_[0]->GetState().GetHealth());
    }

    // 检查战斗是否结束
    bool IsBattleOver() const
    {
        // 检查是否还有存活的进攻方
        bool attackersAlive = false;
        for (auto attacker : attackers_)
        {
            if (attacker->IsAlive())
            {
                attackersAlive = true;
                break;
            }
        }

        // 检查是否还有存活的防守方
        bool defendersAlive = false;
        for (auto defender : defenders_)
        {
            if (defender->IsAlive())
            {
                defendersAlive = true;
                break;
            }
        }

        return !attackersAlive || !defendersAlive;
    }

    // 结束战斗
    void EndBattle()
    {
        battleActive_ = false;
        printf("战斗结束！\n");

        if (attackers_.empty() || !attackers_[0]->IsAlive())
        {
            printf("防御方胜利！\n");
        }
        else
        {
            printf("进攻方胜利！\n");
        }
    }

    // 获取战斗状态（用于UI显示）
    void PrintBattleStatus() const
    {
        printf("=== 战斗状态 ===\n");
        for (size_t i = 0; i < allUnits_.size(); ++i)
        {
            auto& unit = allUnits_[i];
            printf("单位%d: %s, 血量: %.0f/%d, 位置: (%.1f, %.1f)%s\n",
                (int)i,
                unit->GetBaseData().name.c_str(),
                unit->GetState().GetHealth(),  // double → %.0f没问题
                unit->GetBaseData().health,    // int → 用%d
                unit->GetPositionX(),
                unit->GetPositionY(),
                unit->IsAlive() ? "" : " [死亡]");
        }
        printf("================\n");
    }
};
#endif 

