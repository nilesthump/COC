#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "ui/UIEditBox/UIEditBox.h"

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
    void menuLogoutCallback(cocos2d::Ref* pSender);
    void menuConfirmLogoutCallback(cocos2d::Ref* pSender);
    void menuCancelLogoutCallback(cocos2d::Ref* pSender);
    void menuRegisterCallback(cocos2d::Ref* pSender);
    void menuRegisterConfirmCallback(cocos2d::Ref* pSender);
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
    cocos2d::MenuItemImage* registerConfirmItem;

    cocos2d::MenuItemImage* confirmLogoutItem;
    cocos2d::MenuItemImage* cancelLogoutItem;

    cocos2d::ui::EditBox* usernameEditBox;
    cocos2d::ui::EditBox* passwordEditBox;
    cocos2d::ui::EditBox* confirmPasswordEditBox;

    cocos2d::LayerColor* loginLayer;
    cocos2d::LayerColor* registerLayer;
    cocos2d::LayerColor* logoutConfirmLayer;

    cocos2d::Label* usernameLabel;
    cocos2d::Label* passwordLabel;
    cocos2d::Label* confirmPasswordLabel;
    cocos2d::Label* guestLoginLabel;
    cocos2d::Label* loginLabel;
    cocos2d::Label* registerLabel;
    cocos2d::Label* registerResultLabel;

    static bool isLoggedIn;
};

#endif // __HELLOWORLD_SCENE_H__
