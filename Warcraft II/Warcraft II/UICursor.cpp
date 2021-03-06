#include "UICursor.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Window.h"

UICursor::UICursor(iPoint localPos, UIElement* parent, UICursor_Info& info, j1Module* listener, bool isInWorld) : UIElement(localPos, parent, listener, isInWorld), cursor(info)
{
	type = UIE_TYPE_CURSOR;

	default = App->gui->GetRectFromAtlas(cursor.default);
	onClick = App->gui->GetRectFromAtlas(cursor.onClick);
	onMine = App->gui->GetRectFromAtlas(cursor.onMine);
	onMineClick = App->gui->GetRectFromAtlas(cursor.onMineClick);
	onEnemies = App->gui->GetRectFromAtlas(cursor.onEnemies);
	onEnemiesClick = App->gui->GetRectFromAtlas(cursor.onEnemiesClick);

	texArea = default;
	width = texArea.w;
	height = texArea.h;

	priority = PriorityDraw_CURSOR;
	SDL_ShowCursor(0);
}

UICursor::~UICursor()
{
	this->default = { 0,0,0,0 };
	onClick = { 0,0,0,0 };
	onMine = { 0,0,0,0 };
	onMineClick = { 0,0,0,0 };
	onEnemies = { 0,0,0,0 };
	onEnemiesClick = { 0,0,0,0 };

	UIevent = UI_EVENT_NONE;
}

void UICursor::Update(float dt)
{
	SetMousePosition();

	if (listener != nullptr)
		HandleInput();
}

void  UICursor::DrawAbove() const
{
	iPoint blitPos;
	int scale = App->win->GetScale();
	blitPos.x = (GetLocalPos().x - App->render->camera.x) / scale;
	blitPos.y = (GetLocalPos().y - App->render->camera.y) / scale;

	if (texArea.w != 0)
		App->render->Blit(App->gui->GetAtlas(), blitPos.x, blitPos.y, &texArea);
	else
		App->render->Blit(App->gui->GetAtlas(), blitPos.x, blitPos.y);

	if (App->gui->isDebug)
		DebugDraw(blitPos);
}

void UICursor::HandleInput()
{
	switch (UIevent)
	{
	case UI_EVENT_NONE:
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_PRESSED) {
			UIevent = UI_EVENT_MOUSE_LEFT_CLICK;
			texArea = onClick;
			listener->OnUIEvent(this, UIevent);
			break;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_PRESSED) {
			UIevent = UI_EVENT_MOUSE_RIGHT_CLICK;
			texArea = onClick;
			listener->OnUIEvent(this, UIevent);
			break;
		}
		break;
	case UI_EVENT_MOUSE_LEFT_CLICK:
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_RELEASED) {
			UIevent = UI_EVENT_NONE;
			texArea = default;
			listener->OnUIEvent(this, UIevent);
			break;
		}
		break;
	case UI_EVENT_MOUSE_RIGHT_CLICK:
		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_RELEASED) {
			UIevent = UI_EVENT_NONE;
			texArea = default;
			listener->OnUIEvent(this, UIevent);
			break;
		}
		break;
	}
}

//---------------------------------------------------------------

void UICursor::SetMousePosition()
{
	int scale = App->win->GetScale();
	iPoint mouse_position;
	App->input->GetMousePosition(mouse_position.x, mouse_position.y);
	mouse_position.x *= scale;
	mouse_position.y *= scale;
	SetLocalPos(mouse_position);
}

void UICursor::SetTexArea(SDL_Rect texAreaDefault, SDL_Rect texAreaOnClick)
{
	this->texArea = texAreaDefault;
	this->default = texAreaDefault;
	this->onClick = texAreaOnClick;
}

SDL_Rect UICursor::GetDefaultTexArea() const 
{
	return this->default;
}