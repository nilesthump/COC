#include "HelloWorldScene.h"
#include "SecondScene.h"

USING_NS_CC;

//于12.10muMukefu渡劫成功
//太不容易了（哭）
//老天爷下次别这样对这个可怜的娃了
//最后试一次
//再来一次
Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

//报错提示
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //报错
    if (!Scene::init())
    {
        return false;
    }

    //可视区域大小和原点，用来确定UI元素绝对位置
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    //退出游戏按钮设置
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
        double x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
        double y = origin.y + closeItem->getContentSize().height / 2;
        closeItem->setPosition(Vec2(x, y));//设置图标位置
    }

    // ========== 添加第二个按钮：跳转到 SecondScene ==========
    auto secondSceneItem = MenuItemImage::create(
        "btn_normal.png",      // 正常状态图片
        "btn_pressed.png",     // 按下状态图片
        CC_CALLBACK_1(HelloWorld::menuSecondSceneCallback, this));

    if (secondSceneItem == nullptr ||
        secondSceneItem->getContentSize().width <= 0 ||
        secondSceneItem->getContentSize().height <= 0)
    {
        problemLoading("'btn_normal.png' and 'btn_pressed.png'");
    }
    else
    {
        // 设置位置（屏幕右上）
        double x = origin.x + visibleSize.width - secondSceneItem->getContentSize().width / 2;
        double y = origin.y + visibleSize.height - secondSceneItem->getContentSize().height / 2;
        secondSceneItem->setPosition(Vec2(x, y));

        // 可选：在按钮上添加文字
        auto secondLabel = Label::createWithSystemFont("START GAME", "fonts/Marker Felt.ttf", 24);
        secondLabel->setColor(Color3B::WHITE);
        secondLabel->setPosition(Vec2(secondSceneItem->getContentSize().width / 2,
            secondSceneItem->getContentSize().height / 2));//相对于框的位置
        secondSceneItem->addChild(secondLabel);
    }

    //菜单
    auto menu = Menu::create(closeItem, secondSceneItem, NULL);  // 添加第二个按钮到菜单
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    auto label = Label::createWithTTF("Hello Mimi--From yzlgai", "fonts/Marker Felt.ttf", 36);
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
#if 0
    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("mimi.png");
    if (sprite == nullptr)
    {
        problemLoading("'mimi.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

        sprite->setScale(0.5);

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
#endif
    return true;
}

void HelloWorld::menuSecondSceneCallback(Ref* pSender)
{
    // 跳转到 SecondScene
    Director::getInstance()->replaceScene(SecondScene::createScene());
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
