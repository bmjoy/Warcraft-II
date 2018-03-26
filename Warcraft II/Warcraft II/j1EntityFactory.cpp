#include "Defs.h"
#include "p2Log.h"

#include "j1Module.h"
#include "j1App.h"

#include "j1EntityFactory.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Collision.h"
#include "j1Input.h"
#include "j1Player.h"

#include "Entity.h"
#include "DynamicEntity.h"
#include "StaticEntity.h"

j1EntityFactory::j1EntityFactory()
{
	name.assign("entities");
}

// Destructor
j1EntityFactory::~j1EntityFactory()
{
}

bool j1EntityFactory::Awake(pugi::xml_node& config) {

	bool ret = true;

	// Spritesheets
	pugi::xml_node spritesheets = config.child("spritesheets");
	humanBuildingsTexName = spritesheets.child("humanBuildings").attribute("name").as_string();
	neutralBuildingsTexName = spritesheets.child("neutralBuildings").attribute("name").as_string();
	orcishBuildingsTexName = spritesheets.child("orcishBuildings").attribute("name").as_string();
	footmanTexName = spritesheets.child("footmanAnimations").attribute("name").as_string();
	gruntTexName = spritesheets.child("gruntAnimations").attribute("name").as_string();

	//Debug Textures Properties
	buildingPreviewTiles.opacity = config.child("previewTexturesProperties").attribute("tileBuildingPlaceOpacity").as_uint();
	previewBuildingOpacity = config.child("previewTexturesProperties").attribute("buildingPlaceOpacity").as_uint();

	// Static entities
	pugi::xml_node staticEntities = config.child("staticEntities");

	//Human buildings
	pugi::xml_node humanBuildings = staticEntities.child("humanBuildings");

	townHallInfo.townHallMaxLife = { humanBuildings.child("townHall").child("maxLife").attribute("value").as_int() };
	pugi::xml_node aux = humanBuildings.child("townHall").child("sprites");
	townHallInfo.townHallCompleteTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	townHallInfo.townHallInProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	townHallInfo.keepMaxLife = { humanBuildings.child("keep").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("keep").child("sprites");
	townHallInfo.keepCompleteTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	townHallInfo.keepInProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	townHallInfo.castleMaxLife = { humanBuildings.child("castle").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("castle").child("sprites");
	townHallInfo.castleCompleteTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	townHallInfo.castleInProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	chickenFarmInfo.maxLife = { humanBuildings.child("chickenFarm").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("chickenFarm").child("sprites");
	chickenFarmInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	chickenFarmInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	scoutTowerInfo.maxLife = { humanBuildings.child("scoutTower").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("scoutTower").child("sprites");
	scoutTowerInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	scoutTowerInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	playerGuardTowerInfo.maxLife = { humanBuildings.child("guardTower").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("guardTower").child("sprites");
	playerGuardTowerInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	playerGuardTowerInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	playerCannonTowerInfo.maxLife = { humanBuildings.child("canonTower").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("canonTower").child("sprites");
	playerCannonTowerInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	playerCannonTowerInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	barracksInfo.barracks1MaxLife = { humanBuildings.child("barracks").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("barracks").child("sprites");
	barracksInfo.barracksCompleteTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };

	barracksInfo.barracks2MaxLife = { humanBuildings.child("barracks2").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("barracks2").child("sprites");
	barracksInfo.barracks2CompleteTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };

	elvenLumberMillInfo.maxLife= { humanBuildings.child("elvenLumberMill").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("elvenLumberMill").child("sprites");
	elvenLumberMillInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	elvenLumberMillInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	mageTowerInfo.maxLife = { humanBuildings.child("mageTower").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("mageTower").child("sprites");
	mageTowerInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	mageTowerInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	gryphonAviaryInfo.maxLife = { humanBuildings.child("gryphonAviary").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("gryphonAviary").child("sprites");
	gryphonAviaryInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	gryphonAviaryInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	stablesInfo.maxLife = { humanBuildings.child("stables").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("stables").child("sprites");
	stablesInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	stablesInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	//Construction planks for the human buildings

	pugi::xml_node smallConstructionPlanks = humanBuildings.child("constructionPlanks").child("smallTilePlanks");
	aux = smallConstructionPlanks.child("first");
	chickenFarmInfo.constructionPlanks1 = scoutTowerInfo.constructionPlanks1 = playerGuardTowerInfo.constructionPlanks1 = playerCannonTowerInfo.constructionPlanks1 = { aux.attribute("x").as_int(), aux.attribute("y").as_int(), aux.attribute("w").as_int(), aux.attribute("h").as_int() };
	aux = smallConstructionPlanks.child("second");
	chickenFarmInfo.constructionPlanks2 = scoutTowerInfo.constructionPlanks2 = playerGuardTowerInfo.constructionPlanks2 = playerCannonTowerInfo.constructionPlanks2 = { aux.attribute("x").as_int(), aux.attribute("y").as_int(), aux.attribute("w").as_int(), aux.attribute("h").as_int() };

	pugi::xml_node mediumConstructionPlanks = humanBuildings.child("constructionPlanks").child("mediumTilePlanks");
	aux = mediumConstructionPlanks.child("first");
	barracksInfo.constructionPlanks1 = elvenLumberMillInfo.constructionPlanks1 = { aux.attribute("x").as_int(), aux.attribute("y").as_int(), aux.attribute("w").as_int(), aux.attribute("h").as_int() };
	mageTowerInfo.constructionPlanks1 = gryphonAviaryInfo.constructionPlanks1 = stablesInfo.constructionPlanks1 = { aux.attribute("x").as_int(), aux.attribute("y").as_int(), aux.attribute("w").as_int(), aux.attribute("h").as_int() };
	aux = mediumConstructionPlanks.child("second");
	barracksInfo.constructionPlanks2 = elvenLumberMillInfo.constructionPlanks2 = { aux.attribute("x").as_int(), aux.attribute("y").as_int(), aux.attribute("w").as_int(), aux.attribute("h").as_int() };
	mageTowerInfo.constructionPlanks2 = gryphonAviaryInfo.constructionPlanks2 = stablesInfo.constructionPlanks2 = { aux.attribute("x").as_int(), aux.attribute("y").as_int(), aux.attribute("w").as_int(), aux.attribute("h").as_int() };


	//Building preview tiles
	pugi::xml_node previewTiles = humanBuildings.child("previewTiles");
	buildingPreviewTiles.greenTile = { previewTiles.child("green").attribute("x").as_int(), previewTiles.child("green").attribute("y").as_int(), previewTiles.child("green").attribute("w").as_int(), previewTiles.child("green").attribute("h").as_int() };
	buildingPreviewTiles.redTile = { previewTiles.child("red").attribute("x").as_int(), previewTiles.child("red").attribute("y").as_int(), previewTiles.child("red").attribute("w").as_int(), previewTiles.child("red").attribute("h").as_int() };

	//Neutral buildings
	pugi::xml_node neutralBuildings = staticEntities.child("neutralBuildings");

	goldMineInfo.maxLife = { neutralBuildings.child("goldMine").child("maxLife").attribute("value").as_int() };
	aux = neutralBuildings.child("goldMine").child("sprites");
	goldMineInfo.completeTexArea = { aux.child("active").attribute("x").as_int(), aux.child("active").attribute("y").as_int(), aux.child("active").attribute("w").as_int(), aux.child("active").attribute("h").as_int() };
	goldMineInfo.inProgressTexArea = { aux.child("inactive").attribute("x").as_int(), aux.child("inactive").attribute("y").as_int(), aux.child("inactive").attribute("w").as_int(), aux.child("inactive").attribute("h").as_int() };

	aux = neutralBuildings.child("runeStone").child("sprites");
	runestoneInfo.completeTexArea = { aux.attribute("x").as_int(), aux.attribute("y").as_int(), aux.attribute("w").as_int(), aux.attribute("h").as_int() };

	//Dynamic entities
	//Humans
	pugi::xml_node humanEntities = config.child("dynamicEntities").child("humans");

	//Footman animations
	pugi::xml_node footmanAnimations = humanEntities.child("footman").child("animations");
	pugi::xml_node currentAnimation;

	//Up animation Footman
	currentAnimation = footmanAnimations.child("up");
	footmanInfo.up.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.up.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.up.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Down animation Footman
	currentAnimation = footmanAnimations.child("down");
	footmanInfo.down.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.down.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.down.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Right animation Footman
	currentAnimation = footmanAnimations.child("right");
	footmanInfo.right.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.right.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.right.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Left animation Footman
	currentAnimation = footmanAnimations.child("left");
	footmanInfo.left.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.left.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.left.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Up Right animation Footman
	currentAnimation = footmanAnimations.child("upRight");
	footmanInfo.upRight.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.upRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.upRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Up Left animation Footman
	currentAnimation = footmanAnimations.child("upLeft");
	footmanInfo.upLeft.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.upLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.upLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Down Right animation Footman
	currentAnimation = footmanAnimations.child("downRight");
	footmanInfo.downRight.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.downRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.downRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Down Left animation Footman
	currentAnimation = footmanAnimations.child("downLeft");
	footmanInfo.downLeft.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.downLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.downLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Up animation Footman
	currentAnimation = footmanAnimations.child("attackUp");
	footmanInfo.attackUp.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Down animation Footman
	currentAnimation = footmanAnimations.child("attackDown");
	footmanInfo.attackDown.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Left animation Footman
	currentAnimation = footmanAnimations.child("attackLeft");
	footmanInfo.attackLeft.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Right animation Footman
	currentAnimation = footmanAnimations.child("attackRight");
	footmanInfo.attackRight.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Up Left animation Footman
	currentAnimation = footmanAnimations.child("attackUpLeft");
	footmanInfo.attackUpLeft.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackUpLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackUpLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Up Right animation Footman
	currentAnimation = footmanAnimations.child("attackUpRight");
	footmanInfo.attackUpRight.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackUpRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackUpRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Down Left animation Footman
	currentAnimation = footmanAnimations.child("attackDownLeft");
	footmanInfo.attackDownLeft.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackDownLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackDownLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Down Right animation Footman
	currentAnimation = footmanAnimations.child("attackDownRight");
	footmanInfo.attackDownRight.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackDownRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackDownRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Death Up animation Footman
	currentAnimation = footmanAnimations.child("deathUp");
	footmanInfo.deathUp.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.deathUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.deathUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Death Down animation Footman
	currentAnimation = footmanAnimations.child("deathDown");
	footmanInfo.deathDown.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.deathDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.deathDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	//Orcs
	pugi::xml_node orcEntities = config.child("dynamicEntities").child("orcs");

	//Grunt animations
	pugi::xml_node gruntAnimations = orcEntities.child("grunt").child("animations");
	
	//Up animation Grunt
	currentAnimation = gruntAnimations.child("up");
	gruntInfo.up.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.up.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.up.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Down animation Grunt
	currentAnimation = gruntAnimations.child("down");
	gruntInfo.down.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.down.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.down.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Left animation Grunt
	currentAnimation = gruntAnimations.child("left");
	gruntInfo.left.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.left.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.left.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Right animation Grunt
	currentAnimation = gruntAnimations.child("right");
	gruntInfo.right.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.right.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.right.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Up Left animation Grunt
	currentAnimation = gruntAnimations.child("upLeft");
	gruntInfo.upLeft.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.upLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.upLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Up Right animation Grunt
	currentAnimation = gruntAnimations.child("upRight");
	gruntInfo.upRight.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.upRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.upRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Down Left animation Grunt
	currentAnimation = gruntAnimations.child("downLeft");
	gruntInfo.downLeft.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.downLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.downLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Down Right animation Grunt
	currentAnimation = gruntAnimations.child("downRight");
	gruntInfo.downRight.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.downRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.downRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Up animation Grunt
	currentAnimation = gruntAnimations.child("attackUp");
	gruntInfo.attackUp.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Down animation Grunt
	currentAnimation = gruntAnimations.child("attackDown");
	gruntInfo.attackDown.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Left animation Grunt
	currentAnimation = gruntAnimations.child("attackLeft");
	gruntInfo.attackLeft.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Right animation Grunt
	currentAnimation = gruntAnimations.child("attackRight");
	gruntInfo.attackRight.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Up Left animation Grunt
	currentAnimation = gruntAnimations.child("attackUpLeft");
	gruntInfo.attackUpLeft.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackUpLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackUpLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Up Right animation Grunt
	currentAnimation = gruntAnimations.child("attackUpRight");
	gruntInfo.attackUpRight.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackUpRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackUpRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Down Left animation Grunt
	currentAnimation = gruntAnimations.child("attackDownLeft");
	gruntInfo.attackDownLeft.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackDownLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackDownLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Down Right animation Grunt
	currentAnimation = gruntAnimations.child("attackDownRight");
	gruntInfo.attackDownRight.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackDownRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackDownRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Death Up animation Grunt
	currentAnimation = gruntAnimations.child("deathUp");
	gruntInfo.deathUp.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.deathUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.deathUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Death Down animation Grunt
	currentAnimation = gruntAnimations.child("deathDown");
	gruntInfo.deathDown.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.deathDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.deathDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	

	return ret;
}

bool j1EntityFactory::Start()
{
	bool ret = true;

	LOG("Loading entities textures");

	humanBuildingsTex = App->tex->Load(humanBuildingsTexName.data());
	neutralBuildingsTex = App->tex->Load(neutralBuildingsTexName.data());
	orcishBuildingsTex = App->tex->Load(orcishBuildingsTexName.data());
	footmanTex = App->tex->Load(footmanTexName.data());
	gruntTex = App->tex->Load(gruntTexName.data());

	return ret;
}

bool j1EntityFactory::PreUpdate()
{
	bool ret = true;

	// Spawn entities
	list<Entity*>::const_iterator it = toSpawnEntities.begin();

	while (it != toSpawnEntities.end()) {

		fPoint pos = (*it)->GetPosition();
		int x = pos.x * App->scene->scale;
		int y = pos.y * App->scene->scale;
		App->map->WorldToMap(x, y);

		// Move the entity from the waiting list to the active list
		if ((*it)->entityType == EntityCategory_DYNAMIC_ENTITY) {

			activeDynamicEntities.push_back((DynamicEntity*)(*it));
			LOG("Spawning dynamic entity at tile %d,%d", x, y);
		}
		else if ((*it)->entityType == EntityCategory_STATIC_ENTITY) {

			activeStaticEntities.push_back((StaticEntity*)(*it));
			LOG("Spawning static entity at tile %d,%d", x, y);
		}

		it++;
	}
	toSpawnEntities.clear();

	return ret;
}

// Called before render is available
bool j1EntityFactory::Update(float dt)
{
	bool ret = true;
	// Update active dynamic entities
	list<StaticEntity*>::const_iterator it = activeStaticEntities.begin();

	while (it != activeStaticEntities.end()) {
		(*it)->Move(dt);
		it++;
	}

	return ret;
}

void j1EntityFactory::Draw()
{
	// Blit active entities

	// Dynamic entities (one texture per dynamic entity)
	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {

		switch ((*dynEnt)->dynamicEntityType) {

			// Player
		case EntityType_FOOTMAN:
			(*dynEnt)->Draw(footmanTex);
			break;
		case EntityType_ELVEN_ARCHER:
			(*dynEnt)->Draw(elvenArcherTex);
			break;
		case EntityType_GRYPHON_RIDER:
			(*dynEnt)->Draw(gryphonRiderTex);
			break;
		case EntityType_MAGE:
			(*dynEnt)->Draw(mageTex);
			break;
		case EntityType_PALADIN:
			(*dynEnt)->Draw(paladinTex);
			break;

		case EntityType_TURALYON:
			(*dynEnt)->Draw(turalyonTex);
			break;
		case EntityType_KHADGAR:
			(*dynEnt)->Draw(khadgarTex);
			break;
		case EntityType_ALLERIA:
			(*dynEnt)->Draw(alleriaTex);
			break;

			// Enemy
		case EntityType_GRUNT:
			(*dynEnt)->Draw(gruntTex);
			break;
		case EntityType_TROLL_AXETHROWER:
			(*dynEnt)->Draw(trollAxethrowerTex);
			break;
		case EntityType_DRAGON:
			(*dynEnt)->Draw(dragonTex);
			break;

		default:
			break;
		}

		dynEnt++;
	}

	// Static entities (altas textures for every type of static entity)
	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {

		switch ((*statEnt)->staticEntityCategory) {

		case StaticEntityCategory_HumanBuilding:
			(*statEnt)->Draw(humanBuildingsTex);
			break;
		case StaticEntityCategory_NeutralBuilding:
			(*statEnt)->Draw(neutralBuildingsTex);
			break;
		case StaticEntityCategory_OrcishBuilding:
			(*statEnt)->Draw(orcishBuildingsTex);
			break;

		default:
			break;
		}

		statEnt++;
	}

	DrawStaticEntityPreview(App->scene->GetAlphaBuilding(), App->player->GetMousePos());
}

void j1EntityFactory::DrawStaticEntityPreview(ENTITY_TYPE staticEntityType, iPoint mousePos)
{
	switch (staticEntityType) {

	case EntityType_CHICKEN_FARM:
		SDL_SetTextureAlphaMod(humanBuildingsTex, 100);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &chickenFarmInfo.completeTexArea);
		break;
	case EntityType_ELVEN_LUMBER_MILL:
		SDL_SetTextureAlphaMod(humanBuildingsTex, 100);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &elvenLumberMillInfo.completeTexArea);
		break;
	case EntityType_MAGE_TOWER:
		SDL_SetTextureAlphaMod(humanBuildingsTex, 100);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &mageTowerInfo.completeTexArea);
		break;
	case EntityType_GRYPHON_AVIARY:
		SDL_SetTextureAlphaMod(humanBuildingsTex, 100);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &gryphonAviaryInfo.completeTexArea);
		break;
	case EntityType_STABLES:
		SDL_SetTextureAlphaMod(humanBuildingsTex, 100);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &stablesInfo.completeTexArea);
		break;
	case EntityType_SCOUT_TOWER:
		SDL_SetTextureAlphaMod(humanBuildingsTex, 100);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &scoutTowerInfo.completeTexArea);
		break;
	case EntityType_PLAYER_GUARD_TOWER:
		SDL_SetTextureAlphaMod(humanBuildingsTex, 100);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &playerGuardTowerInfo.completeTexArea);
		break;
	case EntityType_PLAYER_CANNON_TOWER:
		SDL_SetTextureAlphaMod(humanBuildingsTex, 100);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &playerCannonTowerInfo.completeTexArea);
		break;
	case EntityCategory_NONE:
		break;
	default:
		break;
	}
	
	HandleStaticEntityPreviewTiles(staticEntityType, mousePos);

}

//Handles when to return green or red tiles 
void j1EntityFactory::HandleStaticEntityPreviewTiles(ENTITY_TYPE staticEntityType, iPoint mousePos)
{
	switch (staticEntityType) {

	case EntityType_CHICKEN_FARM:
	case EntityType_SCOUT_TOWER:
		DrawStaticEntityPreviewTiles(true, Small, mousePos);

		if (isPreviewBuildingOnEntity(App->player->GetMouseTilePos(), Small)) 
			DrawStaticEntityPreviewTiles(false, Small, mousePos);

		break;
	case EntityType_ELVEN_LUMBER_MILL:
	case EntityType_MAGE_TOWER:
	case EntityType_GRYPHON_AVIARY:
	case EntityType_STABLES:
		DrawStaticEntityPreviewTiles(true, Medium, mousePos);

		if (isPreviewBuildingOnEntity(App->player->GetMouseTilePos(), Medium)) 
			DrawStaticEntityPreviewTiles(false, Medium, mousePos);

		break;

	default:
		break;
	}
}

//Draws green or red tiles on the preview depending on if there's a building on the ground and the preview building size
void j1EntityFactory::DrawStaticEntityPreviewTiles(bool isPlaceable, StaticEntitySize buildingSize, iPoint mousePos)
{
	SDL_SetTextureAlphaMod(neutralBuildingsTex, buildingPreviewTiles.opacity);
	iPoint mouseTilePos = App->player->GetMouseTilePos();

	switch (buildingSize) {
	case Small:
		if (isPlaceable) {
			App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y + 32, &buildingPreviewTiles.greenTile);
		}
		else if (!isPlaceable) {
			if(isEntityOnTile(mouseTilePos)) //0,0
				App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 1, mouseTilePos.y})) //32,0
				App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x, mouseTilePos.y + 1 })) //0,32
				App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y + 32, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 1, mouseTilePos.y + 1 })) //32,32
				App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y + 32, &buildingPreviewTiles.redTile);
		}
		break;
	case Medium:
		if (isPlaceable) {
			App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y + 64, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 64, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 64, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y + 64, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 64, mousePos.y + 64, &buildingPreviewTiles.greenTile);
		}
		else if (!isPlaceable) {
			if (isEntityOnTile(mouseTilePos)) //0,0
				App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 1, mouseTilePos.y })) //32,0
				App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x, mouseTilePos.y + 1 })) //0,32
				App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y + 32, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 1, mouseTilePos.y + 1 })) //32,32
				App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y + 32, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 1, mouseTilePos.y + 2 })) //32,64
				App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y + 64, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 2, mouseTilePos.y + 1 })) //64,32
				App->render->Blit(neutralBuildingsTex, mousePos.x + 64, mousePos.y + 32, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 2, mouseTilePos.y})) //64,0
				App->render->Blit(neutralBuildingsTex, mousePos.x + 64, mousePos.y, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x , mouseTilePos.y + 2 })) //0,64
				App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y + 64, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 2, mouseTilePos.y + 2 })) //64,64
				App->render->Blit(neutralBuildingsTex, mousePos.x + 64, mousePos.y + 64, &buildingPreviewTiles.redTile);
		}
		break;
	case Big:
		if (isPlaceable) {
			App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y + 64, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y + 64, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y + 64, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y + 96, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y + 96, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y + 64, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y + 96, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y + 96, &buildingPreviewTiles.greenTile);
		}
		else if (!isPlaceable) {
			if (isEntityOnTile(mouseTilePos)) //0,0
				App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 1, mouseTilePos.y })) //32,0
				App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x, mouseTilePos.y + 1 })) //0,32
				App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y + 32, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 1, mouseTilePos.y + 1 })) //32,32
				App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y + 32, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 1, mouseTilePos.y + 2 })) //32,64
				App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y + 64, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 2, mouseTilePos.y + 1 })) //64,32
				App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y + 32, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 2, mouseTilePos.y })) //64,0
				App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x, mouseTilePos.y + 2 })) //0,64
				App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y + 64, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 2, mouseTilePos.y + 2})) //64,64
				App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y + 64, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 1, mouseTilePos.y + 3 })) //32,96
				App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y + 96, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 2, mouseTilePos.y + 3 })) //64,96
				App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y + 96, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 3, mouseTilePos.y + 1 })) //96,32
				App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y + 32, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 3, mouseTilePos.y + 2 })) //96,32
				App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y + 64, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 3, mouseTilePos.y })) //96,0
				App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x, mouseTilePos.y + 3 })) //0,96
				App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y + 96, &buildingPreviewTiles.redTile);
			if (isEntityOnTile({ mouseTilePos.x + 3, mouseTilePos.y + 3 })) //96,96
				App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y + 96, &buildingPreviewTiles.redTile);
		}
		break;
	case None:
		break;
	default:
		break;
	}

}


