#include "gameManager.hpp"

gameManager::gameManager(Window& window) {
    PluginManager::loadPlugins(window);
    subGUIs.push_back(new mainMenuGUI());
}

gameManager::~gameManager(){
    unloadPlugins();
}


bool gameManager::update(const Shakkar::inputs& input) {
    // get last element
    if (subGUIs.size() == 0)
		return false;
    GUI* last = subGUIs.back();

    GUI_payload payload = last->update(input);

    if (!payload.second)
    {
        auto iter = std::find(subGUIs.begin(), subGUIs.end(), payload.first);
        // if we dont find it, then we are deleting the last menu

        if (iter == subGUIs.end())
        {
			subGUIs.pop_back();
			delete last;
		}

    }
    else if (payload.first != nullptr)
    {
		subGUIs.push_back(payload.first);
	}
    return true;
}
void gameManager::render(Window &window) {
    if (!subGUIs.back()->isInitialized) {
        subGUIs.back()->init(window);
    }
    subGUIs.back()->render(window);
    window.display();
}
