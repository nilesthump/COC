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
#include "BuilderHut.h"
#include "ZoomScrollManager.h"
#include "DiamondGridManager.h"
#include "WebSocketManager.h"
#include "network/WebSocket.h"
#include "CombatSessionManager.h"
#include "BattleTestLayer.h"
#include"UnitEnums.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"

USING_NS_CC;
using namespace network;

class SecondScene :public cocos2d::Scene
{
public:
	// Constructor
	static cocos2d::Scene* createScene();

	bool virtual init();
	void menuFirstCallback(cocos2d::Ref* pSender);
	void menuBuildCallback(Ref* pSender);

	void menuAttackCallback(Ref* pSender);
	void menuBoss1Callback(Ref* pSender);
	void menuBoss2Callback(Ref* pSender);

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

	//返回未满
	Building* getGoldStorage() {
		for (auto building : placedBuildings) {
			if (dynamic_cast<GoldStorage*>(building) && building->getCurrentStock() < building->getMaxStock()) {
				return building;
			}
		}
	}
	Building* getElixirStorage() {
		for (auto building : placedBuildings) {
			if (dynamic_cast<ElixirStorage*>(building) && building->getCurrentStock() < building->getMaxStock()) {
				return building;
			}
		}
	}
	friend void BuildingInfoPanel::onUpgradeClicked(Ref* sender);
	//new判断
	CREATE_FUNC(SecondScene);

	// WebSocket callbacks
	void setupWebSocketCallbacks();
	void setupWebSocketAndRequestResources();
	void onWebSocketMessage(const std::string& message);
	void sendGetResourceRequest();
	void sendUpdateResourceRequest(float dt);
	void sendSaveBuildingRequest(const std::string& buildingType, float x, float y, int level);
	void sendDeleteBuildingRequest(float x, float y);
	void sendGetBuildingsRequest();
	void onWebSocketBuildingsMessage(const std::string& message);
	void onEnter() override;
	void onExit() override;

private:
	// WebSocket回调相关成员变量
	bool _sceneIsDestroyed;
	// 双击检测相�?
	double _lastClickTime; // 上一次点击的时间（使用double类型更精确）
	cocos2d::Vec2 _lastClickPos; // 上一次点击的位置
	bool _isDoubleClick; // 是否为双�?
	const double DOUBLE_CLICK_INTERVAL = 0.3; // 双击时间间隔阈值（秒）

	bool isPointInBuilding(const cocos2d::Vec2& point, Building* building);

	Node* buildPanel;
	Node* attackPanel;

	BuildingInfoPanel* _curOpenInfoPanel = nullptr;// 建筑信息面板
	Building* _curOpenBuilding = nullptr;

	cocos2d::Sprite* background_sprite_;

	cocos2d::Vec2 mouse_pos_;

	cocos2d::Label* coordinate_label_;

	//圣水、金币、宝�?
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
	//�ؿ���ť
	cocos2d::MenuItemImage* boss1Btn;
	cocos2d::MenuItemImage* boss2Btn;

	// 拖拽相关成员变量
	cocos2d::MenuItemImage* goldMineBtn;
	cocos2d::MenuItemImage* elixirCollectorBtn;
	cocos2d::MenuItemImage* goldStorageBtn;
	cocos2d::MenuItemImage* elixirStorageBtn;
	cocos2d::MenuItemImage* armyCampBtn;
	cocos2d::MenuItemImage* wallsBtn;
	cocos2d::MenuItemImage* builderHutBtn;

	cocos2d::MenuItemImage* draggingItem; // 当前正在拖拽的项
	cocos2d::Vec2 dragStartPosition; // 拖拽开始时的位�?
	bool isDragging; // 是否正在拖拽
    
    // 建筑移动相关成员变量
	Building* movingBuilding;
    bool isMovingBuilding; // 是否正在移动建筑
	cocos2d::Vec2 _movingBuildingOriginalPos; // 移动建筑时的原始位置

	static std::vector<Building*> placedBuildings;
	int baseGoldRate; // 基础产金速率
	int baseElixirRate;

	bool _buildingsInitialized; // 防止重复初始化建�?

	Building* createBuildingByType(const std::string& buildingType);
	void initDefaultBuildingsAndSave();
};

extern int maxLevel, maxGoldVolum, maxElixirVolum;
extern int g_elixirCount, g_goldCount, g_gemCount, hutNum;

#endif

