#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"
#include "j1PathFinding.h"
#include "j1PathManager.h"
#include "j1Map.h"
#include "j1Movement.h"

#include "Entity.h"

#include "Brofiler\Brofiler.h"

j1PathManager::j1PathManager(double msSearchPerUpdate) : j1Module(), msSearchPerUpdate(msSearchPerUpdate)
{
	name.assign("pathmanager");
}

// Destructor
j1PathManager::~j1PathManager()
{

}

// Called before quitting
bool j1PathManager::CleanUp()
{

	return true;
}

bool j1PathManager::Update(float dt) 
{
	bool ret = true;

	if (searchRequests.size() > 0)
		UpdateSearches();

	if (pathPlanners.size() > 0)
		UpdatePathPlaners();

	return ret;
}

void j1PathManager::UpdateSearches() 
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Orchid);

	timer.Start();

	list<PathPlanner*>::const_iterator currSearch = searchRequests.begin();

	while (timer.ReadMs() < msSearchPerUpdate && searchRequests.size() > 0) {

		// Make one search cycle of this path request
		PathfindingStatus result = (*currSearch)->CycleOnce();

		// If the search has terminated, remove it from the list
		if (result == PathfindingStatus_PathFound || result == PathfindingStatus_PathNotFound) {
			currSearch = searchRequests.erase(currSearch);
		}
		else {
			// Move on to the next search
			currSearch++;
		}

		// The iterator may now be pointing to the end of the list.
		// If so, it must be reset to the beginning
		if (currSearch == searchRequests.end())
			currSearch = searchRequests.begin();
	}
}

void j1PathManager::UpdatePathPlaners()
{
	for (list<PathPlanner*>::const_iterator iterator = pathPlanners.begin(); iterator != pathPlanners.end(); ++iterator)
	{
		(*iterator)->HilevelUpdate();
	}
}

void j1PathManager::Register(PathPlanner* pathPlanner) 
{
	list<PathPlanner*>::const_iterator it = find(searchRequests.begin(), searchRequests.end(), pathPlanner);

	if (it == searchRequests.end())
		searchRequests.push_back(pathPlanner);
}

void j1PathManager::UnRegister(PathPlanner* pathPlanner) 
{
	pathPlanner->SetSearchRequested(false);

	searchRequests.remove(pathPlanner);
}

// ---------------------------------------------------------------------
// PATH PLANNER
// ---------------------------------------------------------------------

PathPlanner::PathPlanner(Entity* owner, Navgraph& navgraph) :entity(owner), navgraph(navgraph) 
{ 
	App->pathmanager->pathPlanners.push_back(this);
}

PathPlanner::~PathPlanner()
{
	if (currentSearch != nullptr)
		delete currentSearch;
	currentSearch = nullptr;

	if (trigger != nullptr)
		delete trigger;
	trigger = nullptr;

	if (hiLevelSearch != nullptr)
		delete 	hiLevelSearch;
	hiLevelSearch = nullptr;

	entity = nullptr;
}

bool PathPlanner::RequestAStar(iPoint origin, iPoint destination)
{
	bool ret = false;

	if (isSearchRequested)
		return false;

	if (!isHiLevelSearched)
	{
		LoadHiLevelSearch();
		isHiLevelSearched = true;
	}

	App->pathmanager->UnRegister(this);
	GetReadyForNewSearch();

	pathfindingAlgorithmType = PathfindingAlgorithmType_AStar;

	if (currentSearch != nullptr)
		delete currentSearch;
	currentSearch = nullptr;

	currentSearch = new j1PathFinding();

	// Set the walkability map
	currentSearch->SetMap(currentLowLevelMap);
	ret = true;
	// Invalidate if origin or destination are non-walkable
	if (ret)
		ret = currentSearch->InitializeAStar(origin, destination);

	if (ret)
		App->pathmanager->Register(this);

	return ret;
}

