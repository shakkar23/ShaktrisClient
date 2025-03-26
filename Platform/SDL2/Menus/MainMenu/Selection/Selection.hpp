#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Platform/SDL2/headers/TextureManager.hpp"
#include "Platform/SDL2/Menus/Menus.hpp"
#include "Platform/SDL2/Menus/MainMenu/MainMenu.hpp"
#include "ShakTris/Input/Input.hpp"
#include "Platform/SDL2/headers/Engine.hpp"
#include "ShakTris/Plugins/PluginHeaders/Tetris.hpp"
#include "ShakTris/Plugins/PluginHeaders/PluginRegistry.hpp"


class SelectionGUI : public GUI {


public:
	SelectionGUI();

	Shakkar::Tetris* plugin;
	~SelectionGUI();

	void init(Window& window) override;
	GUI_payload update(const Shakkar::inputs& input) override;
	void render(Window& window) override;

private:
	bool pluginReInit = false;
};
