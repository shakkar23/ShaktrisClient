
#include "example.hpp"
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

demo::demo() : Shakkar::Tetris() {
    
    isPlaying = true;
}

void demo::updateSettings(uint32_t das, uint32_t arr) {
    game.updateSettings(das, arr);
}
//override
void demo::Init(Window& window) {
    game.Init(window);

    //handling the fact that this is also simultaneously the ReInit() function, TODO: change later for it to be a stand alone function
    if (this->alreadyInitialized) {
        game.reload();
        isPlaying = true;
    }

    this->alreadyInitialized = true;

}

//override
void demo::gameLogic(const Shakkar::inputs& input) {
    Shakkar::Key menuSelect = input.getKey(SDLK_RETURN);
    if (menuSelect.pressed)
        isPlaying = false;

    game.gameLogic(input);
}

//override
void demo::render(Window& window)  {
    
    game.render(window);
}


////         name of plugin     author           description?
PLUGIN_SETUP("Tetris_exmaple",  "Shakkar23",     "example plugin lets GOOOO") {
    Shakkar::Plugins::addorig<demo>();
    Shakkar::Plugins::getEntries().back()->Init((*(Window*)window));
}