bool PathPlanner::RequestDijkstra(iPoint origin, FindActiveTrigger::ActiveTriggerType activeTriggerType, bool isPathRequested)
{
	bool ret = true;

	if (isSearchRequested)
		return false;

	App->pathmanager->UnRegister(this);
	GetReadyForNewSearch();

	pathfindingAlgorithmType = PathfindingAlgorithmType_Dijkstra;

	this->isPathRequested = isPathRequested;

	currentSearch = new j1PathFinding();

	switch (activeTriggerType) {

	case FindActiveTrigger::ActiveTriggerType_Goal:

		trigger = new FindActiveTrigger(activeTriggerType, entity);

		break;

	case FindActiveTrigger::ActiveTriggerType_Object:

		trigger = new FindActiveTrigger(activeTriggerType, ((StaticEntity*)entity)->staticEntityType);

		break;

	case FindActiveTrigger::ActiveTriggerType_NoType:
	default:

		break;
	}

	navgraph.SetNavgraph(currentSearch);

	// Invalidate if origin is non-walkable
	if (ret)
		ret = currentSearch->InitializeDijkstra(origin, trigger, isPathRequested);

	if (ret)
		App->pathmanager->Register(this);
	
	return ret;
}

void PathPlanner::GetReadyForNewSearch()
{
	pathfindingAlgorithmType = PathfindingAlgorithmType_NoType;
	isSearchCompleted = false;
	isSearchRequested = true;

	// Delete any active search
	if (currentSearch != nullptr)
		delete currentSearch;
	currentSearch = nullptr;

	if (trigger != nullptr)
		delete trigger;
	trigger = nullptr;
}

PathfindingStatus PathPlanner::CycleOnce()
{
	PathfindingStatus result = PathfindingStatus_PathNotFound;

//	if (UpdateNavgraph())
//		App->pathfinding->SetMap(currentLowLevelMap, App->map->lowLevelWalkabilityMap);

	switch (pathfindingAlgorithmType) {

	case PathfindingAlgorithmType_AStar:

		result = currentSearch->CycleOnceAStar();

		// Let the bot know of the failure to find a path
		if (result == PathfindingStatus_PathNotFound) {
			// ERROR!
		}
		// Let the bot know a path has been found
		else if (result == PathfindingStatus_PathFound)

			isSearchCompleted = true;

		break;

	case PathfindingAlgorithmType_Dijkstra:

		result = currentSearch->CycleOnceDijkstra();

		// Let the bot know of the failure to find a path/tile
		if (result == PathfindingStatus_PathNotFound) {
			// ERROR!
		}
		// Let the bot know a path/tile has been found
		else if (result == PathfindingStatus_PathFound)

			isSearchCompleted = true;

		break;
	}

	return result;
}

bool PathPlanner::UpdateNavgraph()
{
	bool ret = false;
	fPoint pos = entity->GetPos();
	
	for (list<WalkabilityMap>::iterator iterator = App->map->lowLevelWalkabilityMap.begin();
		 iterator != App->map->lowLevelWalkabilityMap.end(); ++iterator)
	{
		if ((*iterator).map != currentLowLevelMap.map)
		{
			SDL_Rect roomRect = { (*iterator).position.x, (*iterator).position.y, 
								  (*iterator).width * defaultSize, (*iterator).height * defaultSize };
			SDL_Rect entityRect = { pos.x, pos.y, defaultSize, defaultSize };
			SDL_Rect result{ 0,0,0,0 };
			if (SDL_IntersectRect(&roomRect, &entityRect, &result))
			{
				currentLowLevelMap = (*iterator);
				ret = true;
			}
		}
		if (ret)
			break;
	}

	if (ret)
	{
		DynamicEntity* tempEnt = (DynamicEntity*)entity;
		SingleUnit* singleUnit = tempEnt->GetSingleUnit();

		RequestAStar(singleUnit->currTile, singleUnit->goal);
	}
	return ret;
}


