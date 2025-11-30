#if 1
#include "SecondScene.h"
#include "HelloWorldScene.h"

USING_NS_CC;

Scene* SecondScene::createScene()
{
	return SecondScene::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool SecondScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    // 添加返回按钮
    auto backItem = MenuItemImage::create("btn_normal.png", "btn_pressed.png",
        CC_CALLBACK_1(SecondScene::menuFirstCallback, this));
    if (backItem)
    {
        backItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

        auto backLabel = Label::createWithSystemFont("return", "Arial", 20);
        backLabel->setColor(Color3B::WHITE);
        backLabel->setPosition(Vec2(backItem->getContentSize().width / 2, backItem->getContentSize().height / 2));
        backItem->addChild(backLabel);

        auto menu = Menu::create(backItem, nullptr);
        menu->setPosition(Vec2::ZERO);
        this->addChild(menu, 1);
    }

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    auto label = Label::createWithTTF("Hello!!!!!!!!!", "fonts/Marker Felt.ttf", 36);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("mainscene.png");
    if (sprite == nullptr)
    {
        problemLoading("'mainscene.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

        sprite->setScale(2);

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }

    return true;
}

void SecondScene::menuFirstCallback(Ref* pSender)
{
    // 返回主场景
    Director::getInstance()->replaceScene(HelloWorld::createScene());
}

#endif