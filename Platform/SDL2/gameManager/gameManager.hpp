#pragma once

#include "../../ShakTris/Plugins/PluginManager.hpp"
#include "../Menus/MainMenu/MainMenu.hpp" 
#include "../headers/RenderWindow.hpp"
#include "../Menus/Menus.hpp"

// this is the gameManager, it redirects to wherever it needs to go
class gameManager
{
public:
	bool update(const Shakkar::inputs& input);
	void render(Window &window);

	gameManager(Window& window);
	~gameManager();
private:
	enum class GameState : uint_fast8_t
	{
		MainMenu,
		DefaultTetris,
		Settings,
		Exit
	};
	GameState GameState = gameManager::GameState::MainMenu;
	std::vector<GUI*> subGUIs;
};

