#include "HelloWorldScene.h"
#include "SecondScene.h"
#include "BattleTestLayer.h"
#include "SQLiteTest.h"
#include "SessionManager.h"

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
        guestLoginItem, loginItem, registerItem, deleteAccountItem, changePasswordItem, NULL);
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

        // Show delete account button only when logged in with username/password
        if (deleteAccountItem != nullptr && !session->getCurrentUsername().empty())
        {
            deleteAccountItem->setVisible(true);
            deleteAccountItem->setEnabled(true);
        }
        else if (deleteAccountItem != nullptr)
        {
            deleteAccountItem->setVisible(false);
            deleteAccountItem->setEnabled(false);
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
#if 0
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
#endif
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

    // 初始化SQLite数据库并创建用户表
    sqlite3* db;
    int rc = sqlite3_open("users.db", &db);

    if (rc) {
        CCLOG("无法打开数据库: %s", sqlite3_errmsg(db));
    }
    else {
        CCLOG("数据库打开成功");

        // 创建用户表（如果不存在）
        const char* sql = "CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password TEXT);";
        char* errMsg = nullptr;

        rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

        if (rc != SQLITE_OK) {
            CCLOG("创建表失败: %s", errMsg);
            sqlite3_free(errMsg);
        }
        else {
            CCLOG("用户表创建成功");
        }

        sqlite3_close(db);
    }

    // 根据全局登录状态初始化welcomeLabel
    auto Session = SessionManager::getInstance();
    if (Session->getIsLoggedIn()) {
        std::string username = Session->getCurrentUsername();
        if (welcomeLabel != nullptr) {
            welcomeLabel->setString(username.empty() ? "Welcome!" : "Welcome " + username + "!");
            welcomeLabel->setVisible(true);
        }
    }

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

    // Delete account from SQLite database
    auto session = SessionManager::getInstance();
    std::string username = session->getCurrentUsername();
    if (!username.empty())
    {
        bool deleteSuccess = false;
        sqlite3* db;
        int rc = sqlite3_open("users.db", &db);

        if (rc == SQLITE_OK)
        {
            // Start a transaction
            rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
            if (rc != SQLITE_OK)
            {
                CCLOG("Error starting transaction: %s", sqlite3_errmsg(db));
                sqlite3_close(db);
                return;
            }

            // Prepare SQL statement to delete user
            const char* sql = "DELETE FROM users WHERE username = ?;";
            sqlite3_stmt* stmt;

            rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

            if (rc == SQLITE_OK)
            {
                // Bind username parameter
                sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

                // Execute the query
                if (sqlite3_step(stmt) == SQLITE_DONE)
                {
                    CCLOG("Account deleted successfully");
                    deleteSuccess = true;
                }
                else
                {
                    CCLOG("Error deleting account: %s", sqlite3_errmsg(db));
                }

                sqlite3_finalize(stmt);
            }
            else
            {
                CCLOG("Error preparing SQL statement: %s", sqlite3_errmsg(db));
            }

            // Commit or rollback transaction based on result
            if (deleteSuccess)
            {
                sqlite3_exec(db, "COMMIT TRANSACTION;", nullptr, nullptr, nullptr);
            }
            else
            {
                sqlite3_exec(db, "ROLLBACK TRANSACTION;", nullptr, nullptr, nullptr);
            }

            sqlite3_close(db);
        }
        else
        {
            CCLOG("Error opening database: %s", sqlite3_errmsg(db));
        }

        if (deleteSuccess)
        {
            // Ensure login button is in login state
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

    // Check if username and password match any account in the SQLite database
    bool loginSuccess = false;
    sqlite3* db;
    int rc = sqlite3_open("users.db", &db);

    if (rc == SQLITE_OK)
    {
        // Prepare SQL statement to check credentials
        const char* sql = "SELECT password FROM users WHERE username = ?;";
        sqlite3_stmt* stmt;

        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

        if (rc == SQLITE_OK)
        {
            // Bind username parameter
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

            // Execute the query
            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                // Get password from database
                const char* dbPassword = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                if (dbPassword != nullptr && password == dbPassword)
                {
                    loginSuccess = true;
                }
            }

            sqlite3_finalize(stmt);
        }

        sqlite3_close(db);
    }

    if (loginSuccess)
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

        // Show delete account button for username/password login
        if (deleteAccountItem != nullptr)
        {
            deleteAccountItem->setVisible(true);
            deleteAccountItem->setEnabled(true);
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
        if (loginItem != nullptr)
        {
            loginItem->setVisible(true);
            loginItem->setEnabled(true);
            if (loginLabel != nullptr)
            {
                loginLabel->setVisible(true);
            }
        }

        // Update login status
        auto session = SessionManager::getInstance();
        session->login(username);

        // Update welcome label
        if (welcomeLabel != nullptr)
        {
            welcomeLabel->setString("Welcome " + username + "!");
            welcomeLabel->setVisible(true);
        }

        // Change login button to logout button
        loginItem->setCallback(CC_CALLBACK_1(HelloWorld::menuLogoutCallback, this));
        loginLabel->setString("LOGOUT");

        // Show change password button next to logout button (symmetric position)
        if (changePasswordItem != nullptr)
        {
            changePasswordItem->setVisible(true);
            changePasswordItem->setEnabled(true);
        }
    }
    else
    {
        // Show login error message
        auto errorLabel = Label::createWithSystemFont("Login Failed: Invalid username or password", "fonts/Marker Felt.ttf", 18);
        errorLabel->setColor(Color3B::RED);
        auto visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        errorLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 150));
        loginLayer->addChild(errorLabel);

        // Remove error message after 2 seconds
        auto delay = DelayTime::create(2.0f);
        auto removeLabel = CallFunc::create([errorLabel]() {
            errorLabel->removeFromParentAndCleanup(true);
            });
        auto sequence = Sequence::create(delay, removeLabel, nullptr);
        errorLabel->runAction(sequence);
    }
}

