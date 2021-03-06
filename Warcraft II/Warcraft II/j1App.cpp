#include <iostream> 

#include "Defs.h"
#include "p2Log.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Intro.h"
#include "j1Map.h"
#include "j1Particles.h"
#include "j1Collision.h"
#include "j1FadeToBlack.h"
#include "j1EntityFactory.h"
#include "j1Pathfinding.h"
#include "j1Fonts.h"
#include "j1Gui.h"
#include "j1Player.h"
#include "j1Console.h"
#include "j1Menu.h"
#include "j1Movement.h"
#include "j1PathManager.h"
#include "j1FinishGame.h"
#include "j1Printer.h"
#include "j1EnemyWave.h"
#include "j1FogOfWar.h"
#include "j1Video.h"
#include <time.h>

#include "j1App.h"
#include "Brofiler\Brofiler.h"

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	wantToSave = wantToLoad = false;

	input = new j1Input();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	audio = new j1Audio();
	scene = new j1Scene();
	intro = new j1Intro();
	map = new j1Map();
	particles = new j1Particles();
	collision = new j1Collision();
	fade = new j1FadeToBlack();
	entities = new j1EntityFactory();
	pathfinding = new j1PathFinding();
	font = new j1Fonts();
	gui = new j1Gui();
	player = new j1Player();
	console = new j1Console();
	menu = new j1Menu();
	movement = new j1Movement();
	pathmanager = new j1PathManager(MS_PATHFINDING);
	finish = new j1FinishGame();
	printer = new j1Printer();
	wave = new j1EnemyWave();
	fow = new j1FogOfWar();
	video = new j1Video();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);

	/// Do not change this order -->
	AddModule(pathfinding);
	AddModule(collision);
	AddModule(printer);
	AddModule(movement);
	AddModule(pathmanager);
	AddModule(particles);
	AddModule(entities);
	AddModule(wave);
	/// <-- Do not change this order

	AddModule(font);

	AddModule(player);
	AddModule(video);
	AddModule(scene);
	AddModule(intro);

	AddModule(fow);

	AddModule(finish);
	AddModule(menu);
	AddModule(console);

	AddModule(gui);
	AddModule(fade);

	// render last to swap buffer
	AddModule(render);

	menu->active = false;
	//intro->active = false;
	map->active = false;
	scene->active = false;
	finish->active = false;
	player->active = false;
	entities->active = false;
	collision->active = false;
	pathfinding->active = false;
	pathmanager->active = false;
	movement->active = false;
	particles->active = false;
	wave->active = false;
	fow->active = false;

	srand(time(NULL));
}

// Destructor
j1App::~j1App()
{
	// release modules
	list<j1Module*>::reverse_iterator item;
	item = modules.rbegin();

	while (item != modules.rend())
	{
		delete *item;
		item++;
	}
	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.push_back(module);
}

// Called before render is available
bool j1App::Awake()
{
	pugi::xml_node		appConfig;

	bool ret = false;

	loadGame = "save_game.xml";
	saveGame = "save_game.xml";

	config = LoadConfig(configFile); //root node

	if (!config.empty())
	{
		// self-config
		ret = true;
		appConfig = config.child("app");
		title.assign(appConfig.child("title").childValue());
		organization.assign(appConfig.child("organization").childValue());
		capFrames = config.child("renderer").child("CapFrames").attribute("value").as_uint();
	}

	if (ret == true)
	{
		list<j1Module*>::const_iterator item;
		item = modules.begin();

		while (item != modules.end() && ret)
		{
			ret = (*item)->Awake(config.child((*item)->name.data()));
			item++;
		}
	}

	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	bool ret = true;
	list<j1Module*>::const_iterator item;
	item = modules.begin();

	while (item != modules.end() && ret)
	{
		if ((*item)->active)
			ret = (*item)->Start();

		item++;
	}

	//Set title
	static char title[256];
	sprintf_s(title, 256, "Warcraft II: The Stolen Artifacts");
	App->win->SetTitle(title);

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;
	PrepareUpdate();

	if (scene->GetPauseMenuActions() != PauseMenuActions_NOT_EXIST)
		dt = 0;

	if (input->GetWindowEvent(WE_QUIT) || quitGame)
		ret = false;

	if (ret)
		ret = PreUpdate();

	if (ret)
		ret = DoUpdate();

	if (ret)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& configFile) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = configFile.loadFile("config.xml");

	if (result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = configFile.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	perfClock.Start();
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Magenta);

	if (wantToSave)
		SavegameNow();

	if (wantToLoad)
		LoadGameNow();

	float msSinceStartup = clock.Read();

	uint32 actualFrameMs = perfClock.ReadMs();

	lastFrameMs = actualFrameMs;

	uint32 framesOnLastUpdate = 0;
	frameCount++;

	// Cap frames
	if (!App->render->vsync && toCap) {
		float toVsync = 1000 / capFrames;

		if (actualFrameMs < toVsync)
			SDL_Delay(toVsync - actualFrameMs);
	}

	double fps = 1000.0f / perfClock.ReadMs();

	double avgFPS = (float)frameCount / clock.ReadSec();

	dt = 1.0f / fps;

	string capOnOff;
	if (toCap)
		capOnOff = "on";
	else
		capOnOff = "off";

	string vSyncOnOff;
	if (App->render->vsync)
		vSyncOnOff = "on";
	else
		vSyncOnOff = "off";

	string godMode;
	if (App->scene->god)
		godMode = "on";
	else
		godMode = "off";


