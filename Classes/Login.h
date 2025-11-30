#ifndef _Login_H_
#define _Login_H_

#include "cocos2d.h"

class Login :public cocos2d::Scene 
{
public:
	static cocos2d::Scene* createScene();

	bool virtual init();

	void menuCloseCallback(cocos2d::Ref* pSender);

	CREATE_FUNC(Login);
};

#endif