void HelloWorld::menuChangePasswordCallback(cocos2d::Ref* pSender)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Create a semi-transparent background layer
    if (changePasswordLayer == nullptr)
    {
        changePasswordLayer = LayerColor::create(Color4B(0, 0, 0, 180));
        this->addChild(changePasswordLayer, 10);

        // Create new password label
        auto newPasswordLabel = Label::createWithSystemFont("New Password:", "fonts/Marker Felt.ttf", 20);
        newPasswordLabel->setColor(Color3B::WHITE);
        newPasswordLabel->setPosition(Vec2(origin.x + visibleSize.width / 2 - 200,
            origin.y + visibleSize.height / 2 + 50));
        changePasswordLayer->addChild(newPasswordLabel);

        // Create new password edit box
        newPasswordEditBox = ui::EditBox::create(Size(300, 40), "btn_normal.png");
        newPasswordEditBox->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height / 2 + 50));
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

        // Create confirm new password label
        auto confirmNewPasswordLabel = Label::createWithSystemFont("Confirm Password:", "fonts/Marker Felt.ttf", 20);
        confirmNewPasswordLabel->setColor(Color3B::WHITE);
        confirmNewPasswordLabel->setPosition(Vec2(origin.x + visibleSize.width / 2 - 240,
            origin.y + visibleSize.height / 2 - 20));
        changePasswordLayer->addChild(confirmNewPasswordLabel);

        // Create confirm new password edit box
        confirmNewPasswordEditBox = ui::EditBox::create(Size(300, 40), "btn_normal.png");
        confirmNewPasswordEditBox->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height / 2 - 20));
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

        // Clear input fields when first creating change password layer
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
            double buttonY = origin.y + visibleSize.height / 2 - 90;
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
    std::string newPassword = newPasswordEditBox->getText();
    std::string confirmNewPassword = confirmNewPasswordEditBox->getText();
    bool passwordChanged = false;

    // Validate password length (6-16 characters)
    if (newPassword.length() < 6 || newPassword.length() > 16)
    {
        // Show error message
        auto errorLabel = Label::createWithSystemFont("Password length must be 6-16 characters", "fonts/Marker Felt.ttf", 18);
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

    // Check if new password is the same as old password
    {
        sqlite3* db;
        int rc = sqlite3_open("users.db", &db);
        bool samePassword = false;

        if (rc == SQLITE_OK)
        {
            const char* sql = "SELECT password FROM users WHERE username = ?;";
            sqlite3_stmt* stmt;
            rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

            if (rc == SQLITE_OK)
            {
                auto session = SessionManager::getInstance();
                sqlite3_bind_text(stmt, 1, session->getCurrentUsername().c_str(), -1, SQLITE_TRANSIENT);
                if (sqlite3_step(stmt) == SQLITE_ROW)
                {
                    const char* oldPassword = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                    if (oldPassword != nullptr)
                    {
                        samePassword = (newPassword == std::string(oldPassword));
                    }
                }
                sqlite3_finalize(stmt);
            }
            sqlite3_close(db);
        }

        if (samePassword)
        {
            // Show error message
            auto errorLabel = Label::createWithSystemFont("New password cannot be the same as old password", "fonts/Marker Felt.ttf", 18);
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
    }

    // Check if passwords match
    if (newPassword == confirmNewPassword)
    {
        // Update password in SQLite database
        sqlite3* db;
        int rc = sqlite3_open("users.db", &db);

        if (rc == SQLITE_OK)
        {
            // Start a transaction
            rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
            if (rc != SQLITE_OK)
            {
                CCLOG("Error starting transaction: %s", sqlite3_errmsg(db));
                sqlite3_close(db);
                return;
            }

            // Prepare SQL statement to update password
            const char* sql = "UPDATE users SET password = ? WHERE username = ?;";
            sqlite3_stmt* stmt;

            rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

            if (rc == SQLITE_OK)
            {
                // Bind parameters
                sqlite3_bind_text(stmt, 1, newPassword.c_str(), -1, SQLITE_TRANSIENT);
                auto session = SessionManager::getInstance();
                sqlite3_bind_text(stmt, 2, session->getCurrentUsername().c_str(), -1, SQLITE_TRANSIENT);

                // Execute the statement
                if (sqlite3_step(stmt) == SQLITE_DONE)
                {
                    CCLOG("Password updated successfully");
                    passwordChanged = true;
                }
                else
                {
                    CCLOG("Error updating password: %s", sqlite3_errmsg(db));
                }

                sqlite3_finalize(stmt);
            }
            else
            {
                CCLOG("Error preparing SQL statement: %s", sqlite3_errmsg(db));
            }

            // Commit or rollback transaction based on result
            if (passwordChanged)
            {
                sqlite3_exec(db, "COMMIT TRANSACTION;", nullptr, nullptr, nullptr);
            }
            else
            {
                sqlite3_exec(db, "ROLLBACK TRANSACTION;", nullptr, nullptr, nullptr);
            }

            sqlite3_close(db);
        }
        else
        {
            CCLOG("Error opening database: %s", sqlite3_errmsg(db));
        }

        if (passwordChanged)
        {
            // Show success message
            auto successLabel = Label::createWithSystemFont("Password changed successfully", "fonts/Marker Felt.ttf", 18);
            successLabel->setColor(Color3B::GREEN);
            auto visibleSize = Director::getInstance()->getVisibleSize();
            Vec2 origin = Director::getInstance()->getVisibleOrigin();
            successLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 150));
            changePasswordLayer->addChild(successLabel);

            // Remove success message after 2 seconds
            auto delay = DelayTime::create(1.0f);
            auto removeLabel = CallFunc::create([successLabel, this]() {
                successLabel->removeFromParentAndCleanup(true);
                if (changePasswordLayer != nullptr)
                {
                    changePasswordLayer->setVisible(false);
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
                });
            auto sequence = Sequence::create(delay, removeLabel, nullptr);
            successLabel->runAction(sequence);
        }
        else
        {
            // Show error message
            auto errorLabel = Label::createWithSystemFont("Failed to change password", "fonts/Marker Felt.ttf", 18);
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
        }
    }
    else
    {
        // Show error message
        auto errorLabel = Label::createWithSystemFont("Passwords do not match", "fonts/Marker Felt.ttf", 18);
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
    }
}

