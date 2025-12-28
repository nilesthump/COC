#include "BattleResultLayer.h"
#include "HelloWorldScene.h"
#include "BattleManager.h"
USING_NS_CC;

BattleResultLayer* BattleResultLayer::create(BattleResult result)
{
    auto ret = new (std::nothrow) BattleResultLayer();
    if (ret && ret->init(result))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool BattleResultLayer::init(BattleResult result)
{
    if (!LayerColor::initWithColor(Color4B(0, 0, 0, 180)))
        return false;

    auto size = Director::getInstance()->getVisibleSize();
    auto bm = BattleManager::getInstance();

    // 1. 获取星级（假设 BattleManager 已经根据 LiveBattleScore 计算好了）
    BattleStar star = bm->CalculateStars();

    std::string titleText;
    Color3B titleColor;

    // 2. 根据战斗评价决定颜色
    if (star == BattleStar::ZERO)
    {
        titleText = "DEFEAT";
        titleColor = Color3B::RED;
    }
    else
    {
        titleText = StringUtils::format("VICTORY (%d STARS)", (int)star);
        titleColor = Color3B::YELLOW;
    }

    // 3. 根据战斗结束原因（你的 BattleResult）添加补充描述
    std::string reasonText;
    switch (result)
    {
    case BattleResult::ALL_DESTROYED:
        reasonText = "Total Destruction!";
        break;
    case BattleResult::UNITS_EXHAUSTED:
        reasonText = "No Units Left";
        break;
    case BattleResult::TIME_UP:
        reasonText = "Time Is Up";
        break;
    default:
        reasonText = "Battle Ended";
        break;
    }

    // 主标题 (胜利/失败)
    auto titleLabel = Label::createWithSystemFont(titleText, "Arial", 64);
    titleLabel->setColor(titleColor);
    titleLabel->setPosition(size.width / 2, size.height / 2 + 100);
    this->addChild(titleLabel);

    // 副标题 (结束原因)
    auto reasonLabel = Label::createWithSystemFont(reasonText, "Arial", 24);
    reasonLabel->setPosition(size.width / 2, size.height / 2 + 30);
    this->addChild(reasonLabel);

    auto exitItem = MenuItemLabel::create(
        Label::createWithSystemFont("EXIT", "Arial", 32),
        CC_CALLBACK_1(BattleResultLayer::onExitClicked, this)
    );

    auto menu = Menu::create(exitItem, nullptr);
    menu->setPosition(size.width / 2, size.height / 2 - 40);
    this->addChild(menu);

    playEnterAnimation();
    return true;
}

//Layer 出现时，用一个淡入动画
void BattleResultLayer::playEnterAnimation()
{
    this->setOpacity(0);
    this->runAction(FadeTo::create(0.3f, 180));
}

void BattleResultLayer::onExitClicked(Ref*)
{
    BattleManager* battleManager = BattleManager::getInstance();
    battleManager->clear(); // 重置战斗管理器状态
    this->unscheduleUpdate(); // 停止更新
    Director::getInstance()->replaceScene(HelloWorld::createScene());
}

