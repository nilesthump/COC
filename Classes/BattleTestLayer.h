#ifndef __BATTLE_TEST_LAYER_H__
#define __BATTLE_TEST_LAYER_H__

#include "cocos2d.h"
#include "BattleManager.h"
#include "ZoomScrollManager.h"
#include "DiamondGridManager.h"
#include <memory>
#include <vector>

class BattleTestLayer : public cocos2d::Layer
{
private:
	// 核心系统
	std::unique_ptr<BattleManager> battle_manager_;
	DiamondGridManager* grid_manager_ = nullptr;
	ZoomScrollManager* zoom_manager_ = nullptr;

	// 视觉元素
	cocos2d::Sprite* background_sprite_ = nullptr;
	cocos2d::Node* game_world_ = nullptr;		// 世界容器，包含所有游戏对象

	// 坐标显示
	cocos2d::Label* coordinates_label_ = nullptr;		// 显示所有单位坐标的标签
	std::vector<std::pair<int, cocos2d::Vec2>> barbarian_positions_;		// 存储每个Barbarian的ID和网格坐标
	int next_barbarian_id_ = 1;			

	// 倒计时显示
	Label* countdown_label_ = nullptr;

	// 双击检测
	double last_click_time_ = 0;
	cocos2d::Vec2 last_click_pos_ = cocos2d::Vec2::ZERO;

	// 当前触摸
	double touch_begin_time_ = 0;
	cocos2d::Vec2 touch_begin_pos_ = cocos2d::Vec2::ZERO;

	// 地图拖拽
	bool is_dragging_map_ = false;

	//结果显示，这个标志位防止update()每一帧都弹出结算界面
	bool result_layer_shown_ = false;

public:
	static cocos2d::Scene* createScene();

	virtual bool init() override;
	virtual void update(float delta) override;

	void menuFirstCallback(cocos2d::Ref* pSender);

	//初始化
	void setupBattle();

	//置逻辑
	void placeDefender();
	void placeBarbarianAt(float gridX, float gridY);
	void updateCoordinatesDisplay();

	//触摸事件
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);

	//鼠标滚轮事件
	void onMouseScroll(cocos2d::EventMouse* event);

	//英雄数量满了不让放置的提示信息
	void showDeployHintAtWorldPos(const cocos2d::Vec2& worldPos);

	//结果界面展示layer
	void showBattleResultLayer(BattleResult result);

	CREATE_FUNC(BattleTestLayer);
};

#endif // __BATTLE_TEST_LAYER_H__
