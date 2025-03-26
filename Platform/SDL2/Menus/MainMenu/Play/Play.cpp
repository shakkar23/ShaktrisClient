#include "Play.hpp"

#include "../../inipp/inipp/inipp.h"
#include <fstream>

PlayMenuGUI::PlayMenuGUI() {
	this->font = TTF_OpenFont("Asset/Sprites/PlayMenu/font.ttf", 50);
	isInitialized = false;
	plugin = nullptr;

	inipp::Ini<char> ini;
	std::ifstream is("config.ini");
	ini.parse(is);

	auto worked = inipp::get_value(ini.sections["Controls"], "das_setting", das);
	if (!worked) {
		ini.sections["Controls"]["das_setting"] = "100";
		das = 100;
	}
	auto worked2 = inipp::get_value(ini.sections["Controls"], "arr_setting", arr);
	if (!worked2) {
		ini.sections["Controls"]["arr_setting"] = "0";
		arr = 0;
	}
	if (!worked || !worked2) {
		std::ofstream os("config.ini");
		ini.generate(os);
	}
}


PlayMenuGUI::~PlayMenuGUI() {
	if (font != nullptr) {
		TTF_CloseFont(font);
	}
}


//grabs a plugin from the entries that are registered
void PlayMenuGUI::init(Window& window) {
	auto [w, h] = window.get_window_size();
	this->backGround.load(window, "Asset/Sprites/PlayMenu/black169.png");
	backGround.destRect = { 0,0,w, h };

	pluginReInit = false;
	isInitialized = true;
}


GUI_payload PlayMenuGUI::update(const Shakkar::inputs& input) {
	
	Shakkar::Key menuDown = input.getKey(SDLK_DOWN);
	Shakkar::Key menuUp = input.getKey(SDLK_UP);
	Shakkar::Key menuSelect = input.getKey(SDLK_RETURN);
	Shakkar::Key menuExit = input.getKey(SDLK_ESCAPE);

	if (this->plugin == nullptr) {
		if (menuDown.pressed) {
			selectedGame = (selectedGame + 1) % Shakkar::Plugins::getEntries().size();
		}
		if (menuUp.pressed) { //prevent menu underflow
			if (selectedGame == -1) {
				selectedGame = Shakkar::Plugins::getEntries().size() - 1;
			}
			selectedGame = (selectedGame - 1) % Shakkar::Plugins::getEntries().size();
		}
		
		if (menuSelect.pressed) {
			plugin = Shakkar::Plugins::getEntries()[selectedGame];
			plugin->updateSettings(das, arr);
			pluginReInit = true;
		}
		if (menuExit.pressed) {
			return { nullptr, false };
		}
	}
	else [[unlikely]] if (!plugin->isPlaying) {

		plugin->isPlaying = true;
		plugin = nullptr; 
	}
	else {
		plugin->gameLogic(input);
	}
	return { nullptr, true };
}

void PlayMenuGUI::render(Window& window) {
	[[unlikely]] if (pluginReInit) {
		plugin->Init(window);
		pluginReInit = false;
	}
	else if (plugin)
		plugin->render(window);
	else {

		backGround.render(window);

		SDL_Color color{};
		color.r = 0xff; color.g = 0; color.b = 0;
		SDL_Rect textPos = { 0, 0, 0, 0 };

		// get a reference to all the games
		auto& games = Shakkar::Plugins::getEntries();

		//iterate through all the games
		for (auto i = 0; i < games.size(); i++) {

			// get the name for the game
			if(i == selectedGame) { color.r = 0xff; color.g = 0; color.b = 0; } else { color.r = 0xff; color.g = 0xff; color.b = 0xff; };
			SDL_Surface* TextSurface = TTF_RenderText_Solid(font, games[i]->m_unlocalizedTetrisName.c_str(), color);
			SDL_Texture* text = window.create_texture_from_surface(TextSurface);

			//set the position of the game to be below the last game, and a bit to the right of the left side of the screen
			textPos.y += textPos.h + 10;
			textPos.w = TextSurface->w;
			textPos.h = TextSurface->h;
			textPos.x = 10;

			

			// render and clean up
			window.render_copy(text, NULL, &textPos);
			SDL_FreeSurface(TextSurface);
			SDL_DestroyTexture(text);
		}
	}

}