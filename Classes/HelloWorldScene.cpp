#include "HelloWorldScene.h"
#include "SecondScene.h"
#include "BattleTestLayer.h"

USING_NS_CC;

// Initialize static login status variable
bool HelloWorld::isLoggedIn = false;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}


static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
   
    if (!Scene::init())
    {
        return false;
    }

  
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

   
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
        closeItem->setPosition(Vec2(x, y));
    }


    secondSceneItem = MenuItemImage::create(
        "btn_normal.png",      
        "btn_pressed.png",     
        CC_CALLBACK_1(HelloWorld::menuSecondSceneCallback, this));

    if (secondSceneItem == nullptr ||
        secondSceneItem->getContentSize().width <= 0 ||
        secondSceneItem->getContentSize().height <= 0)
    {
        problemLoading("'btn_normal.png' and 'btn_pressed.png'");
    }
    else
    {

        double x = origin.x + visibleSize.width - secondSceneItem->getContentSize().width / 2;
        double y = origin.y + visibleSize.height - secondSceneItem->getContentSize().height / 2;
        secondSceneItem->setPosition(Vec2(x, y));

       
        auto secondLabel = Label::createWithSystemFont("START GAME", "fonts/Marker Felt.ttf", 24);
        secondLabel->setColor(Color3B::WHITE);
        secondLabel->setPosition(Vec2(secondSceneItem->getContentSize().width / 2,
            secondSceneItem->getContentSize().height / 2));
        secondSceneItem->addChild(secondLabel);
    }

    battleTestItem = MenuItemImage::create(
        "btn_normal.png",
        "btn_pressed.png",
        CC_CALLBACK_1(HelloWorld::menuBattleTestCallback, this));

    if (battleTestItem == nullptr ||
        battleTestItem->getContentSize().width <= 0 ||
        battleTestItem->getContentSize().height <= 0)
    {
        problemLoading("'btn_normal.png' and 'btn_pressed.png'");
    }
    else
    {

        double x = origin.x + visibleSize.width - battleTestItem->getContentSize().width/2;
        double y = origin.y + visibleSize.height - secondSceneItem->getContentSize().height - battleTestItem->getContentSize().height / 2;
        battleTestItem->setPosition(Vec2(x, y));


        auto testLabel = Label::createWithSystemFont("TEST GAME", "fonts/Marker Felt.ttf", 24);
        testLabel->setColor(Color3B::WHITE);
        testLabel->setPosition(Vec2(battleTestItem->getContentSize().width / 2,
            battleTestItem->getContentSize().height / 2));
        battleTestItem->addChild(testLabel);
    }


    // Create login button
    loginItem = MenuItemImage::create(
        "btn_normal.png",
        "btn_pressed.png",
        CC_CALLBACK_1(HelloWorld::menuLoginCallback, this));

    if (loginItem == nullptr ||
        loginItem->getContentSize().width <= 0 ||
        loginItem->getContentSize().height <= 0)
    {
        problemLoading("'btn_normal.png' and 'btn_pressed.png'");
    }
    else
    {
        double x = origin.x + visibleSize.width / 2;
        double y = origin.y + visibleSize.height / 4;
        loginItem->setPosition(Vec2(x, y));

        loginLabel = Label::createWithSystemFont("LOGIN", "fonts/Marker Felt.ttf", 24);
        loginLabel->setColor(Color3B::WHITE);
        loginLabel->setPosition(Vec2(loginItem->getContentSize().width / 2,
            loginItem->getContentSize().height / 2));
        loginItem->addChild(loginLabel);
    }

    auto menu = Menu::create(closeItem, secondSceneItem, battleTestItem, loginItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // Initialize login UI components
    loginLayer = nullptr;
    logoutConfirmLayer = nullptr;
    usernameEditBox = nullptr;
    passwordEditBox = nullptr;
    confirmItem = nullptr;
    confirmLogoutItem = nullptr;
    cancelLogoutItem = nullptr;

    // Initially hide and disable secondSceneItem and battleTestItem
    if (isLoggedIn)
    {
        // Logged in state
        if (secondSceneItem != nullptr)
        {
            secondSceneItem->setVisible(true);
            secondSceneItem->setEnabled(true);
        }

        if (battleTestItem != nullptr)
        {
            battleTestItem->setVisible(true);
            battleTestItem->setEnabled(true);
        }

        // Change login button to logout button
        loginItem->setCallback(CC_CALLBACK_1(HelloWorld::menuLogoutCallback, this));
        loginLabel->setString("LOGOUT");
    }
    else
    {
        // Not logged in state
        if (secondSceneItem != nullptr)
        {
            secondSceneItem->setVisible(false);
            secondSceneItem->setEnabled(false);
        }

        if (battleTestItem != nullptr)
        {
            battleTestItem->setVisible(false);
            battleTestItem->setEnabled(false);
        }

        // Ensure login button is in login state
        loginItem->setCallback(CC_CALLBACK_1(HelloWorld::menuLoginCallback, this));
        loginLabel->setString("LOGIN");
    }
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

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("LoadingScene.jpg");
    if (sprite == nullptr)
    {
        problemLoading("'LoadingScene.jpg'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

        sprite->setScale(1.0);

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }
#if 0
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

    Director::getInstance()->replaceScene(SecondScene::createScene());
}

void HelloWorld::menuBattleTestCallback(Ref* pSender)
{

    Director::getInstance()->replaceScene(BattleTestLayer::createScene());
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

void HelloWorld::menuLoginCallback(cocos2d::Ref* pSender)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Create a semi-transparent background layer
    if (loginLayer == nullptr)
    {
        loginLayer = LayerColor::create(Color4B(0, 0, 0, 180));
        this->addChild(loginLayer, 10);

        // Create username label
        usernameLabel = Label::createWithSystemFont("Account:", "fonts/Marker Felt.ttf", 20);
        usernameLabel->setColor(Color3B::WHITE);
        usernameLabel->setPosition(Vec2(origin.x + visibleSize.width / 2 - 150,
            origin.y + visibleSize.height / 2 + 50));
        loginLayer->addChild(usernameLabel);

        // Create username edit box
        usernameEditBox = ui::EditBox::create(Size(300, 40), "btn_normal.png");
        usernameEditBox->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height / 2 + 50));
        usernameEditBox->setPlaceholderFontName("fonts/Marker Felt.ttf");
        usernameEditBox->setPlaceholderFontSize(20);
        usernameEditBox->setPlaceHolder("Enter your account name");
        usernameEditBox->setFontName("fonts/Marker Felt.ttf");
        usernameEditBox->setFontSize(20);
        usernameEditBox->setFontColor(Color3B::WHITE);
        usernameEditBox->setReturnType(ui::EditBox::KeyboardReturnType::NEXT);
        usernameEditBox->setDelegate(this);
        loginLayer->addChild(usernameEditBox);

        // Create password label
        passwordLabel = Label::createWithSystemFont("Password:", "fonts/Marker Felt.ttf", 20);
        passwordLabel->setColor(Color3B::WHITE);
        passwordLabel->setPosition(Vec2(origin.x + visibleSize.width / 2 - 150,
            origin.y + visibleSize.height / 2 - 20));
        loginLayer->addChild(passwordLabel);

        // Create password edit box
        passwordEditBox = ui::EditBox::create(Size(300, 40), "btn_normal.png");
        passwordEditBox->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height / 2 - 20));
        passwordEditBox->setPlaceholderFontName("fonts/Marker Felt.ttf");
        passwordEditBox->setPlaceholderFontSize(20);
        passwordEditBox->setPlaceHolder("Enter your password");
        passwordEditBox->setFontName("fonts/Marker Felt.ttf");
        passwordEditBox->setFontSize(20);
        passwordEditBox->setFontColor(Color3B::WHITE);
        passwordEditBox->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
        passwordEditBox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
        passwordEditBox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
        passwordEditBox->setDelegate(this);
        loginLayer->addChild(passwordEditBox);

        // Create confirm button
        confirmItem = MenuItemImage::create(
            "btn_normal.png",
            "btn_pressed.png",
            CC_CALLBACK_1(HelloWorld::menuConfirmCallback, this));

        if (confirmItem != nullptr &&
            confirmItem->getContentSize().width > 0 &&
            confirmItem->getContentSize().height > 0)
        {
            double x = origin.x + visibleSize.width / 2;
            double y = origin.y + visibleSize.height / 2 - 90;
            confirmItem->setPosition(Vec2(x, y));

            auto confirmLabel = Label::createWithSystemFont("Confirm", "fonts/Marker Felt.ttf", 24);
            confirmLabel->setColor(Color3B::WHITE);
            confirmLabel->setPosition(Vec2(confirmItem->getContentSize().width / 2,
                confirmItem->getContentSize().height / 2));
            confirmItem->addChild(confirmLabel);

            auto confirmMenu = Menu::create(confirmItem, NULL);
            confirmMenu->setPosition(Vec2::ZERO);
            loginLayer->addChild(confirmMenu);
        }
    }
    else
    {
        // Show the existing login layer if it was already created
        loginLayer->setVisible(true);
    }
}

