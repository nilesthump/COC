#include "HelloWorldScene.h"
#include "SecondScene.h"
#include "BattleTestLayer.h"
#include "SQLiteTest.h"
#include "SessionManager.h"
#include "WebSocketManager.h"
#include "json/document.h"

USING_NS_CC;

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

        double x = origin.x + visibleSize.width - battleTestItem->getContentSize().width / 2;
        double y = origin.y + visibleSize.height - secondSceneItem->getContentSize().height - battleTestItem->getContentSize().height / 2;
        battleTestItem->setPosition(Vec2(x, y));


        auto testLabel = Label::createWithSystemFont("TEST GAME", "fonts/Marker Felt.ttf", 24);
        testLabel->setColor(Color3B::WHITE);
        testLabel->setPosition(Vec2(battleTestItem->getContentSize().width / 2,
            battleTestItem->getContentSize().height / 2));
        battleTestItem->addChild(testLabel);
    }

    // Create guest login button
    guestLoginItem = MenuItemImage::create(
        "btn_normal.png",
        "btn_pressed.png",
        CC_CALLBACK_1(HelloWorld::menuGuestLoginCallback, this));

    if (guestLoginItem == nullptr ||
        guestLoginItem->getContentSize().width <= 0 ||
        guestLoginItem->getContentSize().height <= 0)
    {
        problemLoading("'btn_normal.png' and 'btn_pressed.png'");
    }
    else
    {
        double guestLoginX = origin.x + visibleSize.width / 2 - 200; // Leftmost position
        double y = origin.y + visibleSize.height / 4;
        guestLoginItem->setPosition(Vec2(guestLoginX, y));

        guestLoginLabel = Label::createWithSystemFont("GUEST LOGIN", "fonts/Marker Felt.ttf", 24);
        guestLoginLabel->setColor(Color3B::WHITE);
        guestLoginLabel->setPosition(Vec2(guestLoginItem->getContentSize().width / 2,
            guestLoginItem->getContentSize().height / 2));
        guestLoginItem->addChild(guestLoginLabel);
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
        double loginX = origin.x + visibleSize.width / 2; // Center position
        double y = origin.y + visibleSize.height / 4;
        loginItem->setPosition(Vec2(loginX, y));

        loginLabel = Label::createWithSystemFont("LOGIN", "fonts/Marker Felt.ttf", 24);
        loginLabel->setColor(Color3B::WHITE);
        loginLabel->setPosition(Vec2(loginItem->getContentSize().width / 2,
            loginItem->getContentSize().height / 2));
        loginItem->addChild(loginLabel);
    }

    // Create register button
    registerItem = MenuItemImage::create(
        "btn_normal.png",
        "btn_pressed.png",
        CC_CALLBACK_1(HelloWorld::menuRegisterCallback, this));

    if (registerItem == nullptr ||
        registerItem->getContentSize().width <= 0 ||
        registerItem->getContentSize().height <= 0)
    {
        problemLoading("'btn_normal.png' and 'btn_pressed.png'");
    }
    else
    {
        double registerX = origin.x + visibleSize.width / 2 + 200; // Rightmost position, symmetric to guest login button
        double y = origin.y + visibleSize.height / 4; // Same height as login button
        registerItem->setPosition(Vec2(registerX, y));

        registerLabel = Label::createWithSystemFont("REGISTER", "fonts/Marker Felt.ttf", 24);
        registerLabel->setColor(Color3B::WHITE);
        registerLabel->setPosition(Vec2(registerItem->getContentSize().width / 2,
            registerItem->getContentSize().height / 2));
        registerItem->addChild(registerLabel);
    }

    // Create delete account button
    deleteAccountItem = MenuItemImage::create(
        "btn_normal.png",
        "btn_pressed.png",
        CC_CALLBACK_1(HelloWorld::menuDeleteAccountCallback, this));

    if (deleteAccountItem == nullptr ||
        deleteAccountItem->getContentSize().width <= 0 ||
        deleteAccountItem->getContentSize().height <= 0)
    {
        problemLoading("'btn_normal.png' and 'btn_pressed.png'");
    }
    else
    {
        // Position at bottom left corner
        double x = origin.x + deleteAccountItem->getContentSize().width / 2 + 20;
        double y = origin.y + deleteAccountItem->getContentSize().height / 2 + 20;
        deleteAccountItem->setPosition(Vec2(x, y));

        deleteAccountLabel = Label::createWithSystemFont("DELETE ACCOUNT", "fonts/Marker Felt.ttf", 18);
        deleteAccountLabel->setColor(Color3B::WHITE);
        deleteAccountLabel->setPosition(Vec2(deleteAccountItem->getContentSize().width / 2,
            deleteAccountItem->getContentSize().height / 2));
        deleteAccountItem->addChild(deleteAccountLabel);
    }

    // Create change password button
    changePasswordItem = MenuItemImage::create(
        "btn_normal.png",
        "btn_pressed.png",
        CC_CALLBACK_1(HelloWorld::menuChangePasswordCallback, this));

    if (changePasswordItem == nullptr ||
        changePasswordItem->getContentSize().width <= 0 ||
        changePasswordItem->getContentSize().height <= 0)
    {
        problemLoading("'btn_normal.png' and 'btn_pressed.png'");
    }
    else
    {
        // Position at the same height as login/logout button, to the right
        double x = origin.x + visibleSize.width / 2 + 200; // Right of login/logout button
        double y = origin.y + visibleSize.height / 4;
        changePasswordItem->setPosition(Vec2(x, y));

        auto changePasswordLabel = Label::createWithSystemFont("CHANGE PASSWORD", "fonts/Marker Felt.ttf", 24);
        changePasswordLabel->setColor(Color3B::WHITE);
        changePasswordLabel->setPosition(Vec2(changePasswordItem->getContentSize().width / 2,
            changePasswordItem->getContentSize().height / 2));
        changePasswordItem->addChild(changePasswordLabel);
    }

    auto menu = Menu::create(closeItem, secondSceneItem, battleTestItem,
        guestLoginItem, loginItem, registerItem, deleteAccountItem, 
        changePasswordItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // Initialize login UI components
    loginLayer = nullptr;
    registerLayer = nullptr;
    logoutConfirmLayer = nullptr;
    deleteAccountConfirmLayer = nullptr;
    changePasswordLayer = nullptr;

    // Initialize login specific edit boxes
    loginUsernameEditBox = nullptr;
    loginPasswordEditBox = nullptr;

    // Initialize register specific edit boxes
    registerUsernameEditBox = nullptr;
    registerPasswordEditBox = nullptr;
    confirmPasswordEditBox = nullptr;
    confirmItem = nullptr;
    registerConfirmItem = nullptr;

    // Initialize change password specific edit boxes
    oldPasswordEditBox = nullptr;
    newPasswordEditBox = nullptr;
    confirmNewPasswordEditBox = nullptr;
    changePasswordConfirmItem = nullptr;
    cancelChangePasswordItem = nullptr;

    confirmLogoutItem = nullptr;
    cancelLogoutItem = nullptr;

    confirmDeleteItem = nullptr;
    cancelDeleteItem = nullptr;

    usernameLabel = nullptr;
    passwordLabel = nullptr;

    confirmPasswordLabel = nullptr;
    registerResultLabel = nullptr;

    // Initialize auto-connection state
    _isConnecting = false;
    _connectionTimeoutScheduled = false;
    _isReconnecting = false;
    _serverUrl = "ws://100.80.250.106:8080";

    // Initially hide and disable secondSceneItem and battleTestItem
    auto session = SessionManager::getInstance();
    if (session->getIsLoggedIn())
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

        // Hide guest login and register buttons
        if (guestLoginItem != nullptr)
        {
            guestLoginItem->setVisible(false);
            guestLoginItem->setEnabled(false);
            // Also hide the label to be extra thorough
            if (guestLoginLabel != nullptr)
            {
                guestLoginLabel->setVisible(false);
            }
        }
        if (registerItem != nullptr)
        {
            registerItem->setVisible(false);
            registerItem->setEnabled(false);
            // Also hide the label to be extra thorough
            if (registerLabel != nullptr)
            {
                registerLabel->setVisible(false);
            }
        }

        // Change login button to logout button
        loginItem->setCallback(CC_CALLBACK_1(HelloWorld::menuLogoutCallback, this));
        loginLabel->setString("LOGOUT");

        // Show delete account button,welcome label and change password button only when logged in with username/password
        std::string username = session->getCurrentUsername();
        if (deleteAccountItem != nullptr)
        {
            deleteAccountItem->setVisible(username.empty() ? false : true);
            deleteAccountItem->setEnabled(username.empty() ? false : true);
        }
        if (welcomeLabel != nullptr) 
        {
            welcomeLabel->setString(username.empty() ? "Welcome!" : "Welcome " + username + "!");
            welcomeLabel->setVisible(true);
        }
        if (changePasswordItem != nullptr)
        {
            changePasswordItem->setVisible(username.empty() ? false : true);
            changePasswordItem->setEnabled(username.empty() ? false : true);
        }
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

        // Show guest login and register buttons
        if (guestLoginItem != nullptr)
        {
            guestLoginItem->setVisible(true);
            guestLoginItem->setEnabled(true);
            // Also show the label to be extra thorough
            if (guestLoginLabel != nullptr)
            {
                guestLoginLabel->setVisible(true);
            }
        }
        if (registerItem != nullptr)
        {
            registerItem->setVisible(true);
            registerItem->setEnabled(true);
            // Also show the label to be extra thorough
            if (registerLabel != nullptr)
            {
                registerLabel->setVisible(true);
            }
        }

        // Ensure login button is in login state
        loginItem->setCallback(CC_CALLBACK_1(HelloWorld::menuLoginCallback, this));
        loginLabel->setString("LOGIN");
        // Show login button that was hidden during delete confirmation
        loginItem->setVisible(true);
        loginItem->setEnabled(true);
        loginLabel->setVisible(true);

        // Hide delete account and change password buttons when not logged in
        if (deleteAccountItem != nullptr)
        {
            deleteAccountItem->setVisible(false);
            deleteAccountItem->setEnabled(false);
        }

        if (changePasswordItem != nullptr)
        {
            changePasswordItem->setVisible(false);
            changePasswordItem->setEnabled(false);
        }
    }
    /////////////////////////////
    // 3. add your codes below...
    // add a label shows "Hello World"
    // create and initialize a label
    // Create welcome label (initially hidden)
    welcomeLabel = Label::createWithTTF("", "fonts/Marker Felt.ttf", 36);
    if (welcomeLabel == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        welcomeLabel->setColor(Color3B::GREEN);
        welcomeLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height - 36));
        welcomeLabel->setVisible(false);
        this->addChild(welcomeLabel, 1);
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

    setupWebSocketCallbacks();

    return true;
}

