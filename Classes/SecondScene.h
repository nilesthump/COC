#ifndef _Login_H_
#define _Login_H_

#include "cocos2d.h"
#include "Building.h"
#include "BuildingInfoPanel.h" 
#include "GoldMine.h"
#include "GoldStorage.h"
#include "ElixirCollector.h"
#include "ElixirStorage.h"
#include "ArmyCamp.h"
#include "Walls.h"
#include "TownHall.h"
#include "ZoomScrollManager.h"
#include "DiamondGridManager.h"

USING_NS_CC;

class SecondScene :public cocos2d::Scene
{
public:
	// Constructor
	static cocos2d::Scene* createScene();

	bool virtual init();
	void menuFirstCallback(cocos2d::Ref* pSender);
	void menuBuildCallback(Ref* pSender);

	void update(float delta);

	// Touch event handlers
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);

	// Mouse event handlers
	void onMouseScroll(cocos2d::EventMouse* event);
	void onMouseMove(cocos2d::EventMouse* event);
	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesCancelled(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

	// Background bounds update
	void updateBackgroundBounds();

	//Convert screen coordinates to diamond coordinates
	cocos2d::Vec2 convertScreenToDiamond(const cocos2d::Vec2& screenPos);

	// Check if position is inside diamond
	bool isInDiamond(const cocos2d::Vec2& diamondPos);
	//new判断
	CREATE_FUNC(SecondScene);
private:
	// 双击检测相关
	double _lastClickTime; // 上一次点击的时间（使用double类型更精确）
	cocos2d::Vec2 _lastClickPos; // 上一次点击的位置
	bool _isDoubleClick; // 是否为双击
	const double DOUBLE_CLICK_INTERVAL = 0.3; // 双击时间间隔阈值（秒）

	//碰撞判断
	bool isPointInBuilding(const cocos2d::Vec2& point, cocos2d::Node* building);

	Node* buildPanel;//建造建筑面板
	Node* attackPanel;

	BuildingInfoPanel* _curOpenInfoPanel = nullptr;// 建筑信息面板
	Building* _curOpenBuilding = nullptr;

	cocos2d::Sprite* background_sprite_;

	cocos2d::Vec2 mouse_pos_;

	cocos2d::Label* coordinate_label_;

	//圣水、金币、宝石
	cocos2d::Sprite* elixirIcon; 
	cocos2d::Label* elixirLabel; 
	cocos2d::Label* elixirNameLabel; 
    cocos2d::Sprite* goldIcon; 
	cocos2d::Label* goldLabel; 
	cocos2d::Label* goldNameLabel; 
	cocos2d::Sprite* gemIcon;
	cocos2d::Label* gemLabel;
	cocos2d::Label* gemNameLabel;


	ZoomScrollManager* zoom_manager_;
	DiamondGridManager* grid_manager_;

	std::vector<std::vector<cocos2d::Vec2>>* grids_;

	// 拖拽相关成员变量
	cocos2d::MenuItemImage* goldMineBtn;
	cocos2d::MenuItemImage* elixirCollectorBtn;
	cocos2d::MenuItemImage* goldStorageBtn;
	cocos2d::MenuItemImage* elixirStorageBtn;
	cocos2d::MenuItemImage* armyCampBtn;
	cocos2d::MenuItemImage* wallsBtn;

	cocos2d::MenuItemImage* draggingItem; // 当前正在拖拽的项
	cocos2d::Vec2 dragStartPosition; // 拖拽开始时的位置
	bool isDragging; // 是否正在拖拽
    
    // 建筑移动相关成员变量
	Building* movingBuilding;
    bool isMovingBuilding; // 是否正在移动建筑

	std::vector<Building*> placedBuildings;
	int baseGoldRate; // 基础产金速率
	int baseElixirRate;
};

//圣水
extern int g_elixirCount;
//金币
extern int g_goldCount;
//宝石
extern int g_gemCount;
//上限
extern int maxLevel, maxGoldVolum, maxElixirVolum;
#endif

