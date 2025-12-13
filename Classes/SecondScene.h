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
};

#endif