void HelloWorld::menuDeleteAccountCallback(cocos2d::Ref* pSender)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Create a semi-transparent background layer for confirmation dialog
    if (deleteAccountConfirmLayer == nullptr)
    {
        deleteAccountConfirmLayer = LayerColor::create(Color4B(0, 0, 0, 180));
        this->addChild(deleteAccountConfirmLayer, 10);

        // Create confirm delete button
        confirmDeleteItem = MenuItemImage::create(
            "btn_normal.png",
            "btn_pressed.png",
            CC_CALLBACK_1(HelloWorld::menuConfirmDeleteCallback, this));

        if (confirmDeleteItem != nullptr &&
            confirmDeleteItem->getContentSize().width > 0 &&
            confirmDeleteItem->getContentSize().height > 0)
        {
            double x = origin.x + visibleSize.width / 2 - 120;
            double y = origin.y + visibleSize.height / 2;
            confirmDeleteItem->setPosition(Vec2(x, y));

            auto confirmLabel = Label::createWithSystemFont("Confirm Delete", "fonts/Marker Felt.ttf", 24);
            confirmLabel->setColor(Color3B::WHITE);
            confirmLabel->setPosition(Vec2(confirmDeleteItem->getContentSize().width / 2,
                confirmDeleteItem->getContentSize().height / 2));
            confirmDeleteItem->addChild(confirmLabel);
        }

        // Create cancel delete button
        cancelDeleteItem = MenuItemImage::create(
            "btn_normal.png",
            "btn_pressed.png",
            CC_CALLBACK_1(HelloWorld::menuCancelDeleteCallback, this));

        if (cancelDeleteItem != nullptr &&
            cancelDeleteItem->getContentSize().width > 0 &&
            cancelDeleteItem->getContentSize().height > 0)
        {
            double x = origin.x + visibleSize.width / 2 + 120;
            double y = origin.y + visibleSize.height / 2;
            cancelDeleteItem->setPosition(Vec2(x, y));

            auto cancelLabel = Label::createWithSystemFont("Cancel", "fonts/Marker Felt.ttf", 24);
            cancelLabel->setColor(Color3B::WHITE);
            cancelLabel->setPosition(Vec2(cancelDeleteItem->getContentSize().width / 2,
                cancelDeleteItem->getContentSize().height / 2));
            cancelDeleteItem->addChild(cancelLabel);
        }

        // Create menu with the buttons
        auto deleteMenu = Menu::create(confirmDeleteItem, cancelDeleteItem, NULL);
        deleteMenu->setPosition(Vec2::ZERO);
        deleteAccountConfirmLayer->addChild(deleteMenu);

        // Add a label to ask for confirmation
        auto confirmLabel = Label::createWithSystemFont("Are you sure you want to delete your account?", "fonts/Marker Felt.ttf", 24);
        confirmLabel->setColor(Color3B::WHITE);
        confirmLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 + 60));
        deleteAccountConfirmLayer->addChild(confirmLabel);
    }
    else
    {
        deleteAccountConfirmLayer->setVisible(true);
    }


    // Hide all scene buttons except sub-buttons
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
    if (deleteAccountItem != nullptr)
    {
        deleteAccountItem->setVisible(false);
        deleteAccountItem->setEnabled(false);
    }
    if (loginItem != nullptr)
    {
        loginItem->setVisible(false);
        loginItem->setEnabled(false);
        if (loginLabel != nullptr)
        {
            loginLabel->setVisible(false);
        }
    }
}

void HelloWorld::menuConfirmDeleteCallback(cocos2d::Ref* pSender)
{
    // Hide delete confirmation layer
    if (deleteAccountConfirmLayer != nullptr)
    {
        deleteAccountConfirmLayer->setVisible(false);
    }

    // Get current username from session
    auto session = SessionManager::getInstance();
    std::string username = session->getCurrentUsername();
    if (!username.empty())
    {
        // Send delete request via WebSocket
        sendDeleteRequest(username);
    }
}