//	sprintf_s(title, 256, "FPS: %.2f | AvgFPS: %.2f | Last Frame Ms: %02u",
	//	fps, avgFPS, lastFrameMs);

	if (App->scene->pause) {
		auxiliarDt = dt;
		dt = 0.0f;
	}
	
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Orchid);

	bool ret = true;
	list<j1Module*>::const_iterator item;
	item = modules.begin();
	j1Module* pModule = NULL;

	for (item = modules.begin(); item != modules.end() && ret == true; ++item)
	{
		pModule = *item;

		if (!pModule->active) {
			continue;
		}

		ret = (*item)->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Azure);

	bool ret = true;
	list<j1Module*>::const_iterator item;
	item = modules.begin();

	j1Module* pModule = NULL;

	for (item = modules.begin(); item != modules.end() && ret; ++item)
	{
		pModule = *item;

		if (!pModule->active) {
			continue;
		}

		ret = (*item)->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::LightSeaGreen);

	bool ret = true;
	list<j1Module*>::const_iterator item;
	j1Module* pModule = NULL;

	for (item = modules.begin(); item != modules.end() && ret; ++item)
	{
		pModule = *item;

		if (!pModule->active) {
			continue;
		}

		ret = (*item)->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	bool ret = true;
	list<j1Module*>::reverse_iterator item;
	item = modules.rbegin();

	while (item != modules.rend() && ret)
	{
		ret = (*item)->CleanUp();
		item++;
	}

	return ret;
}


// ---------------------------------------
uint32 j1App::GetSecondsSinceAppStartUp() 
{
	return clock.Read();
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if (index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.data();
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.data();
}

// Load / Save
void j1App::LoadGame()
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list

	wantToLoad = true;
}

// ---------------------------------------
void j1App::SaveGame() const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	wantToSave = true;
}

// ---------------------------------------
void j1App::GetSaveGames(list<string>& list_to_fill) const
{
	// need to add functionality to file_system module for this to work
}

bool j1App::LoadGameNow()
{
	bool ret = false;

	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_parse_result result = data.loadFile(loadGame.data());

	if (result != NULL)
	{
		LOG("Loading new Game State from %s...", loadGame.data());

		root = data.child("game_state");

		list<j1Module*>::const_iterator item = modules.begin();
		ret = true;

		while (item != modules.end() && ret)
		{
			ret = (*item)->Load(root.child((*item)->name.data()));
			item++;
		}

		data.reset();
		if (ret)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", (*item) ? (*item)->name.data() : "unknown");
	}
	else
		LOG("Could not parse game state xml file %s. pugi error: %s", loadGame.data(), result.description());

	wantToLoad = false;
	return ret;
}

bool j1App::SavegameNow() const
{
	bool ret = true;

	LOG("Saving Game State to %s...", saveGame.data());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;

	root = data.append_child("game_state");

	list<j1Module*>::const_iterator item = modules.begin();

	while (item != modules.end() && ret)
	{
		ret = (*item)->Save(root.append_child((*item)->name.data()));
		item++;
	}

	if (ret)
	{
		data.save_file(saveGame.data());
		LOG("... finished saving", );
	}
	else
		LOG("Save process halted from an error in module %s", (*item) ? (*item)->name.data() : "unknown");

	data.reset();
	wantToSave = false;
	return ret;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

pugi::xml_node GetNode(pugi::xml_node& node, char* name, bool create)
{
	pugi::xml_node valueNode;
	if ((create && node.child(name) == NULL) || node.child(name) == NULL)
	{
		valueNode = node.append_child(name);
	}
	else
	{
		valueNode = node.child(name);
	}
	return valueNode;
}

void SaveAttribute(int value, char* name, pugi::xml_node& node, bool create)
{
	pugi::xml_node valueNode = GetNode(node, name, create);

	valueNode.append_attribute(name) = value;
}

void SaveAttribute(float value, char* name, pugi::xml_node& node, bool create)
{
	pugi::xml_node valueNode = GetNode(node, name, create);

	valueNode.append_attribute(name) = value;
}

void SaveAttribute(SDL_Rect value, char* name, pugi::xml_node& node, bool create)
{
	pugi::xml_node valueNode = GetNode(node, name, create);

	valueNode.append_attribute("x") = value.x;
	valueNode.append_attribute("y") = value.y;
	valueNode.append_attribute("w") = value.w;
	valueNode.append_attribute("h") = value.h;
}

void SaveAttribute(uchar* value, char* name, pugi::xml_node& node, bool create, int size)
{
	pugi::xml_node valueNode = GetNode(node, name, create);

	for (pugi::xml_node child = valueNode.first_child(); child;)
	{
		valueNode.remove_child(child);
	}

	for (int i = 0; i < size; ++i)
	{
		valueNode.append_child(name).append_attribute(name) = value[i];
	}
}
