#ifndef _BATTLE_TEST_SCENE_H_
#define _BATTLE_TEST_SCENE_H_

#include "cocos2d.h"
#include <memory>

class BattleManager;

class BattleTestScene : public cocos2d::Layer
{
private:
    std::unique_ptr<BattleManager> battleMgr_;

public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    void update(float delta) override;
    
    CREATE_FUNC(BattleTestScene);
};

#endif