void HelloWorld::menuCancelDeleteCallback(cocos2d::Ref* pSender)
{
    // Hide delete confirmation layer and keep login status
    if (deleteAccountConfirmLayer != nullptr)
    {
        deleteAccountConfirmLayer->setVisible(false);
    }

    // Show all scene buttons that were hidden
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
    if (deleteAccountItem != nullptr)
    {
        deleteAccountItem->setVisible(true);
        deleteAccountItem->setEnabled(true);
    }
    if (loginItem != nullptr)
    {
        loginItem->setVisible(true);
        loginItem->setEnabled(true);
        if (loginLabel != nullptr)
        {
            loginLabel->setVisible(true);
        }
    }
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
        usernameLabel->setPosition(Vec2(origin.x + visibleSize.width / 2 - 200,
            origin.y + visibleSize.height / 2 + 50));
        loginLayer->addChild(usernameLabel);

        // Create username edit box
        loginUsernameEditBox = ui::EditBox::create(Size(300, 40), "btn_normal.png");
        loginUsernameEditBox->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height / 2 + 50));
        loginUsernameEditBox->setPlaceholderFontName("fonts/Marker Felt.ttf");
        loginUsernameEditBox->setPlaceholderFontSize(20);
        loginUsernameEditBox->setPlaceHolder("Enter your account name");
        loginUsernameEditBox->setFontName("fonts/Marker Felt.ttf");
        loginUsernameEditBox->setFontSize(20);
        loginUsernameEditBox->setFontColor(Color3B::WHITE);
        loginUsernameEditBox->setReturnType(ui::EditBox::KeyboardReturnType::NEXT);
        loginUsernameEditBox->setDelegate(this);
        loginLayer->addChild(loginUsernameEditBox);

        // Create password label
        passwordLabel = Label::createWithSystemFont("Password:", "fonts/Marker Felt.ttf", 20);
        passwordLabel->setColor(Color3B::WHITE);
        passwordLabel->setPosition(Vec2(origin.x + visibleSize.width / 2 - 200,
            origin.y + visibleSize.height / 2 - 20));
        loginLayer->addChild(passwordLabel);

        // Clear input fields when first creating login layer
        if (loginUsernameEditBox != nullptr)
        {
            loginUsernameEditBox->setText("");
        }
        if (loginPasswordEditBox != nullptr)
        {
            loginPasswordEditBox->setText("");
        }

        // Create password edit box
        loginPasswordEditBox = ui::EditBox::create(Size(300, 40), "btn_normal.png");
        loginPasswordEditBox->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height / 2 - 20));
        loginPasswordEditBox->setPlaceholderFontName("fonts/Marker Felt.ttf");
        loginPasswordEditBox->setPlaceholderFontSize(20);
        loginPasswordEditBox->setPlaceHolder("Enter your password");
        loginPasswordEditBox->setFontName("fonts/Marker Felt.ttf");
        loginPasswordEditBox->setFontSize(20);
        loginPasswordEditBox->setFontColor(Color3B::WHITE);
        loginPasswordEditBox->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
        loginPasswordEditBox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
        loginPasswordEditBox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
        loginPasswordEditBox->setDelegate(this);
        loginLayer->addChild(loginPasswordEditBox);

        // Create confirm button
        confirmItem = MenuItemImage::create(
            "btn_normal.png",
            "btn_pressed.png",
            CC_CALLBACK_1(HelloWorld::menuConfirmCallback, this));

        // Create cancel login button
        cancelLoginItem = MenuItemImage::create(
            "btn_normal.png",
            "btn_pressed.png",
            CC_CALLBACK_1(HelloWorld::menuCancelLoginCallback, this));

        if (confirmItem != nullptr &&
            confirmItem->getContentSize().width > 0 &&
            confirmItem->getContentSize().height > 0 &&
            cancelLoginItem != nullptr &&
            cancelLoginItem->getContentSize().width > 0 &&
            cancelLoginItem->getContentSize().height > 0)
        {
            // Set confirm button position (left of center)
            double confirmX = origin.x + visibleSize.width / 2 - 120;
            double buttonY = origin.y + visibleSize.height / 2 - 90;
            confirmItem->setPosition(Vec2(confirmX, buttonY));

            // Set cancel login button position (right of center, symmetric to confirm button)
            double cancelX = origin.x + visibleSize.width / 2 + 120;
            cancelLoginItem->setPosition(Vec2(cancelX, buttonY));

            // Add label to confirm button
            auto confirmLabel = Label::createWithSystemFont("Confirm", "fonts/Marker Felt.ttf", 24);
            confirmLabel->setColor(Color3B::WHITE);
            confirmLabel->setPosition(Vec2(confirmItem->getContentSize().width / 2,
                confirmItem->getContentSize().height / 2));
            confirmItem->addChild(confirmLabel);

            // Add label to cancel login button
            auto cancelLabel = Label::createWithSystemFont("Cancel", "fonts/Marker Felt.ttf", 24);
            cancelLabel->setColor(Color3B::WHITE);
            cancelLabel->setPosition(Vec2(cancelLoginItem->getContentSize().width / 2,
                cancelLoginItem->getContentSize().height / 2));
            cancelLoginItem->addChild(cancelLabel);

            // Create a menu with both buttons
            auto loginMenu = Menu::create(confirmItem, cancelLoginItem, NULL);
            loginMenu->setPosition(Vec2::ZERO);
            loginLayer->addChild(loginMenu);
        }
    }
    else
    {
        // Show the existing login layer if it was already created
        loginLayer->setVisible(true);
        // Clear input fields when re-showing the login layer
        if (loginUsernameEditBox != nullptr)
        {
            loginUsernameEditBox->setText("");
        }
        if (loginPasswordEditBox != nullptr)
        {
            loginPasswordEditBox->setText("");
        }
    }

    // Hide login, register, and guest login buttons
    if (loginItem != nullptr)
    {
        loginItem->setVisible(false);
        loginItem->setEnabled(false);
        if (loginLabel != nullptr)
        {
            loginLabel->setVisible(false);
        }
    }
    if (registerItem != nullptr)
    {
        registerItem->setVisible(false);
        registerItem->setEnabled(false);
        if (registerLabel != nullptr)
        {
            registerLabel->setVisible(false);
        }
    }
    if (guestLoginItem != nullptr)
    {
        guestLoginItem->setVisible(false);
        guestLoginItem->setEnabled(false);
        if (guestLoginLabel != nullptr)
        {
            guestLoginLabel->setVisible(false);
        }
    }
}

void HelloWorld::menuRegisterCallback(cocos2d::Ref* pSender)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Create a semi-transparent background layer
    if (registerLayer == nullptr)
    {
        registerLayer = LayerColor::create(Color4B(0, 0, 0, 180));
        this->addChild(registerLayer, 10);

        // Create username label
        usernameLabel = Label::createWithSystemFont("Account:", "fonts/Marker Felt.ttf", 20);
        usernameLabel->setColor(Color3B::WHITE);
        usernameLabel->setPosition(Vec2(origin.x + visibleSize.width / 2 - 200,
            origin.y + visibleSize.height / 2 + 80));
        registerLayer->addChild(usernameLabel);

        // Create username edit box
        registerUsernameEditBox = ui::EditBox::create(Size(300, 40), "btn_normal.png");
        registerUsernameEditBox->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height / 2 + 80));
        registerUsernameEditBox->setPlaceholderFontName("fonts/Marker Felt.ttf");
        registerUsernameEditBox->setPlaceholderFontSize(20);
        registerUsernameEditBox->setPlaceHolder("Enter your account name");
        registerUsernameEditBox->setFontName("fonts/Marker Felt.ttf");
        registerUsernameEditBox->setFontSize(20);
        registerUsernameEditBox->setFontColor(Color3B::WHITE);
        registerUsernameEditBox->setReturnType(ui::EditBox::KeyboardReturnType::NEXT);
        registerUsernameEditBox->setDelegate(this);
        registerLayer->addChild(registerUsernameEditBox);

        // Create password label
        passwordLabel = Label::createWithSystemFont("Password:", "fonts/Marker Felt.ttf", 20);
        passwordLabel->setColor(Color3B::WHITE);
        passwordLabel->setPosition(Vec2(origin.x + visibleSize.width / 2 - 200,
            origin.y + visibleSize.height / 2 + 10));
        registerLayer->addChild(passwordLabel);

        // Create password edit box
        registerPasswordEditBox = ui::EditBox::create(Size(300, 40), "btn_normal.png");
        registerPasswordEditBox->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height / 2 + 10));
        registerPasswordEditBox->setPlaceholderFontName("fonts/Marker Felt.ttf");
        registerPasswordEditBox->setPlaceholderFontSize(20);
        registerPasswordEditBox->setPlaceHolder("Enter your password");
        registerPasswordEditBox->setFontName("fonts/Marker Felt.ttf");
        registerPasswordEditBox->setFontSize(20);
        registerPasswordEditBox->setFontColor(Color3B::WHITE);
        registerPasswordEditBox->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
        registerPasswordEditBox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
        registerPasswordEditBox->setReturnType(ui::EditBox::KeyboardReturnType::NEXT);
        registerPasswordEditBox->setDelegate(this);
        registerLayer->addChild(registerPasswordEditBox);

        // Create confirm password label
        confirmPasswordLabel = Label::createWithSystemFont("Confirm Password:", "fonts/Marker Felt.ttf", 20);
        confirmPasswordLabel->setColor(Color3B::WHITE);
        confirmPasswordLabel->setPosition(Vec2(origin.x + visibleSize.width / 2 - 240,
            origin.y + visibleSize.height / 2 - 60));
        registerLayer->addChild(confirmPasswordLabel);

        // Create confirm password edit box
        confirmPasswordEditBox = ui::EditBox::create(Size(300, 40), "btn_normal.png");
        confirmPasswordEditBox->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height / 2 - 60));
        confirmPasswordEditBox->setPlaceholderFontName("fonts/Marker Felt.ttf");
        confirmPasswordEditBox->setPlaceholderFontSize(20);
        confirmPasswordEditBox->setPlaceHolder("Confirm your password");
        confirmPasswordEditBox->setFontName("fonts/Marker Felt.ttf");
        confirmPasswordEditBox->setFontSize(20);
        confirmPasswordEditBox->setFontColor(Color3B::WHITE);
        confirmPasswordEditBox->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
        confirmPasswordEditBox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
        confirmPasswordEditBox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
        confirmPasswordEditBox->setDelegate(this);
        registerLayer->addChild(confirmPasswordEditBox);

        // Clear input fields when first creating register layer
        if (registerUsernameEditBox != nullptr)
        {
            registerUsernameEditBox->setText("");
        }
        if (registerPasswordEditBox != nullptr)
        {
            registerPasswordEditBox->setText("");
        }
        if (confirmPasswordEditBox != nullptr)
        {
            confirmPasswordEditBox->setText("");
        }

        // Create register confirm button
        registerConfirmItem = MenuItemImage::create(
            "btn_normal.png",
            "btn_pressed.png",
            CC_CALLBACK_1(HelloWorld::menuRegisterConfirmCallback, this));

        // Create register cancel button
        cancelRegisterItem = MenuItemImage::create(
            "btn_normal.png",
            "btn_pressed.png",
            CC_CALLBACK_1(HelloWorld::menuCancelRegisterCallback, this));

        if (registerConfirmItem != nullptr &&
            registerConfirmItem->getContentSize().width > 0 &&
            registerConfirmItem->getContentSize().height > 0 &&
            cancelRegisterItem != nullptr &&
            cancelRegisterItem->getContentSize().width > 0 &&
            cancelRegisterItem->getContentSize().height > 0)
        {
            double centerX = origin.x + visibleSize.width / 2;
            double y = origin.y + visibleSize.height / 2 - 130;

            // Position confirm button to the left of center
            registerConfirmItem->setPosition(Vec2(centerX - 120, y));

            // Position cancel button to the right of center (symmetric with confirm button)
            cancelRegisterItem->setPosition(Vec2(centerX + 120, y));

            auto confirmLabel = Label::createWithSystemFont("Register", "fonts/Marker Felt.ttf", 24);
            confirmLabel->setColor(Color3B::WHITE);
            confirmLabel->setPosition(Vec2(registerConfirmItem->getContentSize().width / 2,
                registerConfirmItem->getContentSize().height / 2));
            registerConfirmItem->addChild(confirmLabel);

            auto cancelLabel = Label::createWithSystemFont("Cancel", "fonts/Marker Felt.ttf", 24);
            cancelLabel->setColor(Color3B::WHITE);
            cancelLabel->setPosition(Vec2(cancelRegisterItem->getContentSize().width / 2,
                cancelRegisterItem->getContentSize().height / 2));
            cancelRegisterItem->addChild(cancelLabel);

            auto registerMenu = Menu::create(registerConfirmItem, cancelRegisterItem, NULL);
            registerMenu->setPosition(Vec2::ZERO);
            registerLayer->addChild(registerMenu);
        }

        // Create register result label
        registerResultLabel = Label::createWithSystemFont("", "fonts/Marker Felt.ttf", 20);
        registerResultLabel->setColor(Color3B::WHITE);
        registerResultLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height / 2 - 180));
        registerLayer->addChild(registerResultLabel);
    }
    else
    {
        // Show the existing register layer if it was already created
        // Clear previous result message
        if (registerResultLabel != nullptr)
        {
            registerResultLabel->setString("");
        }
        // Clear input fields
        if (registerUsernameEditBox != nullptr)
        {
            registerUsernameEditBox->setText("");
        }
        if (registerPasswordEditBox != nullptr)
        {
            registerPasswordEditBox->setText("");
        }
        if (confirmPasswordEditBox != nullptr)
        {
            confirmPasswordEditBox->setText("");
        }
        registerLayer->setVisible(true);
    }

    // Hide login, register, and guest login buttons
    if (loginItem != nullptr)
    {
        loginItem->setVisible(false);
        loginItem->setEnabled(false);
        if (loginLabel != nullptr)
        {
            loginLabel->setVisible(false);
        }
    }
    if (registerItem != nullptr)
    {
        registerItem->setVisible(false);
        registerItem->setEnabled(false);
        if (registerLabel != nullptr)
        {
            registerLabel->setVisible(false);
        }
    }
    if (guestLoginItem != nullptr)
    {
        guestLoginItem->setVisible(false);
        guestLoginItem->setEnabled(false);
        if (guestLoginLabel != nullptr)
        {
            guestLoginLabel->setVisible(false);
        }
    }
}

