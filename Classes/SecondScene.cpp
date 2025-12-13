#if 1
#include "SecondScene.h"
#include "HelloWorldScene.h"
//添加精灵的坐标依附于父母的左下角，待添加的精灵的坐标是其中心坐标
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

    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 添加返回按钮
    auto backItem = MenuItemImage::create("btn_normal.png", "btn_pressed.png",
        CC_CALLBACK_1(SecondScene::menuFirstCallback, this));
    if (backItem == nullptr ||
        backItem->getContentSize().width <= 0 ||
        backItem->getContentSize().height <= 0)
    {
        problemLoading("'btn_normal.png' and 'btn_pressed.png'");
    }
    else
    {
        //坐标左上角
        float x = origin.x + backItem->getContentSize().width / 2;
        float y = origin.y + visibleSize.height - backItem->getContentSize().height / 2;
        backItem->setPosition(Vec2(x, y));//setPosition设置的坐标是相对于父节点的坐标

        auto backLabel = Label::createWithSystemFont("BACK", "fonts/Marker Felt.ttf", 24);
        backLabel->setColor(Color3B::WHITE);
        backLabel->setPosition(Vec2(backItem->getContentSize().width / 2, backItem->getContentSize().height / 2));
        backItem->addChild(backLabel);//backItem是父节点，孩子是backLabel
    }

    //添加建造按钮
    auto buildItem = MenuItemImage::create("btn_normal.png", "btn_pressed.png",
        CC_CALLBACK_1(SecondScene::menuBuildCallback, this));
    if (buildItem == nullptr ||
        buildItem->getContentSize().width <= 0 ||
        buildItem->getContentSize().height <= 0)

    {
        problemLoading("'btn_normal.png' and 'btn_pressed.png'");
    }
    else
    {
        //坐标左上角,暂紧邻返回键
        float x = origin.x + buildItem->getContentSize().width / 2; // 和返回按钮同X
        float y = origin.y + backItem->getPositionY() - buildItem->getContentSize().height; // 紧挨着返回按钮下方
        buildItem->setPosition(Vec2(x, y));

        auto buildLabel = Label::createWithSystemFont("BUILD", "fonts/Marker Felt.ttf", 24);
        buildLabel->setColor(Color3B::WHITE);
        buildLabel->setPosition(Vec2(buildItem->getContentSize().width / 2, buildItem->getContentSize().height / 2));
        buildItem->addChild(buildLabel);
    }

    auto menu = Menu::create(backItem, buildItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    //下为建造面板的具体实现
    buildPanel = Node::create();
    //坐标
    float buildPanelX = buildItem->getContentSize().width;
    float buildPanelY = 0;
    buildPanel->setPosition(Vec2(buildPanelX, buildPanelY));
    buildPanel->setVisible(false); // 默认隐藏
    buildItem->addChild(buildPanel, 1); // 层级1：高于底层背景（0），低于按钮（2）

    // 3.2 建造面板背景图（替换之前的半透明Layer）
    auto panelBg = Sprite::create("btn_long.png"); // 建造面板背景图
    if (panelBg == nullptr) {
        problemLoading("'btn_long.png'");
    }
    else {
        float panelBgX = panelBg->getContentSize().width/2 ;
        float panelBgY = buildItem->getContentSize().height -panelBg->getContentSize().height / 2;
        panelBg->setPosition(Vec2(panelBgX,panelBgY)); // 相对于根节点
        buildPanel->addChild(panelBg);
    }

    //面板内加示例建筑按钮（未来可加更多）
    // 示例1：房子按钮（图片按钮）
    auto houseBtn = MenuItemImage::create(
        "HelloWorld.png",  // 正常状态
        "HelloWorld.png", // 按下状态
        [=](Ref* pSender) {       // 点击逻辑（未来写建造房子的代码）
            log("点击建造房子！");
        }
    );
    if (houseBtn) {
        houseBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height- houseBtn->getContentSize().height * 0.5/2-10)); // 正上方
    }
    houseBtn->setScale(0.5f);//缩放

    // 示例2：仓库按钮（图片按钮）
    auto storageBtn = MenuItemImage::create(
        "HelloWorld.png",
        "HelloWorld.png",
        [=](Ref* pSender) {
            log("点击建造仓库！");
        }
    );
    if (storageBtn) {
        storageBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - houseBtn->getContentSize().height*0.5 *1.5-10-10)); //10是两个建筑之间的间隔
    }
    storageBtn->setScale(0.5f);

    // 面板内的关闭按钮
    auto closeBtn = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        [=](Ref* pSender) { // 点击关闭面板
            buildPanel->setVisible(false);
        }
    );
    if (closeBtn) {
        closeBtn->setPosition(Vec2(panelBg->getContentSize().width / 2, panelBg->getContentSize().height - closeBtn->getContentSize().height*0.5/2)); // 面板底部
        closeBtn->setScale(0.5f); // 缩小一点
    }

    // 把面板内的按钮加到菜单
    auto panelMenu = Menu::create(houseBtn, storageBtn, closeBtn, nullptr);
    panelMenu->setPosition(Vec2::ZERO);
    panelBg->addChild(panelMenu);

    auto label = Label::createWithTTF("Your Clan!!!", "fonts/Marker Felt.ttf", 36);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        //上居中
        label->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height - label->getContentSize().height));
        this->addChild(label, 1);
    }
    // add "HelloWorld" splash screen"
    background_sprite = Sprite::create("normal(winter).jpg");
    if (background_sprite == nullptr)
    {
        problemLoading("'normal(winter).jpg'");
    }
    else
    {
        // position the sprite on the center of the screen
        background_sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        float scale = visibleSize.width / background_sprite->getContentSize().width * 1.5f;
        background_sprite->setScale(scale);
        // add the sprite as a child to this layer
        this->addChild(background_sprite, 0);//第二个参数代表图层，越大越靠上
    }

    //初始化拖拽相关变量
    is_dragging = false;
    drag_start_position = Vec2::ZERO;
    background_start_position = Vec2::ZERO;

    //添加触摸事件监听器
    auto touch_listener = EventListenerTouchOneByOne::create();
    touch_listener->setSwallowTouches(true);
    touch_listener->onTouchBegan = CC_CALLBACK_2(SecondScene::onTouchBegan, this);
    touch_listener->onTouchMoved = CC_CALLBACK_2(SecondScene::onTouchMoved, this);
    touch_listener->onTouchEnded = CC_CALLBACK_2(SecondScene::onTouchEnded, this);
    touch_listener->onTouchCancelled = CC_CALLBACK_2(SecondScene::onTouchCancelled, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);

    return true;
}

