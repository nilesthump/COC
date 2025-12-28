#include"UnitFactory.h"


std::string UnitFactory::makeSpritePath(const std::string& prefix, int level)
{
	const int MAX_LEVEL = 9;
	int clampLv = std::min(level, MAX_LEVEL);
	clampLv = std::max(clampLv, 1);   // 防止传 0
	return prefix + "Lv" + std::to_string(clampLv) + ".png";
}

BattleUnit* UnitFactory::CreateAttackerByType(UnitType type, int level, cocos2d::Node* parent, cocos2d::Sprite* background)
{
	switch (type)
	{
	case UnitType::BARBARIAN: return CreateBarbarian(level, parent, background);
	case UnitType::ARCHER:    return CreateArcher(level, parent, background);
	case UnitType::GIANT:     return CreateGiant(level, parent, background);
	case UnitType::GOBLIN:    return CreateGoblin(level, parent, background);
	case UnitType::BOMBER:    return CreateBomber(level, parent, background);
	case UnitType::BALLOON:   return CreateBalloon(level, parent, background);
	default: return nullptr;
	}
}

BattleUnit* UnitFactory::CreateBuildingByType(BuildingType type, int level, cocos2d::Node* parent, cocos2d::Sprite* background)
{
	switch (type)
	{
	case BuildingType::CANNON:				return CreateCannon(level, parent, background);
	case BuildingType::ARCHER_TOWER:		return CreateArcherTower(level, parent, background);
	case BuildingType::MORTAR:				return CreateMortar(level, parent, background);
	case BuildingType::WALL:				return CreateWall(level, parent, background);
	case BuildingType::GOLD_MINE:			return CreateGoldMine(level, parent, background);
	case BuildingType::GOLD_STORAGE:		return CreateGoldStorage(level, parent, background);
	case BuildingType::ELIXIR:				return CreateElixir(level, parent, background);
	case BuildingType::ELIXIR_STORAGE:      return CreateElixirStorage(level, parent, background);
	case BuildingType::TOWN_HALL:			return CreateTownHall(level, parent, background);
	case BuildingType::BUILDERSHUT:			return CreateBuildersHut(level, parent, background);
	case BuildingType::ARMYCAMP:			return CreateArmyCamp(level, parent, background);

	default: return nullptr;
	}
}

//建筑创建模板
BattleUnit* UnitFactory::CreateBaseBuilding(DefenderData data, const std::string& spritePath, Node* parent, Sprite* background, bool canAttack)
{
	auto unit = new BattleUnit();
	unit->Init(data);
	unit->SetBackgroundSprite(background);

	//表现层组装
	auto building = std::make_unique<BuildingComponent>(data.tile_width, data.tile_height);
	building->AttachTo(parent);
	auto sprite = Sprite::create(spritePath);
	if (sprite)
	{
		building->AttachSprite(sprite);
		building->FitSpriteToFootprint();
	}
	unit->SetBuildingComponent(std::move(building));

	//逻辑绑定
	if (canAttack)
	{
		unit->SetBehavior(std::make_unique<DefenderNormalBehavior>());
		unit->SetNavigation(std::make_unique<DefenderNavigation>());
	}
	else
	{
		unit->SetBehavior(std::make_unique<StaticBuildingBehavior>());
		unit->SetNavigation(nullptr);
	}

	unit->SetupHealthBar(parent);
	return unit;
}

//角色创建模板
BattleUnit* UnitFactory::CreateBaseAttacker(AttackerData data, const std::string& spritePath, Node* parent, Sprite* background)
{
	auto unit = new BattleUnit();
	unit->Init(data);
	unit->SetBackgroundSprite(background);
	unit->SetBehavior(std::make_unique<AttackerNormalBehavior>());
	unit->SetNavigation(std::make_unique<AttackerNavigation>());

	auto sprite = Sprite::create(spritePath);
	if (sprite)
	{
		sprite->setScale(0.5f);
		unit->SetSprite(sprite, parent);
	}

	unit->SetupHealthBar(parent);
	return unit;
}

BattleUnit* UnitFactory::CreateBarbarian(int level, Node* parent, Sprite* background)
{
	return CreateBaseAttacker(AttackerData::CreateBarbarianData(level), makeSpritePath("Barbarian", level), parent, background);
}

BattleUnit* UnitFactory::CreateArcher(int level, Node* parent, Sprite* background)
{
	return CreateBaseAttacker(AttackerData::CreateArcherData(level), makeSpritePath("Archer", level), parent, background);
}

BattleUnit* UnitFactory::CreateGiant(int level, Node* parent, Sprite* background)
{
	return CreateBaseAttacker(AttackerData::CreateGiantData(level), makeSpritePath("Giant", level), parent, background);
}

