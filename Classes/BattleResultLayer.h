#include "cocos2d.h"
#include "BattleTypes.h"

class BattleResultLayer : public cocos2d::LayerColor
{
public:
    static BattleResultLayer* create(BattleResult result);
    bool init(BattleResult result);

private:
    void playEnterAnimation();
    void onExitClicked(cocos2d::Ref* sender);
};
