#ifndef _Login_H_
#define _Login_H_

#include "cocos2d.h"
#include "ZoomScrollManager.h"
#include "DiamondGridManager.h"

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
	CREATE_FUNC(SecondScene);
private:

	Node* buildPanel;


	cocos2d::Sprite* background_sprite_;


	cocos2d::Vec2 mouse_pos_;


	cocos2d::Label* coordinate_label_;


	cocos2d::Sprite* elixirIcon; 
	cocos2d::Label* elixirLabel; 
	cocos2d::Label* elixirNameLabel; 
    
    // 金币相关成员变量
    cocos2d::Sprite* goldIcon; 
	cocos2d::Label* goldLabel; 
	cocos2d::Label* goldNameLabel; 

	ZoomScrollManager* zoom_manager_;
	DiamondGridManager* grid_manager_;

	std::vector<std::vector<cocos2d::Vec2>>* grids_;

	// 拖拽相关成员变量
	cocos2d::MenuItemImage* houseBtn;
	cocos2d::MenuItemImage* storageBtn;
	cocos2d::MenuItemImage* draggingItem; // 当前正在拖拽的项
	cocos2d::Vec2 dragStartPosition; // 拖拽开始时的位置
	bool isDragging; // 是否正在拖拽
    
    // 建筑移动相关成员变量
    std::vector<cocos2d::Sprite*> placedBuildings; // 已放置的建筑列表
    cocos2d::Sprite* movingBuilding; // 当前正在移动的建筑
    bool isMovingBuilding; // 是否正在移动建筑
};

//圣水
extern int g_elixirCount;
//金币
extern int g_goldCount;

#endif

