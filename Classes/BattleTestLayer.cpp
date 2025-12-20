#include "BattleTestLayer.h"
#include "UnitFactory.h"
#include "HelloWorldScene.h"

USING_NS_CC;
void DebugFullGrid(Node* gameWorld, Sprite* backgroundSprite, int gridStep ,bool showLabels );

Scene* BattleTestLayer::createScene()
{
	auto scene = Scene::create();
	auto layer = BattleTestLayer::create();
	scene->addChild(layer);
	return scene;
}

bool BattleTestLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	// 设置场景
	setupBattleScene();
	// 启动更新循环
	this->scheduleUpdate();

	return true;
}

static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

void BattleTestLayer::setupBattleScene()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	auto backItem = MenuItemImage::create("btn_normal.png", "btn_pressed.png",
		CC_CALLBACK_1(BattleTestLayer::menuFirstCallback, this));
	if (backItem == nullptr ||
		backItem->getContentSize().width <= 0 ||
		backItem->getContentSize().height <= 0)
	{
		problemLoading("'btn_normal.png' and 'btn_pressed.png'");
	}
	else
	{

		double x = origin.x + backItem->getContentSize().width / 2;
		double y = origin.y + visibleSize.height - backItem->getContentSize().height / 2;
		backItem->setPosition(Vec2(x, y));

		auto backLabel = Label::createWithSystemFont("BACK", "fonts/Marker Felt.ttf", 24);
		backLabel->setColor(Color3B::WHITE);
		backLabel->setPosition(Vec2(backItem->getContentSize().width / 2, backItem->getContentSize().height / 2));
		backItem->addChild(backLabel);
	}

	auto menu = Menu::create(backItem, nullptr);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	// 1. 加载背景地图
	background_sprite_ = Sprite::create("normal(winter).jpg");
	if (background_sprite_)
	{
		// 背景精灵居中
		background_sprite_->setPosition(Vec2(
			visibleSize.width / 2 + origin.x,
			visibleSize.height / 2 + origin.y
		));

		// 设置缩放（和SecondScene相同）
		float scale = visibleSize.width / background_sprite_->getContentSize().width * 1.5f;
		background_sprite_->setScale(scale);

		this->addChild(background_sprite_, 0);

		// 2. 创建游戏世界容器（作为背景的子节点，这样会随着背景一起移动缩放）
		game_world_ = Node::create();
		// 设置游戏世界位置为背景图像的中心
		game_world_->setPosition(background_sprite_->getContentSize().width / 2, background_sprite_->getContentSize().height / 2);
		background_sprite_->addChild(game_world_, 1); // 添加到背景中

		// 3. 初始化缩放滚动管理器（和SecondScene一样）
		zoom_manager_ = ZoomScrollManager::create(background_sprite_, 0.5f, 2.0f);
		this->addChild(zoom_manager_);

		// 4. 初始化菱形网格管理器（使用您提供的参数）
		int left_x = 667;
		int right_x = 3705 - 556;
		int top_y = 264;
		int bottom_y = 2545 - 471;

		float diamond_width = right_x - left_x;
		float diamond_height = bottom_y - top_y;

		// 计算菱形中心相对于背景精灵中心的位置
		Vec2 diamond_center_absolute = Vec2(
			(left_x + right_x) / 2.0f,
			(top_y + bottom_y) / 2.0f
		);

		// 这里的关键：菱形中心是相对于背景精灵的本地坐标
		Vec2 diamond_center = diamond_center_absolute - Vec2(
			background_sprite_->getContentSize().width / 2,
			background_sprite_->getContentSize().height / 2
		);

		// 创建菱形网格管理器，添加到game_world_
		grid_manager_ = DiamondGridManager::create(diamond_center, diamond_width, diamond_height, 44);
		game_world_->addChild(grid_manager_, 1);

		// 5. 初始化战斗管理器
		battle_manager_ = std::make_unique<BattleManager>();
		battle_manager_->SetBattleActive(true);
        DebugFullGrid(game_world_, background_sprite_,2,true);
		placeCannon();
		placeBarbarian();
	}

}

void BattleTestLayer::placeBarbarian()
{
	BattleUnit* barbarian = UnitFactory::CreateCompleteBarbarian(1, game_world_, background_sprite_);
	if (barbarian)
	{
		barbarian->SetPosition(0, 0);
		battle_manager_->AddUnit(std::unique_ptr<BattleUnit>(barbarian), true);
	}
}

void BattleTestLayer::placeCannon()
{
	BattleUnit* cannon_unit = UnitFactory::CreateCompleteCannon(1, game_world_, background_sprite_);
	if (cannon_unit)
	{
		cannon_unit->SetPosition(10, 10);
		battle_manager_->AddUnit(std::unique_ptr<BattleUnit>(cannon_unit), false);
	}
}


void BattleTestLayer::update(float delta)
{
	// 更新战斗管理器
	if (battle_manager_)
	{
		battle_manager_->Update(delta);
	}
}

void BattleTestLayer::menuFirstCallback(Ref* pSender)
{

	Director::getInstance()->replaceScene(HelloWorld::createScene());
}