bool PathPlanner::HilevelUpdate()
{
	bool ret = false;

	if (haveGoal)
	{
		if (unitReachDestination)
		{
			currRoomPos = nextRoomPos;
			currRoom = nextRoom;
			nextRoomPos = hiLevelPath.front();

			currentLowLevelMap = currRoom->walkabilityMap;
			App->pathfinding->SetMap(currentLowLevelMap);

			SDL_Rect result{ 0,0,0,0 };

			RoomMap* map = App->map->GetMap();
	//		if (test)
		//	{
				for (list<Room>::iterator iterator = map->rooms.begin(); iterator != map->rooms.end(); ++iterator)
				{
					SDL_Rect goalRect{ 2240, 0, 50 * 32, 50 * 32 };

					if (SDL_IntersectRect(&(*iterator).collider, &goalRect, &result))
					{
						nextRoom = &(*iterator);

						unitReachDestination = false;
						unitNeedPath = true;
						ret = true;

						break;
					}
				}

		//	}

			//list<Room>::iterator iterator = map->rooms.begin();
			//for (; iterator != map->rooms.end(); ++iterator)
			//{
			//	if (nextRoomPos.x == (*iterator).x  && nextRoomPos.y == (*iterator).y)
			//	{
			//		currentLowLevelMap = (*iterator).walkabilityMap;
			//		nextRoom = &(*iterator);
			//		unitReachDestination = false;
			//		unitNeedPath = true;
			//		test = ret = true;

			//		break;
			//	}
			//}
		}

		if (unitNeedPath)
		{
			//App->pathmanager->UnRegister(this);
//			App->pathmanager->Register(this);
			isSearchRequested = false;
			singleUnit->roomGoal = GetExitPoint();

			if (singleUnit->roomGoal != -1)
				ret = RequestAStar(singleUnit->currTile, singleUnit->roomGoal);

			else if (singleUnit->roomGoal == -1)
			{
				ret = RequestAStar(singleUnit->currTile, singleUnit->goal);
			}
			unitNeedPath = false;
			singleUnit->isGoalChanged = true;
		}

		if (singleUnit->currTile == singleUnit->roomGoal)
		{
			ret = unitReachDestination = true;
		}

		else if (singleUnit->currTile == singleUnit->goal)
		{
			unitReachDestination = false;
			unitNeedPath = false;
			haveGoal = false;

			ret = true;
		}
	}
	
	return ret;
}

iPoint PathPlanner::GetExitPoint()
{
	iPoint exitPoint{ -1, -1 };
	iPoint auxRoomPos = nextRoomPos - currRoomPos;

	//North
	if (auxRoomPos.y == 1)
	{
		exitPoint = currRoom->exitPointN;
	}

	//South
	else if (auxRoomPos.y == -1)
	{
		exitPoint = currRoom->exitPointS;
	}

	//East
	else if (auxRoomPos.x == 1)
	{
		exitPoint = { currRoom->exitPointE.x / 32,currRoom->exitPointE.y / 32 };
	}

	//West
	else if (auxRoomPos.x == -1)
	{
		exitPoint = currRoom->exitPointW;
	}

	return exitPoint;
}


void PathPlanner::LoadHiLevelSearch()
{
	if (hiLevelSearch == nullptr)
		hiLevelSearch = new j1PathFinding();

	hiLevelSearch->SetMap(App->map->hiLevelWalkabilityMap);

	if (entity != nullptr)
	{
		DynamicEntity* tempEnt = (DynamicEntity*)entity;
		singleUnit = tempEnt->GetSingleUnit();
	}
	fPoint pos = entity->GetPos();
	RoomMap* map = App->map->GetMap();
	SDL_Rect entityRect = { pos.x, pos.y, defaultSize, defaultSize };
	SDL_Rect result{ 0,0,0,0 };

	iPoint originRoom{ -1,-1 };

	DynamicEntity* tempEnt = (DynamicEntity*)entity;
	SingleUnit* singleUnit = tempEnt->GetSingleUnit();




	for (list<Room>::iterator iterator = map->rooms.begin(); iterator != map->rooms.end(); ++iterator)
	{
		SDL_Rect goalRect{ singleUnit->goal.x * defaultSize, singleUnit->goal.y * defaultSize, 32, 32 };

		if (SDL_IntersectRect(&(*iterator).collider, &entityRect, &result))
		{
			currRoomPos = originRoom = { (*iterator).x, (*iterator).y };
			currRoom = &(*iterator);
		}
		if (SDL_IntersectRect(&(*iterator).collider, &goalRect, &result))
		{
			goalRoomPos = { (*iterator).x / 32, (*iterator).y / 32 };
			goalRoomPos = { 2,0 };
		}
	}
	if (!haveGoal)
		if (originRoom != -1 && goalRoomPos != -1)
		{
			hiLevelSearch->CreatePath(originRoom, goalRoomPos);
		//	hiLevelPath = *(hiLevelSearch->GetLastPath());

			hiLevelPath.push_back({ 2,0 });
			hiLevelPath.push_back({ 1,0 });

			if (hiLevelPath.size() > 0)
				nextRoomPos = hiLevelPath.back();
			else
				nextRoomPos = originRoom;
			unitNeedPath = true;
			haveGoal = true;


			for (list<Room>::iterator iterator = map->rooms.begin(); iterator != map->rooms.end(); ++iterator)
			{
				SDL_Rect goalRect{ 1600, 672, 20 * 32, 8 * 32 };

				if (SDL_IntersectRect(&(*iterator).collider, &goalRect, &result))
				{
					nextRoom = &(*iterator);
				}
			}
			currentLowLevelMap = currRoom->walkabilityMap;
			App->pathfinding->SetMap(currentLowLevelMap);
		}


}




