#ifndef __BUILDING_INFO_PANEL_H__
#define __BUILDING_INFO_PANEL_H__

#include "cocos2d.h"
#include "Building.h"

class BuildingInfoPanel : public cocos2d::Node {
public:
    // 静态创建方法（传入目标建筑）
    static BuildingInfoPanel* create(Building* building, cocos2d::Sprite* background_sprite_);
    // 初始化方法
   bool init(Building* building, cocos2d::Sprite* background_sprite_);
    // 更新面板信息（如升级后调用）
    void updateInfo(Building* building, cocos2d::Sprite* background_sprite_);
    void refreshUpgradeButton(Building* building);
    void updateUpgradeCountdown();
    void startUpgradeCountdownScheduler();
    void stopUpgradeCountdownScheduler();
    // 按钮回调
    void onUpgradeClicked(cocos2d::Ref* sender);
    //成功放置
    void playBlinkAnimation(cocos2d::MenuItemImage* btn) {
        if (!btn) return;

        // 停止按钮上已有的动画（避免多次点击动画叠加）
        btn->stopAllActions();

        // 透明度改边，总时长0.6秒
        auto fadeOut = cocos2d::FadeTo::create(0.3f, 50);
        auto fadeIn = cocos2d::FadeTo::create(0.3f, 255);
        auto blinkSeq = cocos2d::Sequence::create(fadeOut, fadeIn, nullptr);
        btn->runAction(blinkSeq);
    }
    void onCollectClicked(cocos2d::Ref* sender);
    void showSoldierInfo(int lv);
    void speedUpgradeClicked(cocos2d::Ref* sender);
private:
    cocos2d::Node* armyExtraPanel; // 方便添加子元素
   
    Building* _targetBuilding; //当前选中的建筑
    cocos2d::Sprite* temp;

    cocos2d::MenuItemImage* barbarianBtn;
    cocos2d::MenuItemImage* archerBtn;
    cocos2d::MenuItemImage* giantBtn;
    cocos2d::MenuItemImage* goblinBtn;
    cocos2d::MenuItemImage* bomberBtn;
    cocos2d::MenuItemImage* balloonBtn;
    cocos2d::Menu* menu;

    // UI元素
    cocos2d::Label* _titleLabel;
    cocos2d::Label* _hpLabel;
    cocos2d::Label* _levelLabel;
    cocos2d::Label* _speedLabel;
    cocos2d::Label* _resourceLabel;
    cocos2d::MenuItemImage* _upgradeBtn;
    cocos2d::MenuItemImage* _collectBtn;
    cocos2d::MenuItemImage* _speedUpBtn;
    //兵种
    cocos2d::Label* _barbarian;
    cocos2d::Label* _archer;
    cocos2d::Label* _giant;
    cocos2d::Label* _goblin; 
    cocos2d::Label* _bomber;
    cocos2d::Label* _balloon;
    //信息面板
    cocos2d::MenuItemImage* soldierInfoBtn;
    cocos2d::Node* soldierNode;
};

#endif // __BUILDING_INFO_PANEL_H__