void HelloWorld::menuConfirmCallback(cocos2d::Ref* pSender)
{
    // Get input values
    std::string username = loginUsernameEditBox->getText();
    std::string password = loginPasswordEditBox->getText();

    // Send login request via WebSocket
    sendLoginRequest(username, password);
}

void HelloWorld::menuChangePasswordCallback(cocos2d::Ref* pSender)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    if (changePasswordLayer == nullptr)
    {
        changePasswordLayer = LayerColor::create(Color4B(0, 0, 0, 180));
        this->addChild(changePasswordLayer, 10);

        auto oldPasswordLabel = Label::createWithSystemFont("Old Password:", "fonts/Marker Felt.ttf", 20);
        oldPasswordLabel->setColor(Color3B::WHITE);
        oldPasswordLabel->setPosition(Vec2(origin.x + visibleSize.width / 2 - 200,
            origin.y + visibleSize.height / 2 + 100));
        changePasswordLayer->addChild(oldPasswordLabel);

        oldPasswordEditBox = ui::EditBox::create(Size(300, 40), "btn_normal.png");
        oldPasswordEditBox->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height / 2 + 100));
        oldPasswordEditBox->setPlaceholderFontName("fonts/Marker Felt.ttf");
        oldPasswordEditBox->setPlaceholderFontSize(20);
        oldPasswordEditBox->setPlaceHolder("Enter old password");
        oldPasswordEditBox->setFontName("fonts/Marker Felt.ttf");
        oldPasswordEditBox->setFontSize(20);
        oldPasswordEditBox->setFontColor(Color3B::WHITE);
        oldPasswordEditBox->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
        oldPasswordEditBox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
        oldPasswordEditBox->setReturnType(ui::EditBox::KeyboardReturnType::NEXT);
        oldPasswordEditBox->setDelegate(this);
        changePasswordLayer->addChild(oldPasswordEditBox);

        auto newPasswordLabel = Label::createWithSystemFont("New Password:", "fonts/Marker Felt.ttf", 20);
        newPasswordLabel->setColor(Color3B::WHITE);
        newPasswordLabel->setPosition(Vec2(origin.x + visibleSize.width / 2 - 200,
            origin.y + visibleSize.height / 2 + 30));
        changePasswordLayer->addChild(newPasswordLabel);

        newPasswordEditBox = ui::EditBox::create(Size(300, 40), "btn_normal.png");
        newPasswordEditBox->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height / 2 + 30));
        newPasswordEditBox->setPlaceholderFontName("fonts/Marker Felt.ttf");
        newPasswordEditBox->setPlaceholderFontSize(20);
        newPasswordEditBox->setPlaceHolder("Enter new password");
        newPasswordEditBox->setFontName("fonts/Marker Felt.ttf");
        newPasswordEditBox->setFontSize(20);
        newPasswordEditBox->setFontColor(Color3B::WHITE);
        newPasswordEditBox->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
        newPasswordEditBox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
        newPasswordEditBox->setReturnType(ui::EditBox::KeyboardReturnType::NEXT);
        newPasswordEditBox->setDelegate(this);
        changePasswordLayer->addChild(newPasswordEditBox);

        auto confirmNewPasswordLabel = Label::createWithSystemFont("Confirm Password:", "fonts/Marker Felt.ttf", 20);
        confirmNewPasswordLabel->setColor(Color3B::WHITE);
        confirmNewPasswordLabel->setPosition(Vec2(origin.x + visibleSize.width / 2 - 240,
            origin.y + visibleSize.height / 2 - 40));
        changePasswordLayer->addChild(confirmNewPasswordLabel);

        confirmNewPasswordEditBox = ui::EditBox::create(Size(300, 40), "btn_normal.png");
        confirmNewPasswordEditBox->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height / 2 - 40));
        confirmNewPasswordEditBox->setPlaceholderFontName("fonts/Marker Felt.ttf");
        confirmNewPasswordEditBox->setPlaceholderFontSize(20);
        confirmNewPasswordEditBox->setPlaceHolder("Confirm new password");
        confirmNewPasswordEditBox->setFontName("fonts/Marker Felt.ttf");
        confirmNewPasswordEditBox->setFontSize(20);
        confirmNewPasswordEditBox->setFontColor(Color3B::WHITE);
        confirmNewPasswordEditBox->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
        confirmNewPasswordEditBox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
        confirmNewPasswordEditBox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
        confirmNewPasswordEditBox->setDelegate(this);
        changePasswordLayer->addChild(confirmNewPasswordEditBox);

        if (oldPasswordEditBox != nullptr)
        {
            oldPasswordEditBox->setText("");
        }
        if (newPasswordEditBox != nullptr)
        {
            newPasswordEditBox->setText("");
        }
        if (confirmNewPasswordEditBox != nullptr)
        {
            confirmNewPasswordEditBox->setText("");
        }

        // Create confirm button
        changePasswordConfirmItem = MenuItemImage::create(
            "btn_normal.png",
            "btn_pressed.png",
            CC_CALLBACK_1(HelloWorld::menuChangePasswordConfirmCallback, this));

        // Create cancel button
        cancelChangePasswordItem = MenuItemImage::create(
            "btn_normal.png",
            "btn_pressed.png",
            CC_CALLBACK_1(HelloWorld::menuCancelChangePasswordCallback, this));

        if (changePasswordConfirmItem != nullptr &&
            changePasswordConfirmItem->getContentSize().width > 0 &&
            changePasswordConfirmItem->getContentSize().height > 0 &&
            cancelChangePasswordItem != nullptr &&
            cancelChangePasswordItem->getContentSize().width > 0 &&
            cancelChangePasswordItem->getContentSize().height > 0)
        {
            // Set confirm button position (left of center)
            double confirmX = origin.x + visibleSize.width / 2 - 120;
            double buttonY = origin.y + visibleSize.height / 2 - 110;
            changePasswordConfirmItem->setPosition(Vec2(confirmX, buttonY));

            // Set cancel button position (right of center, symmetric to confirm button)
            double cancelX = origin.x + visibleSize.width / 2 + 120;
            cancelChangePasswordItem->setPosition(Vec2(cancelX, buttonY));

            // Add label to confirm button
            auto confirmLabel = Label::createWithSystemFont("Confirm Change", "fonts/Marker Felt.ttf", 24);
            confirmLabel->setColor(Color3B::WHITE);
            confirmLabel->setPosition(Vec2(changePasswordConfirmItem->getContentSize().width / 2,
                changePasswordConfirmItem->getContentSize().height / 2));
            changePasswordConfirmItem->addChild(confirmLabel);

            // Add label to cancel button
            auto cancelLabel = Label::createWithSystemFont("Cancel", "fonts/Marker Felt.ttf", 24);
            cancelLabel->setColor(Color3B::WHITE);
            cancelLabel->setPosition(Vec2(cancelChangePasswordItem->getContentSize().width / 2,
                cancelChangePasswordItem->getContentSize().height / 2));
            cancelChangePasswordItem->addChild(cancelLabel);

            // Create a menu with both buttons
            auto changePasswordMenu = Menu::create(changePasswordConfirmItem, cancelChangePasswordItem, NULL);
            changePasswordMenu->setPosition(Vec2::ZERO);
            changePasswordLayer->addChild(changePasswordMenu);
        }
    }
    else
    {
        // Show the existing change password layer if it was already created
        changePasswordLayer->setVisible(true);
        // Clear input fields when re-showing the layer
        if (oldPasswordEditBox != nullptr)
        {
            oldPasswordEditBox->setText("");
        }
        if (newPasswordEditBox != nullptr)
        {
            newPasswordEditBox->setText("");
        }
        if (confirmNewPasswordEditBox != nullptr)
        {
            confirmNewPasswordEditBox->setText("");
        }
    }

    // Hide change password button and logout button
    if (changePasswordItem != nullptr)
    {
        changePasswordItem->setVisible(false);
        changePasswordItem->setEnabled(false);
    }
    if (loginItem != nullptr)
    {
        loginItem->setVisible(false);
        loginItem->setEnabled(false);
        if (loginLabel != nullptr)
        {
            loginLabel->setVisible(false);
        }
    }
    if (deleteAccountItem != nullptr)
    {
        deleteAccountItem->setVisible(false);
        deleteAccountItem->setEnabled(false);
    }
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
}