void HelloWorld::menuCancelChangePasswordCallback(cocos2d::Ref* pSender)
{
    // Hide change password layer
    if (changePasswordLayer != nullptr)
    {
        changePasswordLayer->setVisible(false);
        // Clear input fields
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
    // Hide logout confirmation layer
    if (logoutConfirmLayer != nullptr)
    {
        logoutConfirmLayer->setVisible(false);
    }

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

    // Update registration result message
    if (registerResultLabel != nullptr)
    {
        // Check if database is full (max 3 accounts)
        if (username.empty())
        {
            registerResultLabel->setString("Registration Failed: Username too short");
            registerResultLabel->setColor(Color3B::RED);
        }
        // Validate password length (6-16 characters)
        else if (password.length() < 6 || password.length() > 16)
        {
            registerResultLabel->setString("Registration Failed: Password length 6-16 chars");
            registerResultLabel->setColor(Color3B::RED);
        }
        // Check if password and confirm password match
        else if (password != confirmPassword)
        {
            registerResultLabel->setString("Registration Failed: Passwords do not match");
            registerResultLabel->setColor(Color3B::RED);
        }
        // Check if username already exists in SQLite database
        else
        {
            sqlite3* db;
            int rc = sqlite3_open("users.db", &db);


            if (rc == SQLITE_OK)
            {
                // Start a transaction
                rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
                if (rc != SQLITE_OK)
                {
                    registerResultLabel->setString("Registration Failed: Database error");
                    registerResultLabel->setColor(Color3B::RED);
                    sqlite3_close(db);
                    return;
                }

                bool usernameExists = false;

                // Prepare SQL statement to check if username exists
                const char* sql = "SELECT username FROM users WHERE username = ?;";
                sqlite3_stmt* stmt;

                rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

                if (rc == SQLITE_OK)
                {
                    // Bind username parameter
                    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

                    // Execute the query
                    if (sqlite3_step(stmt) == SQLITE_ROW)
                    {
                        usernameExists = true;
                    }

                    sqlite3_finalize(stmt);
                }

                if (usernameExists)
                {
                    sqlite3_exec(db, "ROLLBACK TRANSACTION;", nullptr, nullptr, nullptr);
                    registerResultLabel->setString("Registration Failed: Username already exists");
                    registerResultLabel->setColor(Color3B::RED);
                }
                else
                {
                    // Prepare SQL statement to insert new user
                    const char* insertSql = "INSERT INTO users (username, password) VALUES (?, ?);";
                    sqlite3_stmt* insertStmt;

                    rc = sqlite3_prepare_v2(db, insertSql, -1, &insertStmt, nullptr);

                    if (rc == SQLITE_OK)
                    {
                        // Bind parameters
                        sqlite3_bind_text(insertStmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
                        sqlite3_bind_text(insertStmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

                        // Execute the statement
                        rc = sqlite3_step(insertStmt);

                        if (rc == SQLITE_DONE)
                        {
                            sqlite3_exec(db, "COMMIT TRANSACTION;", nullptr, nullptr, nullptr);
                            registerResultLabel->setString("Registration Success");
                            registerResultLabel->setColor(Color3B::GREEN);

                            // Add a 1-second delay before hiding the register layer
                            if (registerLayer != nullptr)
                            {
                                auto delay = DelayTime::create(1.0f);
                                auto hideLayer = CallFunc::create([this]() {
                                    if (registerLayer != nullptr)
                                    {
                                        registerLayer->setVisible(false);
                                    }
                                    // Clear input fields after successful registration
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
                                    // Clear registration result message
                                    if (registerResultLabel != nullptr)
                                    {
                                        registerResultLabel->setString("");
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
                                    });
                                auto sequence = Sequence::create(delay, hideLayer, nullptr);
                                registerLayer->runAction(sequence);
                            }
                        }
                        else
                        {
                            sqlite3_exec(db, "ROLLBACK TRANSACTION;", nullptr, nullptr, nullptr);
                            registerResultLabel->setString("Registration Failed: Database error");
                            registerResultLabel->setColor(Color3B::RED);
                        }

                        sqlite3_finalize(insertStmt);
                    }
                    else
                    {
                        sqlite3_exec(db, "ROLLBACK TRANSACTION;", nullptr, nullptr, nullptr);
                        registerResultLabel->setString("Registration Failed: Database error");
                        registerResultLabel->setColor(Color3B::RED);
                    }
                }

                sqlite3_close(db);
            }
            else
            {
                registerResultLabel->setString("Registration Failed: Database error");
                registerResultLabel->setColor(Color3B::RED);
            }
        }
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
