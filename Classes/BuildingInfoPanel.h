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

    // 在protected部分添加回调声明
    void onCollectClicked(cocos2d::Ref * sender);
protected:
    // 按钮回调
    void onUpgradeClicked(cocos2d::Ref* sender);

private:
    cocos2d::Node* armyExtraPanel; // 改为Node类型，方便添加子元素
   
    Building* _targetBuilding; // 当前选中的建筑
    cocos2d::Sprite* temp;

    cocos2d::MenuItemImage* archerBtn;
    cocos2d::MenuItemImage* barbarianBtn;
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
    cocos2d::Label* _positionLabel;
    cocos2d::Label* _resourceLabel;
    cocos2d::MenuItemImage* _upgradeBtn;
    cocos2d::MenuItemImage* _collectBtn;
    //兵种
    cocos2d::Label* _barbarian;
    cocos2d::Label* _archer;
    cocos2d::Label* _giant;
    cocos2d::Label* _goblin;
    cocos2d::Label* _bomber;
    cocos2d::Label* _balloon;
};

#endif // __BUILDING_INFO_PANEL_H__