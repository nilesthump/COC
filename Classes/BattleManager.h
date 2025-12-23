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
#include "BattleTypes.h"
#include <vector>
#include <memory>

class BattleManager
{
private:
    std::vector<std::unique_ptr<BattleUnit>> all_units_;
    std::vector<BattleUnit*> attackers_;
    std::vector<BattleUnit*> defenders_;
    bool battle_active_;
    int total_heroes_;
    int heroes_placed_;
    float battle_time_elapsed_;     //战斗已进行时间
    const float MAX_BATTLE_TIME_ = 180.0f;

    BattleResult battle_result_;

public:
    BattleManager() : battle_active_(false) ,total_heroes_(0),battle_time_elapsed_(0.0f),heroes_placed_(0),battle_result_(BattleResult::NONE){}

    //添加单位
    void AddUnit(std::unique_ptr<BattleUnit> unit, bool is_attacker);

    //设置总英雄数量
    void SetTotalHeroes(int count) { total_heroes_ = count; }
    
    //是否可以再放英雄
    bool CanDeployAttacker() const
    {
        return heroes_placed_ < total_heroes_;
    }

    //获取放置了多少英雄/攻击者
    int GetHeroesPlaced()const { return heroes_placed_; }

    //获取英雄总数
    int GetMaxHeros()const { return total_heroes_; }

    //获取敌人的列表
    std::vector<BattleUnit*> GetEnemiesFor(BattleUnit* unit) const;
   
    //设置战斗是否进行进程
    void SetBattleActive(bool s);

    //主更新循环
    void Update(double deltaTime);

    //清理死亡单位
    void RemoveDeadUnits();

    //返回战斗结果
    BattleResult EvaluateBattleResult() ;
    
    //获取结果
    BattleManager::BattleResult BattleManager::GetBattleResult() const
    {
        return battle_result_;
    }

    //战斗是否结束，给到外面一个接口
    bool BattleManager::HasBattleEnded() const
    {
       return battle_result_ != BattleResult::NONE;
    }

    //获取战斗剩余时间
    float GetRemainTime()const { return MAX_BATTLE_TIME_ - battle_time_elapsed_; }

};
#endif 

