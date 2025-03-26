#pragma once

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Settings/Settings.hpp"
#include "Play/Play.hpp"
#include "../Menus.hpp"
#include "../../../../ShakTris/Input/Input.hpp"
#include "../headers/TextureManager.hpp"

class menuGUI;

class mainMenuGUI : public GUI
{
public:
	mainMenuGUI();
	~mainMenuGUI() = default;

	Sprite background; 
	SurfaceTexture playButton;
	SurfaceTexture settingsButton; 
	SurfaceTexture exitButton;

	void init(Window &window) override;
	GUI_payload update(const Shakkar::inputs &input) override;
	void render(Window &window) override;
	
private:
	uint_fast8_t highlighted{};
	bool isEntered = false;
	enum GameState {
		Play,
		Settings,
		Exit,
		numberOfOptions
	};
	std::vector<SurfaceTexture*> texs;
};