BattleUnit* UnitFactory::CreateGoblin(int level, Node* parent, Sprite* background)
{
	return CreateBaseAttacker(AttackerData::CreateGoblinData(level), makeSpritePath("Goblin", level), parent, background);
}

BattleUnit* UnitFactory::CreateBomber(int level, Node* parent, Sprite* background)
{
	auto unit = CreateBaseAttacker(AttackerData::CreateBomberData(level), makeSpritePath("Bomber", level) , parent, background);
	unit->SetBehavior(std::make_unique<BomberBehavior>());
	unit->SetNavigation(std::make_unique<BomberNavigation>());

	return unit;
}

//气球兵高度问题
BattleUnit* UnitFactory::CreateBalloon(int level, Node* parent, Sprite* background)
{
	auto unit = CreateBaseAttacker(AttackerData::CreateBalloonData(level), makeSpritePath("Balloon", level), parent, background);
	unit->SetBehavior(std::make_unique<BomberBehavior>()); 
	auto sprite = unit->GetSprite();
	if (sprite)
	{
		sprite->setUserData((void*)60);
		auto shadow = Sprite::create("shadow.png"); // 找一个黑色半透明椭圆图片
		if (shadow)
		{
			shadow->setOpacity(128); // 半透明
			shadow->setScale(0.8f);
			float centerX = sprite->getContentSize().width / 2;
			float centerY = sprite->getContentSize().height / 2;

			// 逻辑：父节点向上偏了 60，所以子节点相对于父中心向下偏 60
			// 这样阴影就会刚好落在 basePos（逻辑地面）
			shadow->setPosition(Vec2(centerX, centerY - 120));

			shadow->setName("Shadow");
			sprite->addChild(shadow, -1); // Z轴为-1，确保在气球下方
		
		}
	}
	return unit;
}

//资源类
BattleUnit* UnitFactory::CreateGoldMine(int level, Node* parent, Sprite* background)
{
	return CreateBaseBuilding(DefenderData::CreateGoldMineData(level), makeSpritePath("GoldMine", level) , parent, background, false);
}

BattleUnit* UnitFactory::CreateGoldStorage(int level, Node* parent, Sprite* background)
{
	return CreateBaseBuilding(DefenderData::CreateGoldStorageData(level),  makeSpritePath("GoldStorage", level ),parent, background, false);
}

BattleUnit* UnitFactory::CreateElixir(int level, Node* parent, Sprite* background)
{
	return CreateBaseBuilding(DefenderData::CreateElixirData(level), makeSpritePath("ElixirCollector", level), parent, background, false);
}

BattleUnit* UnitFactory::CreateElixirStorage(int level, Node* parent, Sprite* background)
{
	return CreateBaseBuilding(DefenderData::CreateElixirStorageData(level), makeSpritePath("ElixirStorage", level), parent, background, false);
}

//功能类
BattleUnit* UnitFactory::CreateTownHall(int level, Node* parent, Sprite* background)
{
	return CreateBaseBuilding(DefenderData::CreateTownHallData(level), makeSpritePath("TownHall", level) , parent, background, false);
}

BattleUnit* UnitFactory::CreateBuildersHut(int level, Node* parent, Sprite* background)
{
	return CreateBaseBuilding(DefenderData::CreateBuildersHutData(level), makeSpritePath("BuildersHut", level), parent, background, false);
}

BattleUnit* UnitFactory::CreateArmyCamp(int level, Node* parent, Sprite* background)
{
	return CreateBaseBuilding(DefenderData::CreateArmyCampData(level), makeSpritePath("ArmyCamp", level), parent, background, false);
}

BattleUnit* UnitFactory::CreateWall(int level, Node* parent, Sprite* background)
{
	auto unit = CreateBaseBuilding(DefenderData::CreateWallData(level), makeSpritePath("Walls", level), parent, background, false);
	return unit;
}

//防御攻击类
BattleUnit* UnitFactory::CreateCannon(int level, Node* parent, Sprite* background)
{
	return CreateBaseBuilding(DefenderData::CreateCannonData(level), makeSpritePath("Cannon", level) , parent, background, true);
}

BattleUnit* UnitFactory::CreateArcherTower(int level, Node* parent, Sprite* background)
{
	return CreateBaseBuilding(DefenderData::CreateArcherTowerData(level), makeSpritePath("ArcherTower", level) , parent, background, true);
}

BattleUnit* UnitFactory::CreateMortar(int level, Node* parent, Sprite* background)
{
	return CreateBaseBuilding(DefenderData::CreateMortarData(level), makeSpritePath("Mortar", level), parent, background, true);
}