void HelloWorld::menuConfirmCallback(cocos2d::Ref* pSender)
{
    // Hide login layer
    if (loginLayer != nullptr)
    {
        loginLayer->setVisible(false);
    }

    // Show secondSceneItem and battleTestItem
    if (secondSceneItem != nullptr)
    {
        secondSceneItem->setVisible(true);
        secondSceneItem->setEnabled(true);
    }

    if (battleTestItem != nullptr)
    {
        battleTestItem->setVisible(true);
        battleTestItem->setEnabled(true);
    }

    // Update login status
    isLoggedIn = true;

    // Change login button to logout button
    loginItem->setCallback(CC_CALLBACK_1(HelloWorld::menuLogoutCallback, this));
    loginLabel->setString("LOGOUT");
}

// EditBox delegate functions implementation
void HelloWorld::editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox)
{
    // Handle edit box begin editing
}

void HelloWorld::editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text)
{
    // Handle edit box text change
}

void HelloWorld::editBoxReturn(cocos2d::ui::EditBox* editBox)
{
    // Handle edit box return key pressed
}

void HelloWorld::editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox, cocos2d::ui::EditBoxDelegate::EditBoxEndAction action)
{
    // Handle edit box end editing with action
}

void HelloWorld::menuLogoutCallback(cocos2d::Ref* pSender)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Create a semi-transparent background layer for confirmation dialog
    if (logoutConfirmLayer == nullptr)
    {
        logoutConfirmLayer = LayerColor::create(Color4B(0, 0, 0, 180));
        this->addChild(logoutConfirmLayer, 10);

        // Create confirm logout button
        confirmLogoutItem = MenuItemImage::create(
            "btn_normal.png",
            "btn_pressed.png",
            CC_CALLBACK_1(HelloWorld::menuConfirmLogoutCallback, this));

        if (confirmLogoutItem != nullptr &&
            confirmLogoutItem->getContentSize().width > 0 &&
            confirmLogoutItem->getContentSize().height > 0)
        {
            double x = origin.x + visibleSize.width / 2 - 120;
            double y = origin.y + visibleSize.height / 2;
            confirmLogoutItem->setPosition(Vec2(x, y));

            auto confirmLabel = Label::createWithSystemFont("Confirm", "fonts/Marker Felt.ttf", 24);
            confirmLabel->setColor(Color3B::WHITE);
            confirmLabel->setPosition(Vec2(confirmLogoutItem->getContentSize().width / 2,
                confirmLogoutItem->getContentSize().height / 2));
            confirmLogoutItem->addChild(confirmLabel);
        }

        // Create cancel logout button
        cancelLogoutItem = MenuItemImage::create(
            "btn_normal.png",
            "btn_pressed.png",
            CC_CALLBACK_1(HelloWorld::menuCancelLogoutCallback, this));

        if (cancelLogoutItem != nullptr &&
            cancelLogoutItem->getContentSize().width > 0 &&
            cancelLogoutItem->getContentSize().height > 0)
        {
            double x = origin.x + visibleSize.width / 2 + 120;
            double y = origin.y + visibleSize.height / 2;
            cancelLogoutItem->setPosition(Vec2(x, y));

            auto cancelLabel = Label::createWithSystemFont("Cancel", "fonts/Marker Felt.ttf", 24);
            cancelLabel->setColor(Color3B::WHITE);
            cancelLabel->setPosition(Vec2(cancelLogoutItem->getContentSize().width / 2,
                cancelLogoutItem->getContentSize().height / 2));
            cancelLogoutItem->addChild(cancelLabel);
        }

        // Create menu for the logout confirmation buttons
        auto logoutConfirmMenu = Menu::create(confirmLogoutItem, cancelLogoutItem, NULL);
        logoutConfirmMenu->setPosition(Vec2::ZERO);
        logoutConfirmLayer->addChild(logoutConfirmMenu);
    }
    else
    {
        // Show the existing logout confirmation layer if it was already created
        logoutConfirmLayer->setVisible(true);
    }
}

void HelloWorld::menuConfirmLogoutCallback(cocos2d::Ref* pSender)
{
    // Hide logout confirmation layer
    if (logoutConfirmLayer != nullptr)
    {
        logoutConfirmLayer->setVisible(false);
    }

    // Change logout button back to login button
    loginItem->setCallback(CC_CALLBACK_1(HelloWorld::menuLoginCallback, this));
    loginLabel->setString("LOGIN");

    // Hide and disable secondSceneItem and battleTestItem
    if (secondSceneItem != nullptr)
    {
        secondSceneItem->setVisible(false);
        secondSceneItem->setEnabled(false);
    }

    if (battleTestItem != nullptr)
    {
        battleTestItem->setVisible(false);
        battleTestItem->setEnabled(false);
    }

    // Update login status
    isLoggedIn = false;
}

void HelloWorld::menuCancelLogoutCallback(cocos2d::Ref* pSender)
{
    // Hide logout confirmation layer and keep login status
    if (logoutConfirmLayer != nullptr)
    {
        logoutConfirmLayer->setVisible(false);
    }
}
