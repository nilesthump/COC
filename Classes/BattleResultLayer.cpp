#include "BattleResultLayer.h"
#include "HelloWorldScene.h"
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

    std::string title;
    Color3B color;

    switch (result)
    {
    case BattleResult::ATTACKERS_WIN:
        title = "VICTORY";
        color = Color3B::GREEN;
        break;
    case BattleResult::DEFENDERS_WIN:
        title = "DEFEAT";
        color = Color3B::RED;
        break;
    case BattleResult::TIME_UP:
        title = "TIME UP";
        color = Color3B::YELLOW;
        break;
    default:
        title = "BATTLE END";
        color = Color3B::WHITE;
        break;
    }

    auto label = Label::createWithSystemFont(title, "Arial", 64);
    label->setColor(color);
    label->setPosition(size.width / 2, size.height / 2 + 80);
    this->addChild(label);

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
    Director::getInstance()->replaceScene(HelloWorld::createScene());
}

