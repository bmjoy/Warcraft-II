#include "UIInputText.h"
#include "UIElement.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "UIButton.h"
#include "j1Input.h"

UIInputText::UIInputText(iPoint localPos, UIElement* parent, j1Module* listener) : UIElement (localPos, parent, listener){

	type = UIE_TYPE_INPUT_TEXT;
	original_pos = localPos;
	r = { 0,0,0,0 };

}

void UIInputText::Update(float dt) {
	
	if (InputText_Actived) {	
		App->render->DrawQuad({ original_pos.x + r.w ,original_pos.y,3,13 }, 255, 255, 255, 255);
		if (App->input->isPresed) {
			text += App->input->newLetter;
			App->font->CalcSize(text.data(), r.w, r.h);
			texture = App->font->Print(text.data());
			App->input->isPresed = false;
		}

		if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) {
			if (!text.empty()) {
				text.pop_back();
				App->font->CalcSize(text.data(), r.w, r.h);
				texture = App->font->Print(text.data());
			}
		}
		App->render->Blit(texture, original_pos.x, original_pos.y, &r);
	}



	if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN) {
		InputText_Actived = !InputText_Actived;
		if (InputText_Actived) {
			//labelInputText->SetText("");
			SDL_StartTextInput();

		}
		else if (!InputText_Actived) {
			//labelInputText->SetText("Your Name");
			SDL_StopTextInput();
		}
	}
}
