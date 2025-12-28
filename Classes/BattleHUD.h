//战斗层的所有UI，倒计时，中途返回，摧毁进度，获取资源数，选择面板
#ifndef __BATTLE_HUD_H__
#define __BATTLE_HUD_H__

#include "cocos2d.h"
#include "UnitSelectionPanel4Battle.h"
#include "BattleManager.h"
#include <functional>

class BattleHUD : public cocos2d::Layer
{
public:
    //传入点击返回和选择兵种的回调
    static BattleHUD* create(std::function<void(cocos2d::Ref*)> onBack, std::function<void(UnitType)> onSelect);

    bool init(std::function<void(cocos2d::Ref*)> onBack, std::function<void(UnitType)> onSelect);

    // 每一帧同步 Manager 的数据
    void updateHUD(float dt);

    // 暴露给外部初始化兵种按钮
    void addUnitIcon(const std::string& path, const std::string& name, UnitType type);

private:
    // 基础组件
    UnitSelectionPanel* selection_panel_;
    cocos2d::Label* countdown_label_;
    cocos2d::Label* gold_label_;
    cocos2d::Label* elixir_label_;

    // 战报组件 (贴在面板右上)
    cocos2d::Node* war_status_node_;
    cocos2d::Label* percent_label_;
    cocos2d::Sprite* stars_[3];

    void setupTopBar(std::function<void(cocos2d::Ref*)> onBack);
    void setupWarStatus();
};

#endif