void HelloWorld::menuChangePasswordConfirmCallback(cocos2d::Ref* pSender)
{
    // Get input values
    std::string oldPassword = oldPasswordEditBox->getText();
    std::string newPassword = newPasswordEditBox->getText();
    std::string confirmNewPassword = confirmNewPasswordEditBox->getText();
    bool passwordChanged = false;

    // Validate password length (6-16 characters)
    if (oldPassword.empty())
    {
        auto errorLabel = Label::createWithSystemFont("Please enter old password", "fonts/Marker Felt.ttf", 18);
        errorLabel->setColor(Color3B::RED);
        auto visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        errorLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 150));
        changePasswordLayer->addChild(errorLabel);

        auto delay = DelayTime::create(2.0f);
        auto removeLabel = CallFunc::create([errorLabel]() {
            errorLabel->removeFromParentAndCleanup(true);
            });
        auto sequence = Sequence::create(delay, removeLabel, nullptr);
        errorLabel->runAction(sequence);
        return;
    }

    if (newPassword == oldPassword)
    {
        auto errorLabel = Label::createWithSystemFont(
            "New password cannot be the same as old password",
            "fonts/Marker Felt.ttf", 18);
        errorLabel->setColor(Color3B::RED);
        auto visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        errorLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 150));
        changePasswordLayer->addChild(errorLabel);

        auto delay = DelayTime::create(2.0f);
        auto removeLabel = CallFunc::create([errorLabel]() {
            errorLabel->removeFromParentAndCleanup(true);
            });
        auto sequence = Sequence::create(delay, removeLabel, nullptr);
        errorLabel->runAction(sequence);
        return;
    }

    if (newPassword.length() < 6 || newPassword.length() > 16)
    {
        // Show error message
        auto errorLabel = Label::createWithSystemFont(
            "Password length must be 6-16 characters",
            "fonts/Marker Felt.ttf", 18);
        errorLabel->setColor(Color3B::RED);
        auto visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        errorLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 150));
        changePasswordLayer->addChild(errorLabel);

        // Remove error message after 2 seconds
        auto delay = DelayTime::create(2.0f);
        auto removeLabel = CallFunc::create([errorLabel]() {
            errorLabel->removeFromParentAndCleanup(true);
            });
        auto sequence = Sequence::create(delay, removeLabel, nullptr);
        errorLabel->runAction(sequence);
        return;
    }

    // Get the current username from session
    auto session = SessionManager::getInstance();
    std::string username = session->getCurrentUsername();

    // Check if passwords match
    // Server will verify the old password and return result
    if (newPassword == confirmNewPassword)
    {
        pendingNewPassword = newPassword;
        pendingAction = "changePassword";
        sendVerifyPasswordRequest(username, oldPassword);
    }
    else
    {
        // Show error message
        auto errorLabel = Label::createWithSystemFont("Passwords do not match", "fonts/Marker Felt.ttf", 18);
        errorLabel->setColor(Color3B::RED);
        auto visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        errorLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, 
            origin.y + visibleSize.height / 2 - 150));
        changePasswordLayer->addChild(errorLabel);

        // Remove error message after 2 seconds
        auto delay = DelayTime::create(2.0f);
        auto removeLabel = CallFunc::create([errorLabel]() {
            errorLabel->removeFromParentAndCleanup(true);
            });
        auto sequence = Sequence::create(delay, removeLabel, nullptr);
        errorLabel->runAction(sequence);
    }
}

