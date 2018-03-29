// ----------------------------------------------------
// j1Module.h
// Interface for all engine modules
// ----------------------------------------------------

#ifndef __j1MODULE_H__
#define __j1MODULE_H__

#include <string>

#include "PugiXml\src\pugixml.hpp"

using namespace std;

class j1App;
struct Collider;

class UIElement;
enum UI_EVENT;

class StaticEntity;
class DynamicEntity;
enum EntitiesEvent;

class j1Module
{
public:

	j1Module() : active(false)
	{}

	void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&) const
	{
		return true;
	}

	// Collision
	virtual void OnCollision(Collider*, Collider*) {}

	virtual void OnUIEvent(UIElement* UIelem, UI_EVENT UIevent) {}

	virtual void OnStaticEntitiesEvent(StaticEntity* staticEntity, EntitiesEvent entitiesEvent) {}
	virtual void OnDynamicEntitiesEvent(DynamicEntity* dinamicEntity, EntitiesEvent entitiesEvent) {}

	// Load keys
	virtual bool LoadKeys(pugi::xml_node& buttons)
	{
		return true;
	}

public:

	string		name;
	bool		active = true;

};

#endif //__j1MODULE_H__
