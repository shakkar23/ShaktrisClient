#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../../../../../ShakTris/Input/Input.hpp"
#include "../../../headers/TextureManager.hpp"
#include "../../../headers/Engine.hpp"
#include "../../Menus.hpp"
#include "../MainMenu.hpp"

class settingsMenuGUI : public GUI {


public:
	settingsMenuGUI() = default;
	~settingsMenuGUI() = default;

	SurfaceTexture background;
	SurfaceTexture playButton;
	SurfaceTexture settingsButton;
	SurfaceTexture exitButton;

	void init(Window& window) override;
	GUI_payload update(const Shakkar::inputs& input) override;
	void render(Window& window) override;
	
private:
	std::vector<SurfaceTexture*>texs;

	const int numberOfOptions = 3;
	uint_fast8_t highlighted{};
	bool isEntered = false;

	enum GameState {
		Play,
		Settings,
		Exit
	};

};
