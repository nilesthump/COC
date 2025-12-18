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
	// 建造面板节点
	Node* buildPanel;

	// 背景精灵引用
	cocos2d::Sprite* background_sprite_;

	// 鼠标位置
	cocos2d::Vec2 mouse_pos_;

	// UI元素
	cocos2d::Label* coordinate_label_;

	// 管理器实例
	ZoomScrollManager* zoom_manager_;
	DiamondGridManager* grid_manager_;
};

#endif