void HelloWorld::menuCancelChangePasswordCallback(cocos2d::Ref* pSender)
{
    // Hide change password layer
    if (changePasswordLayer != nullptr)
    {
        changePasswordLayer->setVisible(false);
        // Clear input fields
        if (oldPasswordEditBox != nullptr)
        {
            oldPasswordEditBox->setText("");
        }
        if (newPasswordEditBox != nullptr)
        {
            newPasswordEditBox->setText("");
        }
        if (confirmNewPasswordEditBox != nullptr)
        {
            confirmNewPasswordEditBox->setText("");
        }
    }

    // Show all scene buttons that were hidden
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
    auto session = SessionManager::getInstance();
    if (deleteAccountItem != nullptr && session->getCurrentUsername() != "")
    {
        deleteAccountItem->setVisible(true);
        deleteAccountItem->setEnabled(true);
    }
    if (loginItem != nullptr)
    {
        loginItem->setVisible(true);
        loginItem->setEnabled(true);
        if (loginLabel != nullptr)
        {
            loginLabel->setVisible(true);
        }
    }
    if (changePasswordItem != nullptr)
    {
        changePasswordItem->setVisible(true);
        changePasswordItem->setEnabled(true);
    }
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

void HelloWorld::menuCancelLoginCallback(cocos2d::Ref* pSender)
{
    // Hide login layer and return to unlogged state
    if (loginLayer != nullptr)
    {
        loginLayer->setVisible(false);
        // Clear input fields
        if (loginUsernameEditBox != nullptr)
        {
            loginUsernameEditBox->setText("");
        }
        if (loginPasswordEditBox != nullptr)
        {
            loginPasswordEditBox->setText("");
        }
    }

    // Show login, register, and guest login buttons
    if (loginItem != nullptr)
    {
        loginItem->setVisible(true);
        loginItem->setEnabled(true);
        if (loginLabel != nullptr)
        {
            loginLabel->setVisible(true);
        }
    }
    if (registerItem != nullptr)
    {
        registerItem->setVisible(true);
        registerItem->setEnabled(true);
        if (registerLabel != nullptr)
        {
            registerLabel->setVisible(true);
        }
    }
    if (guestLoginItem != nullptr)
    {
        guestLoginItem->setVisible(true);
        guestLoginItem->setEnabled(true);
        if (guestLoginLabel != nullptr)
        {
            guestLoginLabel->setVisible(true);
        }
    }
}

void HelloWorld::menuCancelRegisterCallback(cocos2d::Ref* pSender)
{
    // Hide register layer and return to unregistered state
    if (registerLayer != nullptr)
    {
        registerLayer->setVisible(false);
        // Clear input fields
        if (registerUsernameEditBox != nullptr)
        {
            registerUsernameEditBox->setText("");
        }
        if (registerPasswordEditBox != nullptr)
        {
            registerPasswordEditBox->setText("");
        }
        if (confirmPasswordEditBox != nullptr)
        {
            confirmPasswordEditBox->setText("");
        }
        // Clear result message
        if (registerResultLabel != nullptr)
        {
            registerResultLabel->setString("");
        }
    }

    // Show login, register, and guest login buttons
    if (loginItem != nullptr)
    {
        loginItem->setVisible(true);
        loginItem->setEnabled(true);
        if (loginLabel != nullptr)
        {
            loginLabel->setVisible(true);
        }
    }
    if (registerItem != nullptr)
    {
        registerItem->setVisible(true);
        registerItem->setEnabled(true);
        if (registerLabel != nullptr)
        {
            registerLabel->setVisible(true);
        }
    }
    if (guestLoginItem != nullptr)
    {
        guestLoginItem->setVisible(true);
        guestLoginItem->setEnabled(true);
        if (guestLoginLabel != nullptr)
        {
            guestLoginLabel->setVisible(true);
        }
    }
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
        

    // Hide all scene buttons except sub-buttons
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
    if (deleteAccountItem != nullptr)
    {
        deleteAccountItem->setVisible(false);
        deleteAccountItem->setEnabled(false);
    }
    if (loginItem != nullptr)
    {
        loginItem->setVisible(false);
        loginItem->setEnabled(false);
        if (loginLabel != nullptr)
        {
            loginLabel->setVisible(false);
        }
    }
}

void HelloWorld::menuConfirmLogoutCallback(cocos2d::Ref* pSender)
{
    CCLOG("=== menuConfirmLogoutCallback called ===");
    // Hide logout confirmation layer
    if (logoutConfirmLayer != nullptr)
    {
        logoutConfirmLayer->setVisible(false);
    }
    // Send logout request to server
    auto session = SessionManager::getInstance();
    std::string username = session->getCurrentUsername();

    bool isLoggedIn = session->getIsLoggedIn();

    CCLOG("Current username: %s, isLoggedIn: %d", username.c_str(), isLoggedIn);

    if (!username.empty() && isLoggedIn) {
        // 非游客登录，发送登出请求给服务器

        // 立即隐藏功能按钮，让用户感觉已经登出
        if (secondSceneItem != nullptr) {
            secondSceneItem->setVisible(false);
            secondSceneItem->setEnabled(false);
        }
        if (battleTestItem != nullptr) {
            battleTestItem->setVisible(false);
            battleTestItem->setEnabled(false);
        }
        if (deleteAccountItem != nullptr) {
            deleteAccountItem->setVisible(false);
            deleteAccountItem->setEnabled(false);
        }
        if (changePasswordItem != nullptr) {
            changePasswordItem->setVisible(false);
            changePasswordItem->setEnabled(false);
        }
        if (welcomeLabel != nullptr) {
            welcomeLabel->setVisible(false);
        }

        CCLOG("Sending logout request for user: %s", username.c_str());
        sendLogoutRequest(username);
        // UI更新将在handleWebSocketMessage中处理
    }
    else {
        // 游客登录，直接更新UI
        CCLOG("Guest login, performing local logout");
        performLocalLogout();
    }
}

void HelloWorld::menuCancelLogoutCallback(cocos2d::Ref* pSender)
{
    // Hide logout confirmation layer and keep login status
    if (logoutConfirmLayer != nullptr)
    {
        logoutConfirmLayer->setVisible(false);
    }

    // Show all scene buttons that were hidden
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
    auto session = SessionManager::getInstance();
    if (deleteAccountItem != nullptr && session->getCurrentUsername() != "")
    {
        deleteAccountItem->setVisible(true);
        deleteAccountItem->setEnabled(true);
    }
    if (loginItem != nullptr)
    {
        loginItem->setVisible(true);
        loginItem->setEnabled(true);
        if (loginLabel != nullptr)
        {
            loginLabel->setVisible(true);
        }
    }
}

void HelloWorld::menuRegisterConfirmCallback(cocos2d::Ref* pSender)
{
    // Get input values
    std::string username = registerUsernameEditBox->getText();
    std::string password = registerPasswordEditBox->getText();
    std::string confirmPassword = confirmPasswordEditBox->getText();

    // Validate input
    if (username.empty())
    {
        if (registerResultLabel != nullptr)
        {
            registerResultLabel->setString("Registration Failed: Username too short");
            registerResultLabel->setColor(Color3B::RED);
        }

    }
    else if (password.length() < 6 || password.length() > 16)
    {
        if (registerResultLabel != nullptr)
        {
            registerResultLabel->setString("Registration Failed: Password length 6-16 chars");
            registerResultLabel->setColor(Color3B::RED);
        }

    }
    else if (password != confirmPassword)
    {
        if (registerResultLabel != nullptr)
        {
            registerResultLabel->setString("Registration Failed: Passwords do not match");
            registerResultLabel->setColor(Color3B::RED);

        }
    }
    else
    {
        // Send register request via WebSocket
        sendRegisterRequest(username, password);
    }
}

void HelloWorld::menuGuestLoginCallback(cocos2d::Ref* pSender)
{
    // Directly login as guest, same as successful login
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

    // Hide guest login and register buttons - ensure both are hidden
    if (guestLoginItem != nullptr)
    {
        guestLoginItem->setVisible(false);
        guestLoginItem->setEnabled(false);
        // Also hide the label to be extra thorough
        if (guestLoginLabel != nullptr)
        {
            guestLoginLabel->setVisible(false);
        }
    }
    if (registerItem != nullptr)
    {
        registerItem->setVisible(false);
        registerItem->setEnabled(false);
        // Also hide the label to be extra thorough
        if (registerLabel != nullptr)
        {
            registerLabel->setVisible(false);
        }
    }
    // Update login status - guest login doesn't set currentLoggedInUser
    auto session = SessionManager::getInstance();
    session->login(""); // Guest login uses empty username

    // Update welcome label for guest login
    if (welcomeLabel != nullptr)
    {
        welcomeLabel->setString("Welcome!");
        welcomeLabel->setVisible(true);
    }
    // Change login button to logout button
    loginItem->setCallback(CC_CALLBACK_1(HelloWorld::menuLogoutCallback, this));
    loginLabel->setString("LOGOUT");

    // Hide delete account button for guest login
    if (deleteAccountItem != nullptr)
    {
        deleteAccountItem->setVisible(false);
        deleteAccountItem->setEnabled(false);
    }
}

void HelloWorld::setupWebSocketCallbacks()
{
    auto wsManager = WebSocketManager::getInstance();

    wsManager->setOnOpenCallback([this]() {
        _isConnecting = false;
        if (_connectionTimeoutScheduled) {
            this->unschedule(CC_SCHEDULE_SELECTOR(HelloWorld::connectionTimeoutCallback));
            _connectionTimeoutScheduled = false;
        }
        stopReconnecting();
        CCLOG("WebSocket connected successfully");
        });

    wsManager->setOnMessageCallback([this](const std::string& message) {
        handleWebSocketMessage(message);
        });

    wsManager->setOnErrorCallback([this](WebSocket::ErrorCode errorCode) {
        _isConnecting = false;
        _connectionTimeoutScheduled = false;
        CCLOG("WebSocket error: %d, will retry...", static_cast<int>(errorCode));
        if (!_isReconnecting) {
            startReconnecting();
        }
        });

    wsManager->setOnCloseCallback([this]() {
        _isConnecting = false;
        _connectionTimeoutScheduled = false;
        CCLOG("WebSocket disconnected, will retry...");
        if (!_isReconnecting) {
            startReconnecting();
        }
        });
}
    
void HelloWorld::connectionTimeoutCallback(float dt) 
{
    if (_isConnecting) {
        CCLOG("WebSocket connection timeout after %d seconds", CONNECTION_TIMEOUT_SECONDS);
        auto wsManager = WebSocketManager::getInstance();
        wsManager->disconnect();
        _isConnecting = false;
        _connectionTimeoutScheduled = false;

        // 如果不是用户主动停止重连，则开始重试
        if (!_isReconnecting) {
            startReconnecting();
        }
    }
}

void HelloWorld::startReconnecting() {
    if (_isReconnecting) {
        return;
    }

    CCLOG("Starting auto-reconnect, will retry every %d seconds", RECONNECT_INTERVAL_SECONDS);
    _isReconnecting = true;

    // 先尝试一次连接
    attemptConnection();

    // 启动定时重试
    this->schedule([this](float dt) {
        retryConnectionCallback(dt);
        }, RECONNECT_INTERVAL_SECONDS, "retryConnection");
}

void HelloWorld::stopReconnecting() {
    if (!_isReconnecting) {
        return;
    }

    CCLOG("Stopping auto-reconnect");
    _isReconnecting = false;
    this->unschedule("retryConnection");
}

void HelloWorld::retryConnectionCallback(float dt) {
    if (_isReconnecting) {
        attemptConnection();
    }
}

void HelloWorld::attemptConnection() {
    auto wsManager = WebSocketManager::getInstance();
    WebSocket::State readyState = wsManager->getReadyState();

    if (readyState == WebSocket::State::OPEN) {
        CCLOG("Connected to server successfully, stopping reconnect");
        stopReconnecting();
        return;
    }

    if (_isConnecting) {
        CCLOG("Already attempting to connect, skipping...");
        return;
    }
    CCLOG("Attempting to connect to server: %s", _serverUrl.c_str());
    _isConnecting = true;

    if (wsManager->connect(_serverUrl)) {
        this->scheduleOnce([this](float dt) {
            connectionTimeoutCallback(dt);
            }, CONNECTION_TIMEOUT_SECONDS, "connectionTimeout");
        _connectionTimeoutScheduled = true;
    }
    else {
        _isConnecting = false;
        CCLOG("Failed to initiate WebSocket connection");
    }
}

void HelloWorld::autoConnectToServer() {
    if (_isReconnecting) {
        CCLOG("Already reconnecting, skipping...");
        return;
    }

    auto wsManager = WebSocketManager::getInstance();
    WebSocket::State readyState = wsManager->getReadyState();

    if (readyState == WebSocket::State::OPEN) {
        CCLOG("Already connected to server");
        return;
    }

    startReconnecting();
}

void HelloWorld::onEnter() {
    Scene::onEnter();
    CCLOG("HelloWorld scene entering, starting auto-connect...");
    autoConnectToServer();
}

void HelloWorld::onExit() {
    CCLOG("HelloWorld scene exiting, keeping WebSocket connection alive for global access...");

    Scene::onExit();
}

void HelloWorld::handleWebSocketMessage(const std::string& message) {
    rapidjson::Document doc;
    doc.Parse(message.c_str());

    if (doc.HasParseError() || !doc.IsObject()) {
        CCLOG("Failed to parse WebSocket message as JSON");
        return;
    }

    std::string action = "";
    bool result = false;
    std::string responseMessage = "";

    if (doc.HasMember("action") && doc["action"].IsString()) {
        action = doc["action"].GetString();
    }
    if (doc.HasMember("result") && doc["result"].IsBool()) {
        result = doc["result"].GetBool();
    }
    if (doc.HasMember("message") && doc["message"].IsString()) {
        responseMessage = doc["message"].GetString();
    }

    if (action == "login") {
        if (result) {
            // Login success - update UI
            if (loginLayer != nullptr) {
                loginLayer->setVisible(false);
            }

            if (secondSceneItem != nullptr) {
                secondSceneItem->setVisible(true);
                secondSceneItem->setEnabled(true);
            }
            if (battleTestItem != nullptr) {
                battleTestItem->setVisible(true);
                battleTestItem->setEnabled(true);
            }
            if (deleteAccountItem != nullptr) {
                deleteAccountItem->setVisible(true);
                deleteAccountItem->setEnabled(true);
            }

            if (guestLoginItem != nullptr) {
                guestLoginItem->setVisible(false);
                guestLoginItem->setEnabled(false);
                if (guestLoginLabel != nullptr) {
                    guestLoginLabel->setVisible(false);
                }
            }
            if (registerItem != nullptr) {
                registerItem->setVisible(false);
                registerItem->setEnabled(false);
                if (registerLabel != nullptr) {
                    registerLabel->setVisible(false);
                }
            }

            auto session = SessionManager::getInstance();
            session->login(pendingUsername, LoginType::ACCOUNT);

            if (welcomeLabel != nullptr) {
                welcomeLabel->setString("Welcome " + pendingUsername + "!");
                welcomeLabel->setVisible(true);
            }

            if (loginItem != nullptr) {
                loginItem->setVisible(true);
                loginItem->setEnabled(true);
                if (loginLabel != nullptr) {
                    loginLabel->setVisible(true);
                }
            }

            loginItem->setCallback(CC_CALLBACK_1(HelloWorld::menuLogoutCallback, this));
            loginLabel->setString("LOGOUT");

            if (changePasswordItem != nullptr) {
                changePasswordItem->setVisible(true);
                changePasswordItem->setEnabled(true);
            }
        }
        else {
            // Login failed - show error
            auto errorLabel = Label::createWithSystemFont("Login Failed: " + responseMessage, "fonts/Marker Felt.ttf", 18);
            errorLabel->setColor(Color3B::RED);
            auto visibleSize = Director::getInstance()->getVisibleSize();
            Vec2 origin = Director::getInstance()->getVisibleOrigin();
            errorLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 150));
            loginLayer->addChild(errorLabel);

            auto delay = DelayTime::create(2.0f);
            auto removeLabel = CallFunc::create([errorLabel]() {
                errorLabel->removeFromParentAndCleanup(true);
                });
            auto sequence = Sequence::create(delay, removeLabel, nullptr);
            errorLabel->runAction(sequence);
        }
        pendingUsername = "";
    }
    else if (action == "register") {
        if (result) {
            // Registration success
            if (registerResultLabel != nullptr) {
                registerResultLabel->setString("Registration Success");
                registerResultLabel->setColor(Color3B::GREEN);
            }

            if (registerLayer != nullptr) {
                auto delay = DelayTime::create(1.0f);
                auto hideLayer = CallFunc::create([this]() {
                    if (registerLayer != nullptr) {
                        registerLayer->setVisible(false);
                    }
                    if (registerUsernameEditBox != nullptr) {
                        registerUsernameEditBox->setText("");
                    }
                    if (registerPasswordEditBox != nullptr) {
                        registerPasswordEditBox->setText("");
                    }
                    if (confirmPasswordEditBox != nullptr) {
                        confirmPasswordEditBox->setText("");
                    }
                    if (registerResultLabel != nullptr) {
                        registerResultLabel->setString("");
                    }
                    if (loginItem != nullptr) {
                        loginItem->setVisible(true);
                        loginItem->setEnabled(true);
                        if (loginLabel != nullptr) {
                            loginLabel->setVisible(true);
                        }
                    }
                    if (registerItem != nullptr) {
                        registerItem->setVisible(true);
                        registerItem->setEnabled(true);
                        if (registerLabel != nullptr) {
                            registerLabel->setVisible(true);
                        }
                    }
                    if (guestLoginItem != nullptr) {
                        guestLoginItem->setVisible(true);
                        guestLoginItem->setEnabled(true);
                        if (guestLoginLabel != nullptr) {
                            guestLoginLabel->setVisible(true);
                        }
                    }
                    });
                auto sequence = Sequence::create(delay, hideLayer, nullptr);
                registerLayer->runAction(sequence);
            }
        }
        else {
            // Registration failed
            if (registerResultLabel != nullptr) {
                registerResultLabel->setString("Registration Failed: " + responseMessage);
                registerResultLabel->setColor(Color3B::RED);
            }
        }
    }
    else if (action == "delete") {
        if (result) {
            // Delete success
            CCLOG("Account deleted successfully");

            if (loginItem != nullptr) {
                loginItem->setCallback(CC_CALLBACK_1(HelloWorld::menuLoginCallback, this));
                loginLabel->setString("LOGIN");
                loginItem->setVisible(true);
                loginItem->setEnabled(true);
                loginLabel->setVisible(true);
            }

            if (secondSceneItem != nullptr) {
                secondSceneItem->setVisible(false);
                secondSceneItem->setEnabled(false);
            }
            if (battleTestItem != nullptr) {
                battleTestItem->setVisible(false);
                battleTestItem->setEnabled(false);
            }
            if (deleteAccountItem != nullptr) {
                deleteAccountItem->setVisible(false);
                deleteAccountItem->setEnabled(false);
            }
            if (changePasswordItem != nullptr) {
                changePasswordItem->setVisible(false);
                changePasswordItem->setEnabled(false);
            }

            if (guestLoginItem != nullptr) {
                guestLoginItem->setVisible(true);
                guestLoginItem->setEnabled(true);
                if (guestLoginLabel != nullptr) {
                    guestLoginLabel->setVisible(true);
                }
            }
            if (registerItem != nullptr) {
                registerItem->setVisible(true);
                registerItem->setEnabled(true);
                if (registerLabel != nullptr) {
                    registerLabel->setVisible(true);
                }
            }

            auto session = SessionManager::getInstance();
            std::string username = session->getCurrentUsername();
            sendLogoutRequest(username);
            session->logout();

            if (welcomeLabel != nullptr) {
                welcomeLabel->setVisible(false);
                welcomeLabel->setString("");
            }
        }
        else {
            CCLOG("Error deleting account: %s", responseMessage.c_str());
        }
    }
    else if (action == "verifyPassword") {
        if (result) {
            auto session = SessionManager::getInstance();
            std::string username = session->getCurrentUsername();

            if (pendingAction == "changePassword") {
                std::string oldPassword = oldPasswordEditBox->getText();
                sendChangePasswordRequest(username, oldPassword, pendingNewPassword);

                pendingAction = "";
                pendingNewPassword = "";
            }
        }
        else {
            auto errorLabel = Label::createWithSystemFont("Old password is incorrect", "fonts/Marker Felt.ttf", 18);
            errorLabel->setColor(Color3B::RED);
            auto visibleSize = Director::getInstance()->getVisibleSize();
            Vec2 origin = Director::getInstance()->getVisibleOrigin();
            errorLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 150));
            changePasswordLayer->addChild(errorLabel);

            auto delay = DelayTime::create(2.0f);
            auto removeLabel = CallFunc::create([errorLabel]() {
                errorLabel->removeFromParentAndCleanup(true);
                });
            auto sequence = Sequence::create(delay, removeLabel, nullptr);
            errorLabel->runAction(sequence);

            pendingAction = "";
            pendingNewPassword = "";
        }
    }
    else if (action == "changePassword") {
        if (result) {
            // Password change success
            auto successLabel = Label::createWithSystemFont("Password changed successfully", "fonts/Marker Felt.ttf", 18);
            successLabel->setColor(Color3B::GREEN);
            auto visibleSize = Director::getInstance()->getVisibleSize();
            Vec2 origin = Director::getInstance()->getVisibleOrigin();
            successLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 150));
            changePasswordLayer->addChild(successLabel);

            auto delay = DelayTime::create(1.0f);
            auto removeLabel = CallFunc::create([this, successLabel]() {
                successLabel->removeFromParentAndCleanup(true);
                if (changePasswordLayer != nullptr) {
                    changePasswordLayer->setVisible(false);
                }
                if (secondSceneItem != nullptr) {
                    secondSceneItem->setVisible(true);
                    secondSceneItem->setEnabled(true);
                }
                if (battleTestItem != nullptr) {
                    battleTestItem->setVisible(true);
                    battleTestItem->setEnabled(true);
                }
                auto session = SessionManager::getInstance();
                if (deleteAccountItem != nullptr && session->getCurrentUsername() != "") {
                    deleteAccountItem->setVisible(true);
                    deleteAccountItem->setEnabled(true);
                }
                if (loginItem != nullptr) {
                    loginItem->setVisible(true);
                    loginItem->setEnabled(true);
                    if (loginLabel != nullptr) {
                        loginLabel->setVisible(true);
                    }
                }
                if (changePasswordItem != nullptr) {
                    changePasswordItem->setVisible(true);
                    changePasswordItem->setEnabled(true);
                }
                });
            auto sequence = Sequence::create(delay, removeLabel, nullptr);
            successLabel->runAction(sequence);
        }
        else {
            // Password change failed
            auto errorLabel = Label::createWithSystemFont("Failed to change password: " + responseMessage, "fonts/Marker Felt.ttf", 18);
            errorLabel->setColor(Color3B::RED);
            auto visibleSize = Director::getInstance()->getVisibleSize();
            Vec2 origin = Director::getInstance()->getVisibleOrigin();
            errorLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 150));
            changePasswordLayer->addChild(errorLabel);

            auto delay = DelayTime::create(2.0f);
            auto removeLabel = CallFunc::create([errorLabel]() {
                errorLabel->removeFromParentAndCleanup(true);
                });
            auto sequence = Sequence::create(delay, removeLabel, nullptr);
            errorLabel->runAction(sequence);
        }
    }
    else if (action == "logout") {
        CCLOG("=== Received logout response ===");
        CCLOG("Result: %d, Message: %s", result, responseMessage.c_str());
        if (result) {
            // Logout success - update UI
            CCLOG("Logout success, performing local logout");
            performLocalLogout();
        }
        else {
            // Logout failed
            CCLOG("Logout failed: %s", responseMessage.c_str());
        }
    }
}