const EntityInfo& j1EntityFactory::GetBuildingInfo(ENTITY_TYPE staticEntityType)
{
	switch (staticEntityType) {
	case EntityType_CHICKEN_FARM:
		return (const EntityInfo&)chickenFarmInfo;
		break;
	case EntityType_BARRACKS:
		return (const EntityInfo&)barracksInfo;
		break;
	case EntityType_ELVEN_LUMBER_MILL:
		return (const EntityInfo&)elvenLumberMillInfo;
		break;
	case EntityType_MAGE_TOWER:
		return(const EntityInfo&)mageTowerInfo;
		break;
	case EntityType_GRYPHON_AVIARY:
		return(const EntityInfo&)gryphonAviaryInfo;
		break;
	case EntityType_STABLES:
		return(const EntityInfo&)stablesInfo;
		break;
	case EntityType_SCOUT_TOWER:
		return(const EntityInfo&)scoutTowerInfo;
		break;
	case EntityType_PLAYER_GUARD_TOWER:
		return(const EntityInfo&)playerGuardTowerInfo;
		break;
	case EntityType_PLAYER_CANNON_TOWER:
		return(const EntityInfo&)playerCannonTowerInfo;
		break;
	default:
		return (const EntityInfo&)chickenFarmInfo;
		break;
	}

	//return ret;
}

