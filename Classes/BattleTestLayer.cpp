//宏观是触摸与鼠标监听配到整个layer，background添加到layer一起缩放移动
//game_world(node)作为所有part的父节点是background的孩子，其他part加到game_world中

//于是角色的setposition位置就是内部grid的逻辑位置，通过convertGridToLocal转换成对于game_world（考虑了缩放之类的）的本地位置
//由于game_word是background的孩子，所以BattleUnit更新显示位置时调用convertGridToLocal就可以正确显示
//12/27 重构封装了整个文件，更加清晰
#include "BattleTestLayer.h"
#include "HelloWorldScene.h"
#include "UnitFactory.h"
#include "ConvertTest.h"
#include "BattleResultLayer.h"
#include "CombatSessionManager.h"
#include "ccUtils.h"
#include <ctime>

USING_NS_CC;
void DebugFullGrid(Node* gameWorld, Sprite* backgroundSprite, int gridStep, bool showLabels);

//错误加载
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

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
		return false;
	resetClickState();
	setupUserInterface();
	setupWorldEnvironment();
	setupInputListeners();
	setupBattleSession();

	this->scheduleUpdate();
	return true;
}

//整体更新
void BattleTestLayer::update(float delta)
{
	auto bm = BattleManager::getInstance();
	if (!bm) return;

	//逻辑更新
	bm->Update(delta);

	//UI更新，只管调用HUD的接口，HUD内部会去读取Manager的数据
	if (hud_)
	{
		hud_->updateHUD(delta);
	}

	//结算
	if (bm->HasBattleEnded() && !result_layer_shown_)
	{
		result_layer_shown_ = true;
		showBattleResultLayer(bm->GetBattleResult());
	}
}

//初始化不随移动的UI
void BattleTestLayer::setupUserInterface()
{
	//创建 HUD 并添加到 Layer (这是唯一的入口)
	hud_ = BattleHUD::create(
		CC_CALLBACK_1(BattleTestLayer::menuFirstCallback, this), // 返回逻辑
		[this](UnitType type)
		{                                  // 选兵逻辑
			this->current_selected_unit_ = type;
			this->resetClickState();
		}
	);
	this->addChild(hud_, 1000);

	hud_->addUnitIcon("Barbarian.png", "Barbarian", UnitType::BARBARIAN);
	hud_->addUnitIcon("Archer.png", "Archer", UnitType::ARCHER);
	hud_->addUnitIcon("Giant.png", "Giant", UnitType::GIANT);
	hud_->addUnitIcon("Goblin.png", "Goblin", UnitType::GOBLIN);
	hud_->addUnitIcon("Bomber.png", "Bomber", UnitType::BOMBER);
	hud_->addUnitIcon("Balloon.png", "Balloon", UnitType::BALLOON);
}

//初始化地图、缩放相关
void BattleTestLayer::setupWorldEnvironment()
{
	auto config = CombatSessionManager::getInstance();
	//地图，缩放管理器初始化部分
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	//加载背景地图
	background_sprite_ = Sprite::create(config->mapBackgroundPath);
	if (!background_sprite_)
		return;

	background_sprite_->setPosition(Vec2(
		visibleSize.width / 2 + origin.x,
		visibleSize.height / 2 + origin.y
	));

	float scale = visibleSize.width / background_sprite_->getContentSize().width * 1.5f;
	background_sprite_->setScale(scale);

	this->addChild(background_sprite_, 0);

	//创建game_world容器 
	game_world_ = Node::create();
	//这是系统的setPosition，直接绑定了game_world_和background的坐标关系
	//注意区分实际放置导入grid逻辑坐标的SetPosition（S大写)
	game_world_->setPosition(background_sprite_->getContentSize().width / 2,
		background_sprite_->getContentSize().height / 2);
	background_sprite_->addChild(game_world_, 1);

	//初始化缩放滚动管理器
	zoom_manager_ = ZoomScrollManager::create(background_sprite_, 0.5f, 2.0f);
	this->addChild(zoom_manager_);

	//初始化菱形地图（草坪）管理器
	int left_x = 667;
	int right_x = 3705 - 556;
	int top_y = 264;
	int bottom_y = 2545 - 471;

	float diamond_width = right_x - left_x;
	float diamond_height = bottom_y - top_y;

	Vec2 diamond_center_absolute = Vec2(
		(left_x + right_x) / 2.0f,
		(top_y + bottom_y) / 2.0f
	);

	Vec2 diamond_center = diamond_center_absolute - Vec2(
		background_sprite_->getContentSize().width / 2,
		background_sprite_->getContentSize().height / 2
	);

	////草坪地图部分也作为game_world的孩子
	//grid_manager_ = DiamondGridManager::create(diamond_center, diamond_width, diamond_height, 50);
	//game_world_->addChild(grid_manager_, 1);


	//Debug用函数
	DebugFullGrid(game_world_, background_sprite_, 2, true);
}

