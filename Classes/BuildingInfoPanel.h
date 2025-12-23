#ifndef __BUILDING_INFO_PANEL_H__
#define __BUILDING_INFO_PANEL_H__

#include "cocos2d.h"
#include "Building.h"

class BuildingInfoPanel : public cocos2d::Node {
public:
    // 静态创建方法（传入目标建筑）
    static BuildingInfoPanel* create(Building* building);

    // 初始化方法
   bool init(Building* building);

    // 更新面板信息（如升级后调用）
    void updateInfo(Building* building);

protected:
    // 按钮回调
    void onUpgradeClicked(cocos2d::Ref* sender);

private:
    Building* _targetBuilding; // 当前选中的建筑

    // UI元素
    cocos2d::Label* _titleLabel;
    cocos2d::Label* _hpLabel;
    cocos2d::Label* _levelLabel;
    cocos2d::Label* _speedLabel;
    cocos2d::MenuItemImage* _upgradeBtn;
};

#endif // __BUILDING_INFO_PANEL_H__