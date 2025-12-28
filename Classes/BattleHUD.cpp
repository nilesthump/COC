#include "BattleHUD.h"

USING_NS_CC;

BattleHUD* BattleHUD::create(std::function<void(Ref*)> onBack, std::function<void(UnitType)> onSelect)
{
    auto hud = new (std::nothrow) BattleHUD();
    if (hud && hud->init(onBack, onSelect))
    {
        hud->autorelease();
        return hud;
    }
    CC_SAFE_DELETE(hud);
    return nullptr;
}

bool BattleHUD::init(std::function<void(Ref*)> onBack, std::function<void(UnitType)> onSelect)
{
    if (!Layer::init()) return false;

    //初始化底部选择面板
    selection_panel_ = UnitSelectionPanel::create(onSelect);
    this->addChild(selection_panel_, 10);

    //初始化顶部 UI (返回、计时、资源)
    setupTopBar(onBack);

    //初始化战报 (百分比和星星)
    setupWarStatus();

    this->scheduleUpdate();
    return true;
}

void BattleHUD::setupTopBar(std::function<void(Ref*)> onBack)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    //返回按钮
    auto backItem = MenuItemImage::create("btn_normal.png", "btn_pressed.png", onBack);
    auto backLabel = Label::createWithTTF("BACK", "fonts/Marker Felt.ttf", 24);
    backLabel->setPosition(Vec2(backItem->getContentSize().width / 2, backItem->getContentSize().height / 2));
    backItem->addChild(backLabel);

    auto menu = Menu::create(backItem, nullptr);
    menu->setPosition(origin.x + backItem->getContentSize().width / 2, origin.y + visibleSize.height - backItem->getContentSize().height / 2);
    this->addChild(menu, 1);

    //倒计时
    countdown_label_ = Label::createWithSystemFont("Time: 180", "Arial", 24);
    countdown_label_->setColor(Color3B::RED);
    countdown_label_->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 30);
    this->addChild(countdown_label_, 1);

    //资源显示
    //金币
    auto gIcon = Sprite::create("GoldCoin.png");
    gIcon->setScale(0.5f);
    gIcon->setPosition(visibleSize.width - 180, visibleSize.height - 30);
    this->addChild(gIcon);

    gold_label_ = Label::createWithSystemFont("0", "Arial", 20);
    gold_label_->setAnchorPoint(Vec2(0, 0.5f));
    gold_label_->setPosition(visibleSize.width - 160, visibleSize.height - 30);
    this->addChild(gold_label_);

    //圣水
    auto eIcon = Sprite::create("Elixir.png");
    eIcon->setScale(0.5f);
    eIcon->setPosition(visibleSize.width - 80, visibleSize.height - 30);
    this->addChild(eIcon);

    elixir_label_ = Label::createWithSystemFont("0", "Arial", 20);
    elixir_label_->setAnchorPoint(Vec2(0, 0.5f));
    elixir_label_->setPosition(visibleSize.width - 60, visibleSize.height - 30);
    this->addChild(elixir_label_);
}

void BattleHUD::setupWarStatus()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    //放在面板高度以上的右侧
    war_status_node_ = Node::create();
    war_status_node_->setPosition(visibleSize.width - 120, 160);
    this->addChild(war_status_node_, 11);

    float starSpacing = 80.0f;
    float labelToStarY = 60.0f;
    //百分比
    percent_label_ = Label::createWithSystemFont("0%", "Arial", 30);
    percent_label_->enableOutline(Color4B::BLACK, 2);
    war_status_node_->addChild(percent_label_);
    percent_label_->setPosition(Vec2(0, labelToStarY));

    // 星星位置
    for (int i = 0; i < 3; i++)
    {
        stars_[i] = Sprite::create("star_empty.png");
        if (stars_[i])
        {
            stars_[i]->setScale(2.0f);
            float posX = (i - 1) * starSpacing;
            stars_[i]->setPosition(Vec2(posX, 0));
            war_status_node_->addChild(stars_[i]);
        }
    }
}

void BattleHUD::updateHUD(float dt)
{
    auto bm = BattleManager::getInstance();
    const auto& score = bm->getCurrentScore();

    //更新倒计时
    countdown_label_->setString(StringUtils::format("Time: %d", (int)ceil(bm->GetRemainTime())));

    //更新资源数字
    gold_label_->setString(std::to_string(score.gold_collected));
    elixir_label_->setString(std::to_string(score.elixir_collected));

    //更新战损百分比
    int percent = (int)(score.getPercent() * 100);
    percent_label_->setString(StringUtils::format("%d%%", percent));

    //更新星星
    BattleStar starStatus = bm->CalculateStars();
    for (int i = 0; i < 3; i++)
    {
        if (i < (int)starStatus && stars_[i]->getName() != "bright")
        {
            if (stars_[i]->getOpacity() < 255 || stars_[i]->getTag() != 100)
            {
                stars_[i]->setTexture("star_bright.png");
                stars_[i]->setTag(100); // 用 tag 标记已亮起

                // 弹跳效果：迅速放大再回到原位
                auto scaleUp = ScaleTo::create(2.2f, 4.0f);
                auto scaleDown = ScaleTo::create(1.0f, 2.5f);
                stars_[i]->runAction(Sequence::create(scaleUp, scaleDown, nullptr));
            }
        }
    }

    //更新兵种数量
    auto attackerMap = CombatSessionManager::getInstance()->battle_start_params.attackerInventory;
    // 这里需要同步 Manager 内部实时剩余的兵力，建议 Manager 提供 GetRemainingCount(type)
    // 暂时用这种逻辑：
    for (int i = (int)UnitType::BARBARIAN; i <= (int)UnitType::BALLOON; i++)
    {
        UnitType t = (UnitType)i;
        selection_panel_->updateUnitCount(t, bm->getRemainingUnitCount(t));
    }
}

void BattleHUD::addUnitIcon(const std::string& path, const std::string& name, UnitType type)
{
    if (selection_panel_)
    {
        selection_panel_->addUnitIcon(path, name, type);
    }
}