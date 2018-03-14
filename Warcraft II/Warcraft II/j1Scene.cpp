#include"Brofiler\Brofiler.h"

#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"

#include "j1Input.h"
#include "j1Textures.h"
#include "j1FadeToBlack.h"
#include "j1Audio.h"
#include "j1Collision.h"
#include "j1Particles.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1EntityFactory.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"

#include "UILabel.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UICursor.h"


j1Scene::j1Scene() : j1Module()
{
	name.assign("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	bool ret = true;

	LOG("Loading scene");

	// Load maps
	pugi::xml_node maps = config.child("maps");

	orthogonalMap = maps.child("orthogonal").attribute("name").as_string();
	orthogonalActive = maps.child("orthogonal").attribute("active").as_bool();
	orthogonalTexName = maps.child("orthogonal").attribute("tex").as_string();

	isometricMap = maps.child("isometric").attribute("name").as_string();
	isometricActive = maps.child("isometric").attribute("active").as_bool();
	isometricTexName = maps.child("isometric").attribute("tex").as_string();

	warcraftMap = maps.child("warcraft").attribute("name").as_string();
	warcraftActive = maps.child("warcraft").attribute("active").as_bool();
	warcraftTexName = maps.child("warcraft").attribute("tex").as_string();

	// Load songs

	// Load FX
	/*
	for (pugi::xml_node node = config.child("audio").child("fx").child("fx"); node; node = node.next_sibling("fx"))
		App->audio->LoadFx(node.attribute("name").as_string());
	*/

	//Load camera attributes
	pugi::xml_node camera = config.child("camera");

	camSpeed = camera.attribute("speed").as_float();
	camMovMargin = camera.attribute("movMarginPcnt").as_int();

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	bool ret = true;

	// Save camera info
	App->win->GetWindowSize(width, height);
	scale = App->win->GetScale();

	// Load an orthogonal, isometric or warcraft-based map
	if (orthogonalActive) {
		ret = App->map->Load(orthogonalMap.data());
		debugTex = App->tex->Load(orthogonalTexName.data());
	}
	else if (isometricActive) {
		ret = App->map->Load(isometricMap.data());
		debugTex = App->tex->Load(isometricTexName.data());
	}
	else if (warcraftActive) {
		ret = App->map->CreateNewMap();

		debugTex = App->tex->Load(warcraftTexName.data());
	}

	// Create walkability map
	if (ret)
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	//LoadInGameUI
	LoadInGameUI();

	//Calculate camera movement in pixels through the percentatge given
	camMovMargin = camMovMargin * ((width + height) / 2) / 100;

	return ret;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	bool ret = true;

	// debug pathfing ------------------
	static iPoint origin;
	static bool isSelected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (isSelected)
		{
			App->pathfinding->CreatePath(origin, p, DISTANCE_TO);
			isSelected = false;
		}
		else
		{
			origin = p;
			isSelected = true;
		}
	}

	// Player start position
	/*
	App->entities->playerData->startPos = App->map->data.GetObjectPosition("Player", "StartPos");
	App->entities->playerData->position = App->entities->playerData->startPos;
	*/

	return ret;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	bool ret = true;



	// Draw
	//App->map->Draw(); // map
	App->entities->Draw(); // entities

	// Debug pathfinding ------------------------------

	int x = 0, y = 0;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	App->render->Blit(debugTex, p.x, p.y);

	const vector<iPoint>* path = App->pathfinding->GetLastPath();

	for (uint i = 0; i < path->size(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->at(i).x, path->at(i).y);
		App->render->Blit(debugTex, pos.x, pos.y);
	}


	DebugKeys();
	CheckCameraMovement(dt);

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_REPEAT)
	{
		App->map->UnLoad();
		App->map->CreateNewMap();
	}

//	if (App->input->GetKey(SDL_SCANCODE_I) == KEY_REPEAT)
	//	App->win->scale += 0.05f;

	if (App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT) 
		App->win->scale -= 0.05f;



	return ret;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	bool ret = true;

	LOG("Freeing scene");

	App->audio->PauseMusic();
	App->map->UnLoad();
	App->tex->UnLoad(debugTex);

	// Set to nullptr the pointers to the UI elements

	return ret;
}

// Debug keys
void j1Scene::DebugKeys()
{
	// F1: start from the beginning of the first level
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		/*
		if (index == 0)
			App->entities->playerData->position = App->entities->playerData->startPos;
		else
			index = 0;

		App->fade->FadeToBlack(this, this, FADE_LESS_SECONDS, FADE_TYPE::FADE_TYPE_SLIDE);
		*/
	}

	// F2: start from the beginning of the current level
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		/*
		App->fade->FadeToBlack(this, this, FADE_LESS_SECONDS, FADE_TYPE::FADE_TYPE_SLIDE);
		*/
	}

	// F4: change between maps
	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {
		/*
		if (index == 0)
			index = 1;
		else
			index = 0;

		App->fade->FadeToBlack(this, this, FADE_LESS_SECONDS, FADE_TYPE::FADE_TYPE_SLIDE);
		*/
	}

	// F5: save the current state
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		App->SaveGame();
	}

	// F6: load the previous state
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		App->LoadGame();
	}

	// F7: fullscreen
	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
		App->win->SetFullscreen();

	// F10: God mode
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		god = !god;

	// 1, 2, 3: camera blit
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN && App->map->blitOffset < 15 && App->map->cameraBlit)
		App->map->blitOffset += 7;

	else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN && App->map->blitOffset > -135 && App->map->cameraBlit)
		App->map->blitOffset -= 7;

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
		App->map->cameraBlit = !App->map->cameraBlit;
}