SDL_Texture* j1EntityFactory::GetHumanBuildingTexture() {

	return humanBuildingsTex;
}

SDL_Texture* j1EntityFactory::GetNeutralBuildingTexture() {

	return neutralBuildingsTex;
}

//It returns true if there's an entity in the tile
bool j1EntityFactory::isEntityOnTile(iPoint tile) const 
{
	//Dynamic entities
	list<DynamicEntity*>::const_iterator activeDyn = activeDynamicEntities.begin();

	while (activeDyn != activeDynamicEntities.end()) {

		iPoint entityTile = App->map->WorldToMap((*activeDyn)->GetPosition().x, (*activeDyn)->GetPosition().y);
		if (tile.x == entityTile.x && tile.y == entityTile.y)
			return true;

		activeDyn++;
	}

	//Static entities
	list<StaticEntity*>::const_iterator activeStatic = activeStaticEntities.begin();

	while (activeStatic != activeStaticEntities.end()) {

		iPoint entityTile = App->map->WorldToMap((*activeStatic)->GetPosition().x, (*activeStatic)->GetPosition().y);

		if ((*activeStatic)->GetSize().x == 64 && (*activeStatic)->GetSize().y == 64) {
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++) {
					if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
						return true;
				}
			}
		}
		else if ((*activeStatic)->GetSize().x == 96 && (*activeStatic)->GetSize().y == 96) {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
						return true;
				}
			}

		}
		else if ((*activeStatic)->GetSize().x == 128 && (*activeStatic)->GetSize().y == 128) {

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
						return true;
				}
			}
		}

		activeStatic++;
	}

	// We do also need to check the toSpawn list (just in case)
	list<Entity*>::const_iterator toSpawn = toSpawnEntities.begin();
	while (toSpawn != toSpawnEntities.end()) {

		iPoint entityTile = App->map->WorldToMap((*toSpawn)->GetPosition().x, (*toSpawn)->GetPosition().y);

		if (tile.x == entityTile.x && tile.y == entityTile.y)
			return true;

		toSpawn++;
	}

	return false;
}

