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
#include "BattleSnapshot.h"
#include "IndexSystem.h"
#include "UnitEnums.h"
#include <vector>
#include <memory>
#include <map>

class BattleManager
{
private:
    std::vector<std::unique_ptr<BattleUnit>> all_units_;
    std::vector<BattleUnit*> attackers_;
    std::vector<BattleUnit*> defenders_;
    bool battle_active_;
    int total_heroes_;
    int heroes_deployed;
    std::map<UnitType, int> unit_counts_; // 存储每种英雄的剩余数量
    float battle_time_elapsed_;     //战斗已进行时间
    const float MAX_BATTLE_TIME_ = 180.0f;
    std::unique_ptr<IndexSystem> index_system_;
    LiveBattleScore current_score_;
    BattleResult battle_result_;

public:
    BattleManager() : battle_active_(false) ,total_heroes_(0),battle_time_elapsed_(0.0f),heroes_deployed(0),battle_result_(BattleResult::NONE),current_score_({}) {}

    //聚合初始化
    void PrepareBattle(const BattleStartParams& params);

    //清理
    void clear();

    //获取当前账本（供UI
    const LiveBattleScore& getCurrentScore() const { return current_score_; }

    //添加单位
    void AddUnit(std::unique_ptr<BattleUnit> unit, bool is_attacker);

    //初始化时传入英雄及其数量
    void SetInitialUnits(const std::map<UnitType, int>& units);

    // 检查特定英雄是否还能放置，对于个体
    bool CanDeployUnit(UnitType type)
    {
        return unit_counts_[type] > 0;
    }

    // 减少计数
    void decrementUnitCount(UnitType type)
    {
        if (unit_counts_[type] > 0) unit_counts_[type]--;
    }

    //获取当前剩余类型英雄数量
    int getRemainingUnitCount(UnitType type) { return unit_counts_[type]; }
    
    //获取放置了多少英雄/攻击者
    int GetHeroesPlaced()const { return heroes_deployed; }

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
    BattleResult GetBattleResult() const
    {
        return battle_result_;
    }

    //战斗是否结束，给到外面一个接口
    bool HasBattleEnded() const
    {
       return battle_result_ != BattleResult::NONE;
    }

    //获取战斗剩余时间
    float GetRemainTime()const { return MAX_BATTLE_TIME_ - battle_time_elapsed_; }

    static BattleManager* getInstance();

    //初始化网格系统
    void initIndexSystem(int size)
    {
        index_system_ = std::make_unique<IndexSystem>(size);
    }

    IndexSystem* GetIndexSystem() { return index_system_.get(); }

    //当建筑被摧毁时，调用此方法更新网格
    void onBuildingDestroyed(float logX, float logY, int w, int h)
    {
        if (index_system_)
        {
            index_system_->MarkOccupiedByLogicalPos(logX, logY, w, h, GridStatus::FREE, nullptr);
        }
    }

    //实时资源掉落
    void HandleResourceDrop(BattleUnit* building, float damageRatio);
  
    //摧毁百分比统计
    void OnUnitDestroyed(BattleUnit* unit);
    
    //计算结果星星
    BattleStar CalculateStars();
};
#endif 

