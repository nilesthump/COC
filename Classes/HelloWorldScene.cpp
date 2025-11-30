#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    auto label = Label::createWithTTF("Hello Mimi--From NIHAO", "fonts/Marker Felt.ttf", 36);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("mimi.png");
    if (sprite == nullptr)
    {
        problemLoading("'mimi.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

        sprite->setScale(0.25);

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }

    auto head_pic_left = Sprite::create("headpic.png");
    if (head_pic_left == nullptr)
    {
        problemLoading("headpic.png");
    }
    else
    {
        head_pic_left->setPosition(Vec2(origin.x + 150, origin.y + visibleSize.height - label->getContentSize().height - 150));

        head_pic_left->setRotation(-45.0);

        head_pic_left->setOpacity(32);

        head_pic_left->setScale(0.25);

        this->addChild(head_pic_left, -1);

        auto moveby = MoveBy::create(10, Vec2(50, 50));
        auto fadein = FadeIn::create(10.0);
        auto rotation = RotateBy::create(10, 45.0);
        auto spawn = Spawn::create(moveby, fadein, rotation, nullptr);
        auto seq = Sequence::create(spawn, spawn->reverse(), nullptr);
        head_pic_left->runAction(seq);

    }

    auto head_pic_right = Sprite::create("headpic.png");
    if (head_pic_left == nullptr)
    {
        problemLoading("headpic.png");
    }
    else
    {
        head_pic_right->setPosition(Vec2(origin.x + visibleSize.width -
            150, origin.y + visibleSize.height - label->getContentSize().height - 150));

        head_pic_right->setRotation(45.0);

        head_pic_right->setOpacity(32);

        head_pic_right->setScale(0.25);

        this->addChild(head_pic_right, -1);
    }

    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