void SecondScene::menuFirstCallback(Ref* pSender)
{
    // 返回主场景
    Director::getInstance()->replaceScene(HelloWorld::createScene());
}

void SecondScene::menuBuildCallback(Ref* pSender)
{
    buildPanel->setVisible(true);
}

bool SecondScene::onTouchBegan(Touch* touch, Event* event)
{
    //记录触摸开始的位置和背景图当前位置
    drag_start_position = touch->getLocation();
    background_start_position = background_sprite->getPosition();
    is_dragging = true;
    return true;
}

void SecondScene::onTouchMoved(Touch* touch, Event* event)
{
    if (!is_dragging)
        return;
    //计算触摸移动的偏移量
    auto current_touch_position = touch->getLocation();
    auto offset = current_touch_position - drag_start_position;

    //计算新位置
    auto new_position = background_start_position + offset;

    //获取屏幕大小和背景图信息
    auto visible_size = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    float background_width = background_sprite->getContentSize().width * background_sprite->getScale();
    float background_height = background_sprite->getContentSize().height * background_sprite->getScale();

    //计算边界限制
    float minX = origin.x + visible_size.width / 2;
    float maxX = origin.x + visible_size.width / 2;
    float minY = origin.y + visible_size.height / 2;
    float maxY = origin.y + visible_size.height / 2;

    //如果背景图比屏幕大，则设置边界
    if (background_width > visible_size.width)
    {
        minX -= (background_width - visible_size.width) / 2;
        maxX += (background_width - visible_size.width) / 2;
    }

    if (background_height > visible_size.width)
    {
        minY -= (background_height - visible_size.height) / 2;
        maxY += (background_height - visible_size.height) / 2;
    }

    //应用边界限制
    new_position.x = clampf(new_position.x, minX, maxX);
    new_position.y = clampf(new_position.y, minY, maxY);

    //更新背景图位置
    background_sprite->setPosition(new_position);
}

void SecondScene::onTouchEnded(Touch* touch, Event* event)
{
    //结束拖拽
    is_dragging = false;
}

void SecondScene::onTouchCancelled(Touch* touch, Event* event)
{
    //取消拖拽
    is_dragging = false;
}
#endif