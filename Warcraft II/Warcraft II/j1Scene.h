#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "Animation.h"
#include "j1Menu.h"
#include "j1Timer.h"

#include <vector>
#include <string>
using namespace std;

#define RECTANGLE_MIN_AREA 5

struct SDL_Texture;
struct UILifeBar;
struct UIWindow;
struct UILabel;
struct UIButton;
struct UIImage;
struct UISlider;
class UISlider_Info;
class UIInputText;

enum ENTITY_TYPE;

struct Particle;

struct SliderStruct;

enum TerenasDialogEvents {
	TerenasDialog_START,
	TerenasDialog_RESCUE_ALLERIA,
	TerenasDialog_RESCUE_KHADGAR,
	TerenasDialog_RESCUE_TURALYON,
	TerenasDialog_NONE
};
enum PauseMenuActions {
	PauseMenuActions_NOT_EXIST,
	PauseMenuActions_NONE,
	PauseMenuActions_CREATED,
	PauseMenuActions_DESTROY,
	PauseMenuActions_RETURN_MENU,
	PauseMenuActions_SETTINGS_MENU,
	PauseMenuActions_SLIDERFX,
	PauseMenuActions_SLIDERMUSIC
};
struct TerenasAdvices {
	UIImage* terenasImage;
	UILabel* text;
};

class j1Scene : public j1Module
{
public:
	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// On triggered
	void OnUIEvent(UIElement* UIelem, UI_EVENT UIevent);

	// Save
	bool Save(pugi::xml_node&) const;

	// Load
	bool Load(pugi::xml_node&);

	// Debug keys
	void DebugKeys();

	void CheckCameraMovement(float dt);

	void LoadInGameUI();
	void LoadBuildingMenu();
	void UnLoadBuildingMenu();
	void LoadResourcesLabels();
	void UnLoadResourcesLabels();
	void CreatePauseMenu();
	void DestroyPauseMenu();
	void CreateSettingsMenu();
	void DestroySettingsMenu();
	void DestroyAllUI();
	PauseMenuActions GetPauseMenuActions();

	void LoadTerenasDialog(TerenasDialogEvents dialogEvent);
	void UnLoadTerenasDialog();

	iPoint FindClosestValidTile(iPoint tile) const;

	bool LoadKeys(pugi::xml_node&);

public:

	//Building costs
	int keepCost = 500;
	int castleCost = 1500;
	int barracks2Cost = 1000;
	int chickenFarmCost = 500;
	int stablesCost = 900;
	int gryphonAviaryCost = 400;
	int mageTowerCost = 1000;
	int churchCost = 900;
	int blacksmithCost = 800;
	int elvenLumberCost = 600;
	int scoutTowerCost = 400;
	int guardTowerCost = 600;
	int cannonTowerCost = 800;

	// Camera
	float up = false, down = false, left = false, right = false;
	uint width = 0;
	uint height = 0;
	float scale = 0;

	// Player
	bool god = false;

	bool pause = false;

	bool hasGoldChanged = false;
	bool hasFoodChanged = false;

	UIImage* entitiesStats;
	ENTITY_TYPE GetAlphaBuilding();
	void SetAplphaBuilding(ENTITY_TYPE alphaBuilding);

	// Movement
	bool debugDrawMovement = true;
	bool debugDrawPath = false;
	bool debugDrawMap = false;
	bool debugDrawAttack = false;

	bool isFrameByFrame = false;

	TerenasDialogEvents terenasDialogEvent = TerenasDialog_NONE;
	TerenasAdvices terenasAdvices;
private:

	// Draw rectangle
	iPoint startRectangle = { 0,0 };

	//UI
	UIButton *buildingButton, *chickenFarmButton, *elvenLumberButton, *blackSmithButton, *stablesButton, *gryphonAviaryButton, *mageTowerButton, *churchButton, *scoutTowerButton, *guardTowerButton, *cannonTowerButton;
	UILabel *buildingLabel, *chickenFarmLabel, *elvenLumberLabel, *blackSmithLabel, *stablesLabel, *gryphonAviaryLabel, *mageTowerLabel, *churchLabel, *scoutTowerLabel, *guardTowerLabel, *cannonTowerLabel;
	//Building costs
	UILabel *chickenFarmCostLabel, *elvenLumberCostLabel, *blackSmithCostLabel, *stablesCostLabel, *gryphonAviaryCostLabel, *mageTowerCostLabel, *churchCostLabel, *scoutTowerCostLabel, *guardTowerCostLabel, *cannonTowerCostLabel;

	UIImage *buildingMenu;

	//Frame InGame
	UIImage* inGameFrameImage;
	UILabel* goldLabel, *foodLabel;

	//Pause Menu
	UIButton* pauseMenuButt = nullptr, *settingsButt = nullptr, *continueButt = nullptr, *ReturnMenuButt = nullptr;
	UILabel* pauseMenuLabel = nullptr, *settingsLabel = nullptr, *continueLabel = nullptr, *ReturnMenuLabel = nullptr;
	UIImage* parchmentImg = nullptr;
	//Settings Menu
	UIButton* returnButt = nullptr, *fullScreenButt = nullptr;
	UILabel*  returnLabel = nullptr, *fullScreenLabel = nullptr;
	SliderStruct AudioFXPause;
	SliderStruct AudioMusicPause;


	bool buildingMenuOn = false;

	string orthogonalMap, isometricMap, warcraftMap;
	string orthogonalTexName, isometricTexName, warcraftTexName;
	string mainThemeMusicName;
	bool orthogonalActive, isometricActive, warcraftActive;

	SDL_Texture* debugTex = nullptr;

	iPoint mouse = { 0,0 };

	//Camera attributes
	float camSpeed = 0.0f;
	int camMovMargin = 0;

	SDL_Scancode buttonSaveGame = SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonLoadGame = SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonFullScreen = SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonGodMode = SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonMoveUp = SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonMoveDown = SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonMoveLeft = SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonMoveRight = SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonLeaveGame = SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonReloadMap = SDL_SCANCODE_UNKNOWN;

	ENTITY_TYPE alphaBuilding;

	PauseMenuActions pauseMenuActions = PauseMenuActions_NOT_EXIST;

	j1Timer terenasDialogTimer;
};

#endif //__j1SCENE1_H__