/**
 * 在所有网格位置绘制标记
 * @param gameWorld 游戏世界节点
 * @param backgroundSprite 背景精灵
 * @param gridStep 网格间隔（1=全部显示，2=每隔一个，4=每隔3个）
 * @param showLabels 是否显示坐标文字
 */
void DebugFullGrid(Node* gameWorld, Sprite* backgroundSprite, int gridStep , bool showLabels)
{
    if (!gameWorld || !backgroundSprite)
        return;

    // 创建一个容器节点，方便统一管理
    auto gridContainer = Node::create();
    gridContainer->setName("GridDebugContainer");
    gameWorld->addChild(gridContainer, 1000);

    // 绘制所有网格点
    int pointCount = 0;
    for (int gridX = 0; gridX <= 43; gridX += gridStep)
    {
        for (int gridY = 0; gridY <= 43; gridY += gridStep)
        {
            Vec2 local_pos = ConvertTest::convertGridToLocal(
                gridX, gridY, backgroundSprite
            );

            // 根据位置设置不同的颜色
            Color3B color;
            float markerSize = 3.0f;

            // 边界点用特殊颜色
            if (gridX == 0 && gridY == 0)
            {
                color = Color3B::RED;      // 左下角
                markerSize = 6.0f;
            }
            else if (gridX == 43 && gridY == 43)
            {
                color = Color3B::BLUE;     // 右上角
                markerSize = 6.0f;
            }
            else if (gridX == 0 && gridY == 43)
            {
                color = Color3B::GREEN;    // 左上角
                markerSize = 6.0f;
            }
            else if (gridX == 43 && gridY == 0)
            {
                color = Color3B::YELLOW;   // 右下角
                markerSize = 6.0f;
            }
            else if (gridX == 21 && gridY == 21)
            {
                color = Color3B::WHITE;    // 中心点
                markerSize = 5.0f;
            }
            else if (gridX == 0 || gridX == 43 || gridY == 0 || gridY == 43)
            {
                color = Color3B::ORANGE;   // 边界
                markerSize = 4.0f;
            }
            else
            {
                color = Color3B(150, 150, 150);  // 普通点
                markerSize = 3.0f;
            }

            // 创建标记点
            auto drawNode = DrawNode::create();
            drawNode->drawSolidCircle(Vec2::ZERO, markerSize, 0, 16, Color4F(color));
            drawNode->setPosition(local_pos);
            gridContainer->addChild(drawNode);

            // 对于关键点或稀疏网格，添加坐标标签
            bool shouldShowLabel = false;

            if (gridStep >= 4)
            {
                // 如果网格很稀疏，所有点都显示标签
                shouldShowLabel = showLabels;
            }
            else
            {
                // 只显示关键点的标签
                shouldShowLabel = showLabels && (
                    (gridX == 0 || gridX == 43 || gridX == 21) &&
                    (gridY == 0 || gridY == 43 || gridY == 21)
                    );
            }

            if (shouldShowLabel)
            {
                auto label = Label::createWithSystemFont(
                    StringUtils::format("(%d,%d)", gridX, gridY),
                    "Arial",
                    14
                );
                label->setColor(color);
                label->setPosition(local_pos + Vec2(0, 20));
                gridContainer->addChild(label);
            }

            pointCount++;
        }
    }

    CCLOG("已绘制 %d 个网格点", pointCount);

    // 绘制网格线
    auto gridLines = DrawNode::create();
    gridLines->setName("GridLines");

    // 绘制水平线（固定gridY，变化gridX）
    for (int gridY = 0; gridY <= 43; gridY += (gridStep * 2))
    {
        Vec2 start = ConvertTest::convertGridToLocal(0, gridY, backgroundSprite);
        Vec2 end = ConvertTest::convertGridToLocal(43, gridY, backgroundSprite);
        gridLines->drawLine(start, end, Color4F(0.5f, 0.5f, 0.5f, 0.5f));
    }

    // 绘制垂直线（固定gridX，变化gridY）
    for (int gridX = 0; gridX <= 43; gridX += (gridStep * 2))
    {
        Vec2 start = ConvertTest::convertGridToLocal(gridX, 0, backgroundSprite);
        Vec2 end = ConvertTest::convertGridToLocal(gridX, 43, backgroundSprite);
        gridLines->drawLine(start, end, Color4F(0.5f, 0.5f, 0.5f, 0.5f));
    }

    gridContainer->addChild(gridLines, -1);

    // 绘制边界框（红色加粗）
    auto boundaryDraw = DrawNode::create();
    boundaryDraw->setName("Boundary");

    Vec2 corners[] = {
        ConvertTest::convertGridToLocal(0, 0, backgroundSprite),
        ConvertTest::convertGridToLocal(43, 0, backgroundSprite),
        ConvertTest::convertGridToLocal(43, 43, backgroundSprite),
        ConvertTest::convertGridToLocal(0, 43, backgroundSprite)
    };

    for (int i = 0; i < 4; i++)
    {
        boundaryDraw->drawLine(
            corners[i],
            corners[(i + 1) % 4],
            Color4F::RED
        );
    }

    gridContainer->addChild(boundaryDraw, 10);

    CCLOG("========================================");
}