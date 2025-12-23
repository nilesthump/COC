#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "ui/UIEditBox/UIEditBox.h"
#include "sqlite3.h"

class HelloWorld : public cocos2d::Scene,public cocos2d::ui::EditBoxDelegate
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    void menuSecondSceneCallback(cocos2d::Ref* pSender);
    void menuBattleTestCallback(cocos2d::Ref* pSender);

    void menuLoginCallback(cocos2d::Ref* pSender);
    void menuConfirmCallback(cocos2d::Ref* pSender);
    void menuCancelLoginCallback(cocos2d::Ref* pSender);

    // 添加删除账号相关的回调函数声明
    void menuDeleteAccountCallback(cocos2d::Ref* pSender);
    void menuConfirmDeleteCallback(cocos2d::Ref* pSender);
    void menuCancelDeleteCallback(cocos2d::Ref* pSender);

    void menuLogoutCallback(cocos2d::Ref* pSender);
    void menuConfirmLogoutCallback(cocos2d::Ref* pSender);
    void menuCancelLogoutCallback(cocos2d::Ref* pSender);

    void menuRegisterCallback(cocos2d::Ref* pSender);
    void menuRegisterConfirmCallback(cocos2d::Ref* pSender);
    void menuCancelRegisterCallback(cocos2d::Ref* pSender);

    void menuGuestLoginCallback(cocos2d::Ref* pSender);

    // EditBox delegate functions
    virtual void editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox) override;
    virtual void editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text) override;
    virtual void editBoxReturn(cocos2d::ui::EditBox* editBox) override;
    virtual void editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox,
        cocos2d::ui::EditBoxDelegate::EditBoxEndAction action) override;
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:
    cocos2d::MenuItemImage* secondSceneItem;
    cocos2d::MenuItemImage* battleTestItem;
    cocos2d::MenuItemImage* guestLoginItem;
    cocos2d::MenuItemImage* loginItem;
    cocos2d::MenuItemImage* registerItem;
    cocos2d::MenuItemImage* confirmItem;
    cocos2d::MenuItemImage* cancelLoginItem;
    cocos2d::MenuItemImage* registerConfirmItem;
    cocos2d::MenuItemImage* cancelRegisterItem;

    cocos2d::MenuItemImage* confirmLogoutItem;
    cocos2d::MenuItemImage* cancelLogoutItem;
    cocos2d::MenuItemImage* deleteAccountItem;
    cocos2d::MenuItemImage* confirmDeleteItem;
    cocos2d::MenuItemImage* cancelDeleteItem;

    // Login specific edit boxes
    cocos2d::ui::EditBox* loginUsernameEditBox;
    cocos2d::ui::EditBox* loginPasswordEditBox;

    // Register specific edit boxes
    cocos2d::ui::EditBox* registerUsernameEditBox;
    cocos2d::ui::EditBox* registerPasswordEditBox;
    cocos2d::ui::EditBox* confirmPasswordEditBox;

    cocos2d::LayerColor* loginLayer;
    cocos2d::LayerColor* registerLayer;
    cocos2d::LayerColor* logoutConfirmLayer;
    cocos2d::LayerColor* deleteAccountConfirmLayer;

    cocos2d::Label* usernameLabel;
    cocos2d::Label* passwordLabel;
    cocos2d::Label* confirmPasswordLabel;
    cocos2d::Label* guestLoginLabel;
    cocos2d::Label* loginLabel;
    cocos2d::Label* registerLabel;
    cocos2d::Label* registerResultLabel;
    cocos2d::Label* deleteAccountLabel;
    cocos2d::Label* welcomeLabel;

    static bool isLoggedIn;
    std::string currentLoggedInUser;
};

#endif // __HELLOWORLD_SCENE_H__
