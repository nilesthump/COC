/*
 * BattleManager.h - 战斗管理器类
 * 核心职责：战斗场景的总控制器，协调所有战斗单位的行动
 * 设计模式：管理者模式 + 游戏循环模式
 *
 *              BattleManager（战斗总控）
 *                      ↓ 管理
 *   ┌────────┴────────┐
 *   ↓                                  ↓
 * BattleUnit                       BattleUnit
 *   ↓                                  ↓
 * （进攻方）                        （防守方）
 *
 * 1. 管理所有参战单位
 * 2. 控制战斗节奏
 * 3. 判断胜负条件
 * 4. 协调单位互动
 */
#ifndef BATTLEMANAGER_H
#define BATTLEMANAGER_H
#include "BattleUnit.h"
#include "UnitFactory.h"
#include <vector>
#include <memory>

//? 敌人列表的实时更新体现在哪里？比如一个对象死了之后，对方应该获取的敌人列表中会减掉它
class BattleManager
{
private:
    std::vector<std::unique_ptr<BattleUnit>> all_units_;
    std::vector<BattleUnit*> attackers_;
    std::vector<BattleUnit*> defenders_;
    bool battle_active_;

public:
    BattleManager() : battle_active_(false) {}

    //添加单位
    void AddUnit(std::unique_ptr<BattleUnit> unit, bool is_attacker);

    //获取敌人的列表
    std::vector<BattleUnit*> GetEnemiesFor(BattleUnit* unit) const;
   
    //主更新循环
    void Update(double deltaTime);

    //检查战斗是否结束
    //! 这里还有个时间限制检查没加进去
    bool IsBattleOver() const;
    
    void BattleManager::StartBattle()
    {
        battle_active_ = true;
    }

    void BattleManager::EndBattle()
    {
        battle_active_ = false;
    }

    //给到Scene用于添加血条、音效、战斗姿态等
    const std::vector<std::unique_ptr<BattleUnit>>& GetAllUnits() const
    {
        return all_units_;
    }

};
#endif 