void HelloWorld::sendLoginRequest(const std::string& username, const std::string& password) {
    pendingUsername = username;
    std::string message = "{\"action\":\"login\",\"username\":\"" +
        username + "\",\"password\":\"" + password + "\"}";
    WebSocketManager::getInstance()->send(message);
    CCLOG("Sending login request for user: %s", username.c_str());
}

void HelloWorld::sendRegisterRequest(const std::string& username, const std::string& password) {
    std::string message = "{\"action\":\"register\",\"username\":\"" +
        username + "\",\"password\":\"" + password + "\"}";
    WebSocketManager::getInstance()->send(message);
    CCLOG("Sending register request for user: %s", username.c_str());
}

void HelloWorld::sendDeleteRequest(const std::string& username) {
    std::string message = "{\"action\":\"delete\",\"username\":\"" + username + "\"}";
    WebSocketManager::getInstance()->send(message);
    CCLOG("Sending delete request for user: %s", username.c_str());
}

void HelloWorld::sendChangePasswordRequest(const std::string& username,
    const std::string& oldPassword, const std::string& newPassword) {
    std::string message = "{\"action\":\"changePassword\",\"username\":\"" +
        username + "\",\"oldPassword\":\"" + oldPassword +
        "\",\"newPassword\":\"" + newPassword + "\"}";
    WebSocketManager::getInstance()->send(message);
    CCLOG("Sending change password request for user: %s", username.c_str());
}