vector<iPoint> PathPlanner::GetPath() const
{
	if (isSearchCompleted)

		if (pathfindingAlgorithmType == PathfindingAlgorithmType_AStar || (pathfindingAlgorithmType == PathfindingAlgorithmType_Dijkstra && isPathRequested))

			return *currentSearch->GetLastPath();
}

iPoint PathPlanner::GetTile() const
{
	if (isSearchCompleted)
		return currentSearch->GetLastTile();
}

bool PathPlanner::IsSearchCompleted() const
{
	return isSearchCompleted;
}

bool PathPlanner::IsSearchRequested() const
{
	return isSearchRequested;
}

void PathPlanner::SetSearchRequested(bool isSearchRequested)
{
	this->isSearchRequested = isSearchRequested;
}

void PathPlanner::SetCheckingCurrTile(bool isCheckingCurrTile) 
{
	if (trigger != nullptr)
		trigger->isCheckingCurrTile = isCheckingCurrTile;
}

void PathPlanner::SetCheckingNextTile(bool isCheckingNextTile) 
{
	if (trigger != nullptr)
		trigger->isCheckingNextTile = isCheckingNextTile;
}

void PathPlanner::SetCheckingGoalTile(bool isCheckingGoalTile) 
{
	if (trigger != nullptr)
		trigger->isCheckingGoalTile = isCheckingGoalTile;
}

// WalkabilityMap struct ---------------------------------------------------------------------------------

bool Navgraph::CreateNavgraph()
{
	return true; // App->map->CreateWalkabilityMap(hiW, hiH, &hiW, lowW , lowH, &lowData);
}

bool Navgraph::SetNavgraph(j1PathFinding* currentSearch) const
{
	if (currentSearch == nullptr)
		return false;

	//currentSearch->SetMap(hiLevelWalkabilityMap, lowLevelWalkabilityMap);

	return true; 
}

bool Navgraph::GetNavgraph()
{
	hiLevelWalkabilityMap = App->map->hiLevelWalkabilityMap;
	lowLevelWalkabilityMap = App->map->lowLevelWalkabilityMap;

	return true;
}

// FindActiveTrigger class ---------------------------------------------------------------------------------

FindActiveTrigger::FindActiveTrigger(ActiveTriggerType activeTriggerType, Entity* entity) :activeTriggerType(activeTriggerType), entity(entity) {}

FindActiveTrigger::FindActiveTrigger(ActiveTriggerType activeTriggerType, ENTITY_TYPE entityType) : activeTriggerType(activeTriggerType), entityType(entityType) {}

bool FindActiveTrigger::isSatisfied(iPoint tile) const
{
	bool isSatisfied = false;

	DynamicEntity* dynamicEntity = (DynamicEntity*)entity;

	switch (activeTriggerType) {

	case ActiveTriggerType_Goal:

		if (App->movement->IsValidTile(dynamicEntity->GetSingleUnit(), tile, isCheckingCurrTile, isCheckingNextTile, isCheckingGoalTile))
			isSatisfied = true;

		break;

	case ActiveTriggerType_Object:

		isSatisfied = true;

		break;

	case ActiveTriggerType_NoType:
	default:

		break;
	}

	return isSatisfied;
}