void j1Scene::CheckCameraMovement(float dt) {

	int downMargin = -(App->map->data.height * App->map->data.tileHeight) + height / scale;
	int rightMargin = -(App->map->data.width * App->map->data.tileWidth) + width / scale;

	downMargin = -10000;
	rightMargin = -10000;

	//Move with arrows
	//UP
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT && App->render->camera.y <= 0)
		App->render->camera.y += camSpeed * dt;
	//DOWN
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT && App->render->camera.y >= downMargin)
		App->render->camera.y -= camSpeed * dt;
	//LEFT
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT && App->render->camera.x <= 0)
		App->render->camera.x += camSpeed * dt;
	//RIGHT
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT && App->render->camera.x >= rightMargin)
		App->render->camera.x -= camSpeed * dt;

	//Move with mouse
	App->input->GetMousePosition(mouse.x, mouse.y);

	////UP
	//if (mouse.y <= camMovMargin/scale && App->render->camera.y <= 0)
	//	App->render->camera.y += camSpeed * dt;
	////DOWN
	//if (mouse.y >= (height - camMovMargin) / scale && App->render->camera.y >= downMargin)
	//	App->render->camera.y -= camSpeed * dt;
	////LEFT
	//if (mouse.x <= camMovMargin / scale && App->render->camera.x <= 0)
	//	App->render->camera.x += camSpeed * dt;
	////RIGHT
	//if (mouse.x >= (width - camMovMargin) / scale && App->render->camera.x >= rightMargin)
	//	App->render->camera.x -= camSpeed * dt;

}

void j1Scene::LoadInGameUI()
{
	UIButton_Info buildingButtonInfo;
	buildingButtonInfo.normalTexArea = {0, 0, 79, 20};
	buildingButtonInfo.hoverTexArea = { 79, 0, 79, 20 };
	buildingButtonInfo.pressedTexArea = { 158, 0, 79, 20 };
	buildingButton = App->gui->CreateUIButton({ (int)App->render->camera.w - buildingButtonInfo.normalTexArea.w, 5 }, buildingButtonInfo, this, nullptr);

	UILabel_Info buildingLabelInfo;
	buildingLabelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	buildingLabelInfo.normalColor = White_;
	buildingLabelInfo.text = "Buildings";
	buildingLabel = App->gui->CreateUILabel({ 5,5 }, buildingLabelInfo, this, buildingButton);

}

void j1Scene::LoadBuildingMenu()
{
	UIImage_Info buildingMenuInfo;
	buildingMenuInfo.texArea = { 0,20,172,251 };
	buildingMenu = App->gui->CreateUIImage({ -92, 1 }, buildingMenuInfo, this, buildingButton);
	buildingMenuOn = true;

	UIButton_Info chickenFarmInfo;
	chickenFarmInfo.normalTexArea = { 202,21,46,38 };
	chickenFarmInfo.hoverTexArea = { 249,21,46,38 };
	chickenFarmInfo.pressedTexArea = { 296,21,46,38 };
	chickenFarmButton = App->gui->CreateUIButton({ 10, 30 }, chickenFarmInfo, this, buildingMenu);

	UILabel_Info chickenFarmLabelInfo;
	chickenFarmLabelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	chickenFarmLabelInfo.normalColor = White_;
	chickenFarmLabelInfo.text = "Chicken Farm";
	chickenFarmLabel = App->gui->CreateUILabel({ 64, 45 }, chickenFarmLabelInfo, this, buildingMenu);
}

void j1Scene::UnLoadBuildingMenu()
{
	App->gui->DestroyElement(buildingMenu);
	App->gui->DestroyElement(chickenFarmButton);
	App->gui->DestroyElement(chickenFarmLabel);
	buildingMenuOn = false;
}

void j1Scene::OnUIEvent(UIElement* UIelem, UI_EVENT UIevent)
{
	switch (UIevent)
	{
	case UI_EVENT_MOUSE_ENTER:

		break;

	case UI_EVENT_MOUSE_LEAVE:

		break;

	case UI_EVENT_MOUSE_LEFT_CLICK:
		if (UIelem == buildingButton) {
			if (!buildingMenuOn) 			
				LoadBuildingMenu();
		
			else 
				UnLoadBuildingMenu();
			
		}

		break;

	case UI_EVENT_MOUSE_LEFT_UP:

		break;
	}
}

// -------------------------------------------------------------
// -------------------------------------------------------------

// Save
bool j1Scene::Save(pugi::xml_node& save) const
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
bool j1Scene::Load(pugi::xml_node& save)
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