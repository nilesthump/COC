#ifndef _Login_H_
#define _Login_H_

#include "cocos2d.h"

class SecondScene :public cocos2d::Scene
{
public:
	//声明
	static cocos2d::Scene* createScene();
	
	bool virtual init();
	void menuFirstCallback(cocos2d::Ref* pSender);
	void menuBuildCallback(Ref* pSender);

	//触摸事件处理函数
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);

	//缩放事件处理函数
	void onMouseScroll(cocos2d::EventMouse* event);
	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesCancelled(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

	//辅助函数
	void updateBackgroundBounds();

	CREATE_FUNC(SecondScene);
private:
	// 建造面板的根节点（背景+内部按钮都挂在这个节点下）
	Node* buildPanel;

	//场景底部图精灵
	cocos2d::Sprite* background_sprite;

	//初始化拖拽相关变量
	bool is_dragging;
	cocos2d::Vec2 drag_start_position;
	cocos2d::Vec2 background_start_position;

	//缩放相关变量
	bool is_scaling;
	float previous_distance;
	cocos2d::Vec2 scale_center;
	float min_scale;//最小缩放比例
	float max_scale;//最大缩放比例
};

#endif