void HelloWorld::sendVerifyPasswordRequest(const std::string& username, const std::string& password) {
    std::string message = "{\"action\":\"verifyPassword\",\"username\":\"" +
        username + "\",\"password\":\"" + password + "\"}";
    WebSocketManager::getInstance()->send(message);
    CCLOG("Sending verify password request for user: %s", username.c_str());
}

void HelloWorld::sendLogoutRequest(const std::string& username) {
    std::string message = "{\"action\":\"logout\",\"username\":\"" + username + "\"}";
    CCLOG("Sending logout message: %s", message.c_str());
    WebSocketManager::getInstance()->send(message);
    CCLOG("Logout request sent for user: %s", username.c_str());
}

void HelloWorld::performLocalLogout() {
    // Change logout button back to login button
    loginItem->setCallback(CC_CALLBACK_1(HelloWorld::menuLoginCallback, this));
    loginLabel->setString("LOGIN");
    // Show login button that was hidden during logout confirmation
    loginItem->setVisible(true);
    loginItem->setEnabled(true);
    loginLabel->setVisible(true);
    // Hide and disable secondSceneItem, battleTestItem, deleteAccountItem, and changePasswordItem
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

    if (deleteAccountItem != nullptr)
    {
        deleteAccountItem->setVisible(false);
        deleteAccountItem->setEnabled(false);
    }

    if (changePasswordItem != nullptr)
    {
        changePasswordItem->setVisible(false);
        changePasswordItem->setEnabled(false);
    }

    // Show guest login and register buttons
    if (guestLoginItem != nullptr)
    {
        guestLoginItem->setVisible(true);
        guestLoginItem->setEnabled(true);
        if (guestLoginLabel != nullptr)
        {
            guestLoginLabel->setVisible(true);
        }
    }
    if (registerItem != nullptr)
    {
        registerItem->setVisible(true);
        registerItem->setEnabled(true);
        if (registerLabel != nullptr)
        {
            registerLabel->setVisible(true);
        }
    }

    // Update login status
    auto session = SessionManager::getInstance();
    session->logout();

    // Hide welcome label
    if (welcomeLabel != nullptr)
    {
        welcomeLabel->setVisible(false);
        welcomeLabel->setString("");
    }
}