//触摸、鼠标交互绑定
void BattleTestLayer::setupInputListeners()
{
	//触摸事件监听
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(BattleTestLayer::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(BattleTestLayer::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(BattleTestLayer::onTouchEnded, this);
	touchListener->onTouchCancelled = CC_CALLBACK_2(BattleTestLayer::onTouchCancelled, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	//鼠标事件监听器
	auto mouseListener = EventListenerMouse::create();
	mouseListener->onMouseScroll = CC_CALLBACK_1(BattleTestLayer::onMouseScroll, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

//战斗启动！
void BattleTestLayer::setupBattleSession()
{
	auto config = CombatSessionManager::getInstance();
	auto bm = BattleManager::getInstance();

	// --- 1. 核心大本营 (4x4) ---
	// 中心点 23.5f，占用索引范围：[22, 23, 24, 25]
	BuildingSnapshot th;
	th.type = BuildingType::TOWN_HALL;
	th.level = 1;
	th.logicalPos = Vec2(23.5f, 23.5f);
	config->battle_start_params.buildings.push_back(th);

	// --- 2. 核心防御塔 (3x3) ---
	// 中心点为整数。放置在紧贴大本营的四个角。
	// 大本营边界是 21.5 和 25.5。防御塔中心选在 20.0 和 27.0。
	// 占用范围：20.0 占 [19, 20, 21]；27.0 占 [26, 27, 28]
	Vec2 defPositions[] = {
		Vec2(20.0f, 20.0f), Vec2(27.0f, 20.0f),
		Vec2(20.0f, 27.0f), Vec2(27.0f, 27.0f)
	};
	BuildingType defTypes[] = { BuildingType::CANNON, BuildingType::ARCHER_TOWER, BuildingType::CANNON, BuildingType::MORTAR };

	for (int i = 0; i < 4; i++)
	{
		BuildingSnapshot def;
		def.type = defTypes[i];
		def.level = 1;
		def.logicalPos = defPositions[i];
		config->battle_start_params.buildings.push_back(def);
	}

	// --- 3. 资源建筑 (3x3) ---
	// 放置在防御塔外侧，确保不与防御塔（19-21, 26-28）冲突
	// 金矿：纵向放在 17.0
	for (int i = 0; i < 4; i++)
	{
		BuildingSnapshot gm;
		gm.type = BuildingType::GOLD_MINE;
		gm.level = 1;
		gm.logicalPos = Vec2(18.0f + i * 3.0f, 16.0f); // 占用 y: [15, 16, 17]
		config->battle_start_params.buildings.push_back(gm);

		BuildingSnapshot ex;
		ex.type = BuildingType::ELIXIR;
		ex.level = 1;
		ex.logicalPos = Vec2(18.0f + i * 3.0f, 31.0f); // 占用 y: [30, 31, 32]
		config->battle_start_params.buildings.push_back(ex);
	}

	// --- 4. 仓库 (3x3) ---
	// 左右护法，避开大本营 (22-25) 和防御塔
	BuildingSnapshot gs;
	gs.type = BuildingType::GOLD_STORAGE;
	gs.level = 1;
	gs.logicalPos = Vec2(16.0f, 23.5f); // 3x3 偶数对齐有点特殊，建议也用整数避开 23.5
	// 修正：仓库也用整数中心 16.0, 23.0，占用 x:[15,16,17], y:[22,23,24]
	gs.logicalPos = Vec2(16.0f, 23.0f);
	config->battle_start_params.buildings.push_back(gs);

	BuildingSnapshot es;
	es.type = BuildingType::ELIXIR_STORAGE;
	es.level = 1;
	es.logicalPos = Vec2(31.0f, 23.0f); // 占用 x:[30,31,32], y:[22,23,24]
	config->battle_start_params.buildings.push_back(es);

	// --- 5. 紧凑型围墙 (1x1) ---
	// 目前最外层建筑边缘：
	// y轴最小 15 (金矿), 最大 32 (圣水)
	// x轴最小 15 (仓库), 最大 32 (仓库)
	// 围墙需要包围这些索引，所以圈定在 14 和 33
	for (int x = 14; x <= 33; x++)
	{
		for (int y = 14; y <= 33; y++)
		{
			// 只在矩形边缘放置墙
			if (x == 14 || x == 33 || y == 14 || y == 33)
			{
				BuildingSnapshot wall;
				wall.type = BuildingType::WALL;
				wall.level = 1;
				wall.logicalPos = Vec2((float)x, (float)y);
				config->battle_start_params.buildings.push_back(wall);
			}
		}
	}

	// --- 6. 兵力与启动 ---
	auto& inv = config->battle_start_params.army;
	inv.clear(); // 确保是空的

	// 创建第一个兵种
	Army barbarian;
	barbarian.type = UnitType::BARBARIAN;
	barbarian.level = 2;
	barbarian.amount = 5;
	inv.push_back(barbarian); // 存入数组

	// 如果有气球兵
	Army balloon;
	balloon.type = UnitType::BALLOON;
	balloon.level = 1;
	balloon.amount = 3;
	inv.push_back(balloon);

	bm->PrepareBattle(config->battle_start_params);
	placeDefender();
	//计算禁放区（内部以及外部一圈）
	bm->GetIndexSystem()->UpdateDeploymentMap();
}

//固定放置（直接放逻辑位置）
void BattleTestLayer::placeDefender()
{
	auto config = CombatSessionManager::getInstance();
	auto bm = BattleManager::getInstance();

	for (const auto& b : config->battle_start_params.buildings)
	{
		// 这里的 CreateBuildingByType 是我们要去 Factory 补全的工具函数
		auto unit = UnitFactory::CreateBuildingByType(b.type, b.level, game_world_, background_sprite_);
		if (unit)
		{
			unit->SetPositionDefender(b.logicalPos.x, b.logicalPos.y);
			bm->AddUnit(std::unique_ptr<BattleUnit>(unit), false);
		}
	}
}

//在给定点的位置放置，作为双击鼠标点击的反应
void BattleTestLayer::placeUnitAt(float gridX, float gridY)
{
	auto bm = BattleManager::getInstance();
	//获取点击位置在 game_world_ 里的本地坐标，用于显示提示文字
	Vec2 hintLocalPos = ConvertTest::convertGridToLocal(gridX, gridY, background_sprite_);

	//前置检查 (没选人或没兵了)
	if (current_selected_unit_ == UnitType::NONE || !bm->CanDeployUnit(current_selected_unit_))
	{
		showDeployHintAtWorldPos(hintLocalPos);
		return;
	}

	if(bm->GetIndexSystem()->CanDeployAt(gridX, gridY))
	{
		auto army_pool1 = bm->GetArmyPool();
		int deployLevel = army_pool1[current_selected_unit_].level;
		auto unit = UnitFactory::CreateAttackerByType(current_selected_unit_, deployLevel, game_world_, background_sprite_);
		if (unit)
		{
			unit->SetPositionAttacker(gridX, gridY);
			bm->AddUnit(std::unique_ptr<BattleUnit>(unit), true);

			//更新 UI
			bm->decrementUnitCount(current_selected_unit_);
		}
	}
	else
	{
		auto tempUnit = UnitFactory::CreateAttackerByType(current_selected_unit_, 1, game_world_, background_sprite_);
		if (tempUnit)
		{
			tempUnit->SetPositionAttacker(gridX, gridY);
			tempUnit->PlayPlacementFailAnimation();
			//延迟自毁
			this->runAction(Sequence::create(DelayTime::create(1.0f), CallFunc::create([tempUnit]() { delete tempUnit; }), nullptr));
		}
	}
	return;
}

//以下是触摸和鼠标事件
//重置点击
void BattleTestLayer::resetClickState()
{
	last_click_time_ = 0;
	last_click_pos_ = Vec2::ZERO;
}

bool BattleTestLayer::onTouchBegan(Touch* touch, Event* event)
{
	touch_begin_time_ = utils::gettime();
	touch_begin_pos_ = touch->getLocation();
	is_dragging_map_ = false;

	// 可选：防止拖拽污染双击
	// last_click_time_ = 0;
	return true;
}

void BattleTestLayer::onTouchMoved(Touch* touch, Event* event)
{
	Vec2 current = touch->getLocation();
	float distance = current.distance(touch_begin_pos_);

	// 超过阈值，判定为拖拽
	if (!is_dragging_map_ && distance > 10.0f)
	{
		is_dragging_map_ = true;
		zoom_manager_->onTouchBegan(touch, event); // 开始拖地图
	}

	if (is_dragging_map_ && zoom_manager_)
	{
		zoom_manager_->onTouchMoved(touch, event);
	}
}

void BattleTestLayer::onTouchEnded(Touch* touch, Event* event)
{
	double now = utils::gettime();
	double duration = now - touch_begin_time_;
	Vec2 end_pos = touch->getLocation();
	float distance = end_pos.distance(touch_begin_pos_);

	// 如果是拖拽，结束拖拽即可
	if (is_dragging_map_)
	{
		zoom_manager_->onTouchEnded(touch, event);
		is_dragging_map_ = false;
		return;
	}

	// 过滤掉按太久 or 移动太多的情况
	if (duration > 0.3f || distance > 15.0f)
		return;

	// 双击判断
	double interval = now - last_click_time_;
	float click_distance = last_click_pos_.distance(end_pos);

	if (interval > 0.1 && interval < 0.5 && click_distance < 30.0f)
	{
		//双击成立
		CCLOG("[DOUBLE CLICK]");

		//直接从点击屏幕位置到gird，真好
		Vec2 gridPos = ConvertTest::convertScreenToGrid(
			end_pos, background_sprite_, game_world_);

		//这里调用了放置具体位置的barbarianAt函数
		placeUnitAt(gridPos.x, gridPos.y);

		//重置
		last_click_time_ = 0;
		last_click_pos_ = Vec2::ZERO;
	}
	else
	{
		// 记录为第一次点击
		last_click_time_ = now;
		last_click_pos_ = end_pos;
	}
}

void BattleTestLayer::onTouchCancelled(Touch* touch, Event* event)
{
	if (is_dragging_map_)
	{
		zoom_manager_->onTouchEnded(touch, event);
	}

	is_dragging_map_ = false;
}

void BattleTestLayer::onMouseScroll(EventMouse* event)
{
	if (zoom_manager_)
	{
		zoom_manager_->onMouseScroll(event);
	}
}

//展示兵力已经满了不让再放置的提示，提示在点击位置附近
void BattleTestLayer::showDeployHintAtWorldPos(const Vec2& worldPos)
{
	std::string msg = "OUT OF UNITS!";
	if (current_selected_unit_ == UnitType::NONE)
		msg = "SELECT A UNIT FIRST!";
	auto label = Label::createWithSystemFont(msg, "fonts/Marker Felt.ttf", 20);
	label->setColor(Color3B::RED);

	// worldPos 是 game_world_ 的本地坐标
	label->setPosition(worldPos + Vec2(0, 20));
	label->setOpacity(255);

	// 挂在 game_world_，跟着缩放 / 拖拽
	game_world_->addChild(label, 5000);

	// 飘字动画
	auto moveUp = MoveBy::create(0.8f, Vec2(0, 40));
	auto fadeOut = FadeOut::create(0.8f);
	auto spawn = Spawn::create(moveUp, fadeOut, nullptr);

	auto cleanup = CallFunc::create([label]()
		{
			label->removeFromParent();
		});

	label->runAction(Sequence::create(spawn, cleanup, nullptr));
}

//展示结算画面
void BattleTestLayer::showBattleResultLayer(BattleResult result)
{
	auto layer = BattleResultLayer::create(result);
	this->addChild(layer, 10000);
}

//返回HelloWorldScene触发
void BattleTestLayer::menuFirstCallback(Ref* pSender)
{
	BattleManager* battleManager = BattleManager::getInstance();
	battleManager->clear(); // 重置战斗管理器状态
	this->unscheduleUpdate(); // 停止更新
	Director::getInstance()->replaceScene(HelloWorld::createScene());
}

//Debug用的放置条格子，辅助用
void DebugFullGrid(Node* gameWorld, Sprite* backgroundSprite, int gridStep, bool showLabels)
{
	if (!gameWorld || !backgroundSprite)
		return;

	auto gridContainer = Node::create();
	gridContainer->setName("GridDebugContainer");
	gameWorld->addChild(gridContainer, 1000);

	int pointCount = 0;

	//0-49
	for (int gridX = 0; gridX <= 49; gridX += gridStep)
	{
		for (int gridY = 0; gridY <= 49; gridY += gridStep)
		{
			Vec2 local_pos = ConvertTest::convertGridToLocal(gridX, gridY, backgroundSprite);

			Color3B color;
			float markerSize = 3.0f;

			// 特殊点标记
			if (gridX == 0 && gridY == 0)
			{
				color = Color3B::RED;
				markerSize = 6.0f;
			}
			else if (gridX == 49 && gridY == 49)  // 改为49
			{
				color = Color3B::BLUE;
				markerSize = 6.0f;
			}
			else if (gridX == 0 && gridY == 49)  // 改为49
			{
				color = Color3B::GREEN;
				markerSize = 6.0f;
			}
			else if (gridX == 49 && gridY == 0)  // 改为49
			{
				color = Color3B::YELLOW;
				markerSize = 6.0f;
			}
			else if (gridX == 24 && gridY == 24)  // 中心点改为24
			{
				color = Color3B::WHITE;
				markerSize = 5.0f;
			}
			else if (gridX == 0 || gridX == 49 || gridY == 0 || gridY == 49)  // 边界改为49
			{
				color = Color3B::ORANGE;
				markerSize = 4.0f;
			}
			else
			{
				color = Color3B(150, 150, 150);
				markerSize = 3.0f;
			}

			auto drawNode = DrawNode::create();
			drawNode->drawSolidCircle(Vec2::ZERO, markerSize, 0, 16, Color4F(color));
			drawNode->setPosition(local_pos);
			gridContainer->addChild(drawNode);

			// 标签显示逻辑
			bool shouldShowLabel = false;
			if (gridStep >= 4)
			{
				shouldShowLabel = showLabels;
			}
			else
			{
				shouldShowLabel = showLabels && (
					(gridX == 0 || gridX == 49 || gridX == 24) &&  // 改为49和24
					(gridY == 0 || gridY == 49 || gridY == 24)
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

	CCLOG("[DEBUG] Drew %d grid points", pointCount);

	// 绘制网格线
	auto gridLines = DrawNode::create();
	gridLines->setName("GridLines");

	for (int gridY = 0; gridY <= 49; gridY += (gridStep * 2))  // 改为49
	{
		Vec2 start = ConvertTest::convertGridToLocal(0, gridY, backgroundSprite);
		Vec2 end = ConvertTest::convertGridToLocal(49, gridY, backgroundSprite);  // 改为49
		gridLines->drawLine(start, end, Color4F(0.5f, 0.5f, 0.5f, 0.5f));
	}

	for (int gridX = 0; gridX <= 49; gridX += (gridStep * 2))  // 改为49
	{
		Vec2 start = ConvertTest::convertGridToLocal(gridX, 0, backgroundSprite);
		Vec2 end = ConvertTest::convertGridToLocal(gridX, 49, backgroundSprite);  // 改为49
		gridLines->drawLine(start, end, Color4F(0.5f, 0.5f, 0.5f, 0.5f));
	}

	gridContainer->addChild(gridLines, -1);

	// 绘制边界
	auto boundaryDraw = DrawNode::create();
	boundaryDraw->setName("Boundary");

	Vec2 corners[] = {
		ConvertTest::convertGridToLocal(0, 0, backgroundSprite),
		ConvertTest::convertGridToLocal(49, 0, backgroundSprite),    // 改为49
		ConvertTest::convertGridToLocal(49, 49, backgroundSprite),   // 改为49
		ConvertTest::convertGridToLocal(0, 49, backgroundSprite)     // 改为49
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
}
