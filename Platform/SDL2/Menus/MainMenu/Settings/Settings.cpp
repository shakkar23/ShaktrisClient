#include "Settings.hpp"


void settingsMenuGUI::init(Window& window) {
	if (!isInitialized) {
		background.load(window, "Asset/Sprites/invertedshak.png");
		settingsButton.load(window, "Asset/Sprites/MainMenu/settingsText.png");
		playButton.load (window, "Asset/Sprites/MainMenu/playText.png");
		exitButton.load(window, "Asset/Sprites/MainMenu/exitText.png");

		//change this
		auto[w, h] = window.get_window_size();
		background.destRect = { 0, 0, w, h};
		//background.textureRegion;

		playButton.destRect = { 0,0,177,61 };
		//playButton.textureRegion;

		settingsButton.destRect = { 0,61,370,63 };
		//settingsButton.textureRegion;

		exitButton.destRect = { 0,(61 + 63),160,61 };
		//exitButton.textureRegion;
		texs = { &background, &playButton, &settingsButton, &exitButton };
		highlighted = 0;
		isInitialized = true;
	}
	return;
}

GUI_payload settingsMenuGUI::update(const Shakkar::inputs& input) {
	// in this scenario the top option is 0, and you increment to go down
	Shakkar::Key menuDown = input.getKey(SDLK_DOWN);
	Shakkar::Key menuUp = input.getKey(SDLK_UP);
	Shakkar::Key menuSelect = input.getKey(SDLK_RETURN);

	if (menuDown.pressed) {
		highlighted = (highlighted + 1) % numberOfOptions;
	}
	if (menuUp.pressed) { //prevent menu underflow
		if (highlighted == 0) {
			highlighted = numberOfOptions;
		}
		highlighted = (highlighted - 1) % numberOfOptions;
	}
	if (menuSelect.pressed) {
		switch ((GameState)highlighted)
		{
		case settingsMenuGUI::Play:
			return{ new PlayMenuGUI() , true};
			break;
		case settingsMenuGUI::Settings:
			return{ new mainMenuGUI(), true };

			break;
		case settingsMenuGUI::Exit:
			return { nullptr, false };
			break;
		default:
			break;
		}
	}
	return { nullptr, true };
}

void settingsMenuGUI::render(Window& window) {
	Uint8 r = 0, g = 0, b = 255;
	Uint8 rDud = 255, gDud = 255, bDud = 255;
	switch ((GameState)highlighted)
	{
	case settingsMenuGUI::Play:
		SDL_SetTextureColorMod(&(*this->texs[(Play + 1)]->texture),
			r, g, b);
		SDL_SetTextureColorMod(&(*this->texs[(Settings + 1)]->texture),
			rDud, gDud, bDud);
		SDL_SetTextureColorMod(&(*this->texs[(Exit + 1)]->texture),
			rDud, gDud, bDud);

		break;
	case settingsMenuGUI::Settings:

		SDL_SetTextureColorMod(&(*this->texs[(Play + 1)]->texture),
			rDud, gDud, bDud);
		SDL_SetTextureColorMod(&(*this->texs[(Settings + 1)]->texture),
			r, g, b);
		SDL_SetTextureColorMod(&(*this->texs[(Exit + 1)]->texture),
			rDud, gDud, bDud);
		break;
	case settingsMenuGUI::Exit:

		SDL_SetTextureColorMod(&(*this->texs[(Play + 1)]->texture),
			rDud, gDud, bDud);
		SDL_SetTextureColorMod(&(*this->texs[(Settings + 1)]->texture),
			rDud, gDud, bDud);
		SDL_SetTextureColorMod(&(*this->texs[(Exit + 1)]->texture),
			r, g, b);
		break;
	default:
		break;
	}
	for (auto* e : texs) {
		e->render(window);
	}
}