// Returns true if a building can NOT be built in that spot
bool j1EntityFactory::isPreviewBuildingOnEntity(iPoint tile, StaticEntitySize buildingSize) const
{
	//Dynamic entities
	list<DynamicEntity*>::const_iterator activeDyn = activeDynamicEntities.begin();

	while (activeDyn != activeDynamicEntities.end()) {
	
		iPoint entityTile = App->map->WorldToMap((*activeDyn)->GetPosition().x, (*activeDyn)->GetPosition().y);

		//This checks the tile of the dynamic entity and its surroundings
		switch (buildingSize)
		{
		case Small:
			for (int i = -1; i < 1; i++) {
				for (int j = -1; j < 1; j++) {
					if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
						return true;
				}
			}
			break;
		case Medium:
			for (int i = -2; i < 1; i++) {
				for (int j = -2; j < 1; j++) {
					if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
						return true;
				}
			}
			break;
		case Big:
			for (int i = -3; i < 1; i++) {
				for (int j = -3; j < 1; j++) {
					if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
						return true;
				}
			}
			break;
		default:
			break;
		}

		activeDyn++;
	}

	//Static entities
	list<StaticEntity*>::const_iterator activeStatic = activeStaticEntities.begin();

	while (activeStatic != activeStaticEntities.end()) {

			iPoint entityTile = App->map->WorldToMap((*activeStatic)->GetPosition().x, (*activeStatic)->GetPosition().y);
			
			//This checks all of the tiles that conform of the static entity and their surrounding tiles
			if ((*activeStatic)->GetSize().x == 64 && (*activeStatic)->GetSize().y == 64) {
				switch (buildingSize)
				{
				case Small:
					for (int i = -1; i < 2; i++) {
						for (int j = -1; j < 2; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
							}
						}
					break;
				case Medium:
					for (int i = -2; i < 2; i++) {
						for (int j = -2; j < 2; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				case Big:
					for (int i = -3; i < 2; i++) {
						for (int j = -3; j < 2; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				default:
					break;
				}
			}
			else if ((*activeStatic)->GetSize().x == 96 && (*activeStatic)->GetSize().y == 96) {
				switch (buildingSize)
				{
				case Small:
					for (int i = -1; i < 3; i++) {
						for (int j = -1; j < 3; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				case Medium:
					for (int i = -2; i < 3; i++) {
						for (int j = -2; j < 3; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				case Big:
					for (int i = -3; i < 3; i++) {
						for (int j = -3; j < 3; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				default:
					break;
				}
			}
			else if ((*activeStatic)->GetSize().x == 128 && (*activeStatic)->GetSize().y == 128) {
				switch (buildingSize)
				{
				case Small:
					for (int i = -1; i < 4; i++) {
						for (int j = -1; j < 4; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				case Medium:
					for (int i = -2; i < 4; i++) {
						for (int j = -2; j < 4; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				case Big:
					for (int i = -3; i < 4; i++) {
						for (int j = -3; j < 4; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				default:
					break;
				}
			}

			activeStatic++;
	}

	return false;
}

bool j1EntityFactory::PostUpdate()
{
	bool ret = true;

	// Remove active entities
	/// Remove dynamic entities
	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {
		if ((*dynEnt)->remove) {
			delete *dynEnt;
			activeDynamicEntities.remove(*dynEnt);
		}

		dynEnt++;
	}

	/// Remove static entities
	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {
		if ((*statEnt)->remove) {
			delete *statEnt;
			activeStaticEntities.remove(*statEnt);
		}

		statEnt++;
	}

	return ret;
}

// Called before quitting
bool j1EntityFactory::CleanUp()
{
	bool ret = true;

	LOG("Freeing all entities");

	// Remove active entities
	/// Remove dynamic entities
	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {
		if ((*dynEnt)->remove) {
			delete *dynEnt;
			activeDynamicEntities.remove(*dynEnt);
		}

		dynEnt++;
	}
	activeDynamicEntities.clear();

	/// Remove static entities
	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {
		if ((*statEnt)->remove) {
			delete *statEnt;
			activeStaticEntities.remove(*statEnt);
		}

		statEnt++;
	}
	activeStaticEntities.clear();

	// Remove to spawn entities
	list<Entity*>::const_iterator it = toSpawnEntities.begin();

	while (it != toSpawnEntities.end()) {
		delete *it;
		it++;
	}
	toSpawnEntities.clear();

	// Free all textures
	App->tex->UnLoad(footmanTex);

	return ret;
}


Entity* j1EntityFactory::AddEntity(ENTITY_TYPE entityType, fPoint pos, const EntityInfo& entityInfo, j1Module* listener)
{
	switch (entityType) {

	case EntityType_TOWN_HALL:
	{
		TownHall* townHall = new TownHall(pos, { 128,128 }, townHallInfo.townHallMaxLife, (const TownHallInfo&)entityInfo, listener);
		townHall->entityType = EntityCategory_STATIC_ENTITY;
		townHall->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		townHall->staticEntityType = EntityType_TOWN_HALL;

		toSpawnEntities.push_back((Entity*)townHall);
		return (StaticEntity*)townHall;
	}
	break;

	case EntityType_CHICKEN_FARM:
	{
		ChickenFarm* chickenFarm = new ChickenFarm(pos, { 64,64 }, chickenFarmInfo.maxLife, (const ChickenFarmInfo&)entityInfo, listener);
		chickenFarm->entityType = EntityCategory_STATIC_ENTITY;
		chickenFarm->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		chickenFarm->staticEntityType = EntityType_CHICKEN_FARM;


		toSpawnEntities.push_back((Entity*)chickenFarm);
		return (StaticEntity*)chickenFarm;
	}
	break;

	case EntityType_BARRACKS:
	{

		Barracks* barracks = new Barracks(pos, { 96,96 }, barracksInfo.barracks1MaxLife, (const BarracksInfo&)entityInfo, listener);
		barracks->entityType = EntityCategory_STATIC_ENTITY;

		barracks->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		barracks->staticEntityType = EntityType_BARRACKS;

		toSpawnEntities.push_back((Entity*)barracks);
		return (StaticEntity*)barracks;
	}
	break;

	case EntityType_ELVEN_LUMBER_MILL:
	{

		ElvenLumberMill* elvenLumberMill = new ElvenLumberMill(pos, { 96,96 }, elvenLumberMillInfo.maxLife, (const ElvenLumberMillInfo&)entityInfo, listener);
		elvenLumberMill->entityType = EntityCategory_STATIC_ENTITY;

		elvenLumberMill->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		elvenLumberMill->staticEntityType = EntityType_BARRACKS;

		toSpawnEntities.push_back((Entity*)elvenLumberMill);
		return (StaticEntity*)elvenLumberMill;
	}
	break;

	case EntityType_MAGE_TOWER:
	{

		MageTower* mageTower = new MageTower(pos, { 96,96 }, mageTowerInfo.maxLife, (const MageTowerInfo&)entityInfo, listener);
		mageTower->entityType = EntityCategory_STATIC_ENTITY;

		mageTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		mageTower->staticEntityType = EntityType_MAGE_TOWER;

		toSpawnEntities.push_back((Entity*)mageTower);
		return (StaticEntity*)mageTower;
	}
	break;

	case EntityType_GRYPHON_AVIARY:
	{

		GryphonAviary* gryphonAviary = new GryphonAviary(pos, { 96,96 }, gryphonAviaryInfo.maxLife, (const GryphonAviaryInfo&)entityInfo, listener);
		gryphonAviary->entityType = EntityCategory_STATIC_ENTITY;

		gryphonAviary->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		gryphonAviary->staticEntityType = EntityType_GRYPHON_AVIARY;

		toSpawnEntities.push_back((Entity*)gryphonAviary);
		return (StaticEntity*)gryphonAviary;
	}
	break;

	case EntityType_STABLES:
	{

		Stables* stables = new Stables(pos, { 96,96 }, stablesInfo.maxLife, (const StablesInfo&)entityInfo, listener);
		stables->entityType = EntityCategory_STATIC_ENTITY;

		stables->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		stables->staticEntityType = EntityType_STABLES;

		toSpawnEntities.push_back((Entity*)stables);
		return (StaticEntity*)stables;
	}
	break;

	case EntityType_SCOUT_TOWER:
	{
		ScoutTower* scoutTower = new ScoutTower(pos, { 64,64 }, scoutTowerInfo.maxLife, (const ScoutTowerInfo&)entityInfo, listener);
		scoutTower->entityType = EntityCategory_STATIC_ENTITY;
		scoutTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		scoutTower->staticEntityType = EntityType_SCOUT_TOWER;

		toSpawnEntities.push_back((Entity*)scoutTower);
		return (StaticEntity*)scoutTower;
	}
	break;

	case EntityType_PLAYER_GUARD_TOWER:
	{
		PlayerGuardTower* playerGuardTower = new PlayerGuardTower(pos, { 64,64 }, 12, (const PlayerGuardTowerInfo&)entityInfo, listener);
		playerGuardTower->entityType = EntityCategory_STATIC_ENTITY;
		playerGuardTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		playerGuardTower->staticEntityType = EntityType_PLAYER_GUARD_TOWER;

		toSpawnEntities.push_back((Entity*)playerGuardTower);
		return (StaticEntity*)playerGuardTower;
	}
	break;

	case EntityType_PLAYER_CANNON_TOWER:
	{
		PlayerCannonTower* playerCannonTower = new PlayerCannonTower(pos, { 64,64 }, 12, (const PlayerCannonTowerInfo&)entityInfo, listener);
		playerCannonTower->entityType = EntityCategory_STATIC_ENTITY;
		playerCannonTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		playerCannonTower->staticEntityType = EntityType_PLAYER_CANNON_TOWER;

		toSpawnEntities.push_back((Entity*)playerCannonTower);
		return (StaticEntity*)playerCannonTower;
	}
	break;

	case EntityType_GOLD_MINE:
	{
		GoldMine* goldMine = new GoldMine(pos, { 96,96 }, goldMineInfo.maxLife, (const GoldMineInfo&)entityInfo, listener);
		goldMine->entityType = EntityCategory_STATIC_ENTITY;
		goldMine->staticEntityCategory = StaticEntityCategory_NeutralBuilding;
		goldMine->staticEntityType = EntityType_GOLD_MINE;

		toSpawnEntities.push_back((Entity*)goldMine);
		return (StaticEntity*)goldMine;
	}
	break;

	case EntityType_RUNESTONE:
	{
		Runestone* runestone = new Runestone(pos, { 64,64 }, 0, (const RunestoneInfo&)entityInfo, listener);
		runestone->entityType = EntityCategory_STATIC_ENTITY;
		runestone->staticEntityCategory = StaticEntityCategory_NeutralBuilding;
		runestone->staticEntityType = EntityType_RUNESTONE;

		toSpawnEntities.push_back((Entity*)runestone);
		return (StaticEntity*)runestone;
	}
	break;

	case EntityType_WATCH_TOWER:
	{
		WatchTower* watchTower = new WatchTower(pos, { 64,64 }, 12, (const WatchTowerInfo&)entityInfo, listener);
		watchTower->entityType = EntityCategory_STATIC_ENTITY;
		watchTower->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		watchTower->staticEntityType = EntityType_WATCH_TOWER;

		toSpawnEntities.push_back((Entity*)watchTower);
		return (StaticEntity*)watchTower;
	}
	break;

	case EntityType_ENEMY_GUARD_TOWER:
	{
		EnemyGuardTower* enemyGuardTower = new EnemyGuardTower(pos, { 64,64 }, 12, (const EnemyGuardTowerInfo&)entityInfo, listener);
		enemyGuardTower->entityType = EntityCategory_STATIC_ENTITY;
		enemyGuardTower->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		enemyGuardTower->staticEntityType = EntityType_ENEMY_GUARD_TOWER;

		toSpawnEntities.push_back((Entity*)enemyGuardTower);
		return (StaticEntity*)enemyGuardTower;
	}
	break;

	case EntityType_ENEMY_CANNON_TOWER:
	{
		EnemyCannonTower* enemyCannonTower = new EnemyCannonTower(pos, { 64,64 }, 12, (const EnemyCannonTowerInfo&)entityInfo, listener);
		enemyCannonTower->entityType = EntityCategory_STATIC_ENTITY;
		enemyCannonTower->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		enemyCannonTower->staticEntityType = EntityType_ENEMY_CANNON_TOWER;

		toSpawnEntities.push_back((Entity*)enemyCannonTower);
		return (StaticEntity*)enemyCannonTower;
	}
	break;
	
	case EntityType_FOOTMAN:
	{
		Footman* footman = new Footman(pos, (const FootmanInfo&)entityInfo, listener);
		footman->entityType = EntityCategory_DYNAMIC_ENTITY;
		footman->dynamicEntityType = EntityType_FOOTMAN;

		toSpawnEntities.push_back((Entity*)footman);
		return (DynamicEntity*)footman;
	}
	break;

	case EntityType_ELVEN_ARCHER:
	{

		ElvenArcher* elvenArcher = new ElvenArcher(pos, (const ElvenArcherInfo&)entityInfo, listener);
		elvenArcher->entityType = EntityCategory_DYNAMIC_ENTITY;
		elvenArcher->dynamicEntityType = EntityType_ELVEN_ARCHER;

		toSpawnEntities.push_back((Entity*)elvenArcher);
		return (DynamicEntity*)elvenArcher;
	}
	break;

	case EntityType_GRYPHON_RIDER:
	{
		GryphonRider* gryphonRider = new GryphonRider(pos, (const GryphonRiderInfo&)entityInfo, listener);
		gryphonRider->entityType = EntityCategory_DYNAMIC_ENTITY;
		gryphonRider->dynamicEntityType = EntityType_GRYPHON_RIDER;

		toSpawnEntities.push_back((Entity*)gryphonRider);
		return (DynamicEntity*)gryphonRider;
	}
	break;

	case EntityType_MAGE:
	{
		Mage* mage = new Mage(pos, (const MageInfo&)entityInfo, listener);
		mage->entityType = EntityCategory_DYNAMIC_ENTITY;
		mage->dynamicEntityType = EntityType_MAGE;

		toSpawnEntities.push_back((Entity*)mage);
		return (DynamicEntity*)mage;
	}
	break;

	case EntityType_PALADIN:
	{
		Paladin* paladin = new Paladin(pos, (const PaladinInfo&)entityInfo, listener);
		paladin->entityType = EntityCategory_DYNAMIC_ENTITY;
		paladin->dynamicEntityType = EntityType_PALADIN;

		toSpawnEntities.push_back((Entity*)paladin);
		return (DynamicEntity*)paladin;
	}
	break;

	case EntityType_TURALYON:
	{
		Turalyon* turalyon = new Turalyon(pos, (const TuralyonInfo&)entityInfo, listener);
		turalyon->entityType = EntityCategory_DYNAMIC_ENTITY;
		turalyon->dynamicEntityType = EntityType_TURALYON;

		toSpawnEntities.push_back((Entity*)turalyon);
		return (DynamicEntity*)turalyon;
	}
	break;

	case EntityType_KHADGAR:
	{
		Khadgar* khadgar = new Khadgar(pos, (const KhadgarInfo&)entityInfo, listener);
		khadgar->entityType = EntityCategory_DYNAMIC_ENTITY;
		khadgar->dynamicEntityType = EntityType_KHADGAR;

		toSpawnEntities.push_back((Entity*)khadgar);
		return (DynamicEntity*)khadgar;
	}
	break;

	case EntityType_ALLERIA:
	{
		Alleria* alleria = new Alleria(pos, (const AlleriaInfo&)entityInfo, listener);
		alleria->entityType = EntityCategory_DYNAMIC_ENTITY;
		alleria->dynamicEntityType = EntityType_ALLERIA;

		toSpawnEntities.push_back((Entity*)alleria);
		return (DynamicEntity*)alleria;
	}
	break;

	case EntityType_GRUNT:
	{
		Grunt* grunt = new Grunt(pos, (const GruntInfo&)entityInfo, listener);
		grunt->entityType = EntityCategory_DYNAMIC_ENTITY;
		grunt->dynamicEntityType = EntityType_GRUNT;

		toSpawnEntities.push_back((Entity*)grunt);
		return (DynamicEntity*)grunt;
	}
	break;

	case EntityType_TROLL_AXETHROWER:
	{
		TrollAxethrower* trollAxethrower = new TrollAxethrower(pos, (const TrollAxethrowerInfo&)entityInfo, listener);
		trollAxethrower->entityType = EntityCategory_DYNAMIC_ENTITY;
		trollAxethrower->dynamicEntityType = EntityType_TROLL_AXETHROWER;

		toSpawnEntities.push_back((Entity*)trollAxethrower);
		return (DynamicEntity*)trollAxethrower;
	}
	break;

	case EntityType_DRAGON:
	{
		Dragon* dragon = new Dragon(pos, (const DragonInfo&)entityInfo, listener);
		dragon->entityType = EntityCategory_DYNAMIC_ENTITY;
		dragon->dynamicEntityType = EntityType_DRAGON;

		toSpawnEntities.push_back((Entity*)dragon);
		return (DynamicEntity*)dragon;
	}
	break;

	default:

		return nullptr;
	}

}

// -------------------------------------------------------------
// -------------------------------------------------------------

bool j1EntityFactory::Load(pugi::xml_node& save)
{
	bool ret = true;

	pugi::xml_node node;

	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {
		// MYTODO: Add some code here
		dynEnt++;
	}

	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {
		// MYTODO: Add some code here
		statEnt++;
	}

	return ret;
}

bool j1EntityFactory::Save(pugi::xml_node& save) const
{
	bool ret = true;

	pugi::xml_node node;

	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {
		// MYTODO: Add some code here
		dynEnt++;
	}

	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {
		// MYTODO: Add some code here
		statEnt++;
	}

	return ret;
}

void j1EntityFactory::DestroyEntity(StaticEntity* elem)
{
	activeStaticEntities.remove(elem);
}