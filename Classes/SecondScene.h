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

	CREATE_FUNC(SecondScene);
private:
	// 建造面板的根节点（背景+内部按钮都挂在这个节点下）
	Node* buildPanel;
};

#endif
