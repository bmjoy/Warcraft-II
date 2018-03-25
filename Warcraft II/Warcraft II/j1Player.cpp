#include "j1Player.h"

#include "Defs.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1EntityFactory.h"
#include "j1Scene.h"
#include "j1Gui.h"
#include "j1Particles.h"

#include "UILabel.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UILifeBar.h"

j1Player::j1Player() : j1Module()
{
	name.assign("scene");
}

// Destructor
j1Player::~j1Player() {}

// Called before render is available
bool j1Player::Awake(pugi::xml_node& config)
{
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Player::Start()
{
	bool ret = true;

	return ret;
}

bool j1Player::Update(float dt) {

	CheckIfPlaceBuilding();

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		if (stables != nullptr) {
			if (stables->GetIsFinishedBuilt()) {
				Entity* ent = (Entity*)stables;
				ent->SetDamageLife(20);
				stables->CheckBuildingState();
				if (entitySelectedStats.entitySelected == ent) {
					entitySelectedStats.HP->SetText(ent->GetStringLife());
					entitySelectedStats.lifeBar->DecreaseLife(20);
				}
			}
		}

	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
		if (mageTower != nullptr)
			if (mageTower->GetIsFinishedBuilt()) {
				Entity* ent = (Entity*)mageTower;
				ent->SetDamageLife(20);
				mageTower->CheckBuildingState();
				if (entitySelectedStats.entitySelected == ent) {
					entitySelectedStats.HP->SetText(ent->GetStringLife());
					entitySelectedStats.lifeBar->DecreaseLife(20);
				}
			}
	

	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
		if (!scoutTower.empty())
			if (scoutTower.back()->GetIsFinishedBuilt()) {
				Entity* ent = (Entity*)scoutTower.back();
				ent->SetDamageLife(20);
				scoutTower.back()->CheckBuildingState();
				if (entitySelectedStats.entitySelected == ent) {
					entitySelectedStats.HP->SetText(ent->GetStringLife());
					entitySelectedStats.lifeBar->DecreaseLife(20);
				}
			}
	

	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
		if (gryphonAviary != nullptr)
			if (gryphonAviary->GetIsFinishedBuilt()) {
				Entity* ent = (Entity*)gryphonAviary;
				ent->SetDamageLife(20);
				gryphonAviary->CheckBuildingState();
				if (entitySelectedStats.entitySelected == ent) {
					entitySelectedStats.HP->SetText(ent->GetStringLife());
					entitySelectedStats.lifeBar->DecreaseLife(20);
				}
			}
		
	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
		if (!chickenFarm.empty()) 
			if (chickenFarm.back()->GetIsFinishedBuilt()) {
				Entity* ent = (Entity*)chickenFarm.back();
				ent->SetDamageLife(20);
				chickenFarm.back()->CheckBuildingState();
				if (entitySelectedStats.entitySelected == ent) {
					entitySelectedStats.HP->SetText(ent->GetStringLife());
					entitySelectedStats.lifeBar->DecreaseLife(20);
				}
			}
		

	//Life Bar on building 
	if (entitySelectedStats.entitySelected != nullptr) {
		if (!((StaticEntity*)entitySelectedStats.entitySelected)->GetIsFinishedBuilt()) {
			entitySelectedStats.lifeBar->SetLife(((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTimer() * entitySelectedStats.entitySelected->GetMaxLife() / 10);
		}
		else if (((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTimer() == ((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTime()) {
			entitySelectedStats.lifeBar->SetLife(((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTimer() * entitySelectedStats.entitySelected->GetMaxLife() / 10);
			entitySelectedStats.HP->SetText(entitySelectedStats.entitySelected->GetStringLife());
			entitySelectedStats.HP->SetLocalPos({ 5, App->scene->entitiesStats->GetLocalRect().h - 17});
		}
	}


	return true;
}

iPoint j1Player::GetMouseTilePos() {

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint mousePos = App->render->ScreenToWorld(x, y);
	iPoint mouseTile = App->map->WorldToMap(mousePos.x, mousePos.y);

	return mouseTile;
}

iPoint j1Player::GetMousePos() {

	iPoint mouseTilePos = App->map->MapToWorld(GetMouseTilePos().x, GetMouseTilePos().y);

	return mouseTilePos;
}

void j1Player::CheckIfPlaceBuilding()
{

	// Mouse position (world and map coords)
	float auxX = (int)GetMousePos().x;
	float auxY = (int)GetMousePos().y;
	fPoint buildingPos = { auxX, auxY };

	ENTITY_TYPE alphaBuilding = App->scene->GetAlphaBuilding();

	//Creates static entities (buildings)
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) 
	{

		SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);

		switch (alphaBuilding) 
		{


		case EntityType_CHICKEN_FARM:
			StaticEntity* c;
			c = (StaticEntity*)App->entities->AddEntity(EntityType_CHICKEN_FARM, buildingPos, App->entities->GetBuildingInfo(EntityType_CHICKEN_FARM), this);
			App->scene->SetAplphaBuilding(EntityType_NONE);
			c->SetStringLife(c->GetCurrLife(), c->GetMaxLife());
			chickenFarm.push_back(c);
			break;

		case EntityType_STABLES:
			stables = (StaticEntity*)App->entities->AddEntity(EntityType_STABLES, buildingPos, App->entities->GetBuildingInfo(EntityType_STABLES), this);
			App->scene->SetAplphaBuilding(EntityType_NONE);
			stables->SetStringLife(stables->GetCurrLife(), stables->GetMaxLife());
			break;

		case EntityType_GRYPHON_AVIARY:
			gryphonAviary = (StaticEntity*)App->entities->AddEntity(EntityType_GRYPHON_AVIARY, buildingPos, App->entities->GetBuildingInfo(EntityType_GRYPHON_AVIARY), this);
			App->scene->SetAplphaBuilding(EntityType_NONE);
			gryphonAviary->SetStringLife(gryphonAviary->GetCurrLife(), gryphonAviary->GetMaxLife());
			break;

		case EntityType_MAGE_TOWER:
			mageTower = (StaticEntity*)App->entities->AddEntity(EntityType_MAGE_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_MAGE_TOWER), this);
			App->scene->SetAplphaBuilding(EntityType_NONE);
			mageTower->SetStringLife(mageTower->GetCurrLife(), mageTower->GetMaxLife());
			break;

		case EntityType_SCOUT_TOWER:
			StaticEntity* s;
			s = (StaticEntity*)App->entities->AddEntity(EntityType_SCOUT_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_SCOUT_TOWER), this);
			App->scene->SetAplphaBuilding(EntityType_NONE);
			s->SetStringLife(s->GetCurrLife(), s->GetMaxLife());
			scoutTower.push_back(s);

			break;

		case EntityCategory_NONE:
			break;

		default:
			break;
		}
	}


	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		App->scene->SetAplphaBuilding(EntityType_NONE);

	//This makes that buildings on the scene aren't printed on alpha

	if (alphaBuilding != EntityType_MAX)
	{
		SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
		SDL_SetTextureAlphaMod(App->entities->GetNeutralBuildingTexture(), 255);
	}
}

// Called before quitting
bool j1Player::CleanUp()
{
	bool ret = true;

	return ret;
}

// -------------------------------------------------------------
// -------------------------------------------------------------

// Save
bool j1Player::Save(pugi::xml_node& save) const
{
	bool ret = true;

	/*
	if (save.child("gate") == NULL) {
	save.append_child("gate").append_attribute("opened") = gate;
	save.child("gate").append_attribute("fx") = fx;
	}
	else {
	save.child("gate").attribute("opened") = gate;
	save.child("gate").attribute("fx") = fx;
	}
	*/

	return ret;
}

// Load
bool j1Player::Load(pugi::xml_node& save)
{
	bool ret = true;

	/*
	if (save.child("gate") != NULL) {
	gate = save.child("gate").attribute("opened").as_bool();
	fx = save.child("gate").attribute("fx").as_bool();
	}
	*/

	return ret;

}


void j1Player::OnStaticEntitiesEvent(StaticEntity* staticEntity, EntitiesEvent entitiesEvent) {

	Entity* ent = (Entity*)staticEntity;

	switch (entitiesEvent)
	{
	case EntitiesEvent_NONE:
		break;
	case EntitiesEvent_RIGHT_CLICK:
		DeleteEntitiesMenu();
		break;
	case EntitiesEvent_LEFT_CLICK:
		DeleteEntitiesMenu();

		if (staticEntity->staticEntityType == EntityType_CHICKEN_FARM)
			MakeEntitiesMenu(ent->GetStringLife(), "Chicken Farm", { 241,34,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_GRYPHON_AVIARY)
			MakeEntitiesMenu(ent->GetStringLife(), "Gryphon Aviary", { 394,160,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_MAGE_TOWER)
			MakeEntitiesMenu(ent->GetStringLife(), "Mage Tower", { 394,202,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_SCOUT_TOWER)
			MakeEntitiesMenu(ent->GetStringLife(), "Scout Tower", { 394,34,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_STABLES)
			MakeEntitiesMenu(ent->GetStringLife(), "Stables", { 241,160,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_BARRACKS)
			MakeEntitiesMenu(ent->GetStringLife(), "Barracks", { 241,160,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_TOWN_HALL)
			MakeEntitiesMenu(ent->GetStringLife(), "Town Hall", { 241,160,50,41 }, ent);
	
		break;
	case EntitiesEvent_HOVER:
		if ((staticEntity->staticEntityType == EntityType_TOWN_HALL || staticEntity->staticEntityType == EntityType_BARRACKS) && ent->GetCurrLife() == ent->GetMaxLife())
			hoverCheck = HoverCheck_Upgrate;
		else if (ent->GetCurrLife() < ent->GetMaxLife())
			hoverCheck = HoverCheck_Repair;
		else
			hoverCheck = HoverCheck_None;

		CreateHoverButton(hoverCheck, { (int)ent->GetPosition().x, (int)ent->GetPosition().y, ent->GetSize().x, ent->GetSize().y}, staticEntity);


		break;
	case EntitiesEvent_LEAVE:
		DestroyHoverButton();
		break;
	case EntitiesEvent_CREATED:
		if (staticEntity->staticEntityType == EntityType_CHICKEN_FARM)
			MakeEntitiesMenu("NO_HP_TEXT", "Chicken Farm", { 241,34,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_GRYPHON_AVIARY)
			MakeEntitiesMenu("NO_HP_TEXT", "Gryphon Aviary", { 394,160,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_MAGE_TOWER)
			MakeEntitiesMenu("NO_HP_TEXT", "Mage Tower", { 394,202,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_SCOUT_TOWER)
			MakeEntitiesMenu("NO_HP_TEXT", "Scout Tower", { 394,34,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_STABLES)
			MakeEntitiesMenu("NO_HP_TEXT", "Stables", { 241,160,50,41 },ent);
		break;
	
	default:
		break;
	}

}

void j1Player::MakeEntitiesMenu(string HP_text, string entityName_text, SDL_Rect iconDim, Entity* currentEntity) {

	UILabel_Info labelInfo;
	labelInfo.text = entityName_text;
	labelInfo.verticalOrientation = VERTICAL_POS_TOP;
	entitySelectedStats.entityName = App->gui->CreateUILabel({ 5,5 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

	labelInfo.text = HP_text;
	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.verticalOrientation = VERTICAL_POS_BOTTOM;
	if (HP_text != "NO_HP_TEXT")
		entitySelectedStats.HP = App->gui->CreateUILabel({ 5, App->scene->entitiesStats->GetLocalRect().h }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	
	else if (HP_text == "NO_HP_TEXT") {
		labelInfo.text = "Building...";
		entitySelectedStats.HP = App->gui->CreateUILabel({ 80, App->scene->entitiesStats->GetLocalRect().h - 30 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	}

	UIImage_Info imageInfo;
	imageInfo.texArea = iconDim;
	imageInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;
	imageInfo.verticalOrientation = VERTICAL_POS_CENTER;
	entitySelectedStats.entityIcon = App->gui->CreateUIImage({ 5, App->scene->entitiesStats->GetLocalRect().h/2 }, imageInfo, nullptr, (UIElement*)App->scene->entitiesStats);


	UILifeBar_Info lifeInfo;
	lifeInfo.background = { 289,346,145,23 };
	lifeInfo.bar = { 300,373,128,8 };
	lifeInfo.maxLife = currentEntity->GetMaxLife();
	lifeInfo.life = ((StaticEntity*)currentEntity)->GetConstructionTimer() * currentEntity->GetMaxLife() / 10;
	if (lifeInfo.life > currentEntity->GetMaxLife())
		lifeInfo.life = currentEntity->GetCurrLife();
	lifeInfo.maxWidth = lifeInfo.bar.w;
	lifeInfo.lifeBarPosition = { 12, 10 };

	entitySelectedStats.lifeBar = App->gui->CreateUILifeBar({ 60, 50}, lifeInfo, nullptr, (UIElement*)App->scene->entitiesStats);

	entitySelectedStats.entitySelected = currentEntity;
}

void j1Player::DeleteEntitiesMenu() {

	App->gui->DestroyElement(entitySelectedStats.HP);
	App->gui->DestroyElement(entitySelectedStats.entityName);
	App->gui->DestroyElement(entitySelectedStats.entityIcon);
	App->gui->DestroyElement(entitySelectedStats.lifeBar);
	entitySelectedStats.entitySelected = nullptr;
}

void j1Player::CreateHoverButton(HoverCheck hoverCheck, SDL_Rect pos, StaticEntity* staticEntity) {

	UIButton_Info InfoButton;
	if (hoverCheck == HoverCheck_Repair) {
		InfoButton.normalTexArea  = { 579,34,49,41 };
		InfoButton.hoverTexArea   = { 629,34,49,41 };
		InfoButton.pressedTexArea = { 679,34,49,41 };
	}
	else if (hoverCheck == HoverCheck_Upgrate) {
		InfoButton.normalTexArea  = { 579,118,49,41 };
		InfoButton.hoverTexArea   = { 629,118,49,41 };
		InfoButton.pressedTexArea = { 679,118,49,41 };
	}
	InfoButton.horizontalOrientation = HORIZONTAL_POS_CENTER;
	InfoButton.verticalOrientation = VERTICAL_POS_CENTER;



	if (hoverCheck != HoverCheck_None) {
		hoverButtonStruct.hoverButton = App->gui->CreateUIButton({ pos.x + pos.w / 2, pos.y + pos.h / 2 }, InfoButton, this, nullptr, true);
		hoverButtonStruct.Entity_Hover = staticEntity;
	}
}

void j1Player::DestroyHoverButton() {
	if (hoverButtonStruct.hoverButton != nullptr) {
		App->gui->DestroyElement(hoverButtonStruct.hoverButton);
		hoverButtonStruct.Entity_Hover = nullptr;
	}
}

void j1Player::OnUIEvent(UIElement* UIelem, UI_EVENT UIevent) {

	switch (UIevent)
	{
	case UI_EVENT_NONE:
		break;
	case UI_EVENT_MOUSE_ENTER:
		break;
	case UI_EVENT_MOUSE_LEAVE:
		break;
	case UI_EVENT_MOUSE_RIGHT_CLICK:
		break;
	case UI_EVENT_MOUSE_LEFT_CLICK:
		if (hoverCheck == HoverCheck_Repair) {
			hoverButtonStruct.Entity_Hover->SetCurrLife(hoverButtonStruct.Entity_Hover->GetMaxLife());
			hoverButtonStruct.Entity_Hover->CheckBuildingState();
			entitySelectedStats.HP->SetText(hoverButtonStruct.Entity_Hover->GetStringLife());
			entitySelectedStats.lifeBar->SetLife(hoverButtonStruct.Entity_Hover->GetMaxLife());

		}
		else if (hoverCheck == HoverCheck_Upgrate)
		{
			//Use hoverButtonStruct
			//TODO JOAN
		}
		break;
	case UI_EVENT_MOUSE_RIGHT_UP:
		break;
	case UI_EVENT_MOUSE_LEFT_UP:
		break;
	case UI_EVENT_MAX_EVENTS:
		break;
	default:
		break;
	}


}
