
#include "test.hpp"
#include "../../../../Platform/SDL2/Audio/Audio.hpp"
#include <string>
#include <cstdlib>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

demo::demo() : Shakkar::Tetris() {
    isPlaying = true;
}

void demo::updateSettings(uint32_t das, uint32_t arr) {
}
//override
void demo::Init(Window& window) {

    //handling the fact that this is also simultaneously the ReInit() function, TODO: change later for it to be a stand alone function
    isPlaying = true;
    std::tie(windowWidth, windowHeight) = window.get_window_size();
}

//override
void demo::gameLogic(const Shakkar::inputs& input) {
    auto mouse = input.getMouse();
    mouseX = mouse.x;
    mouseY = mouse.y;
}

//override
void demo::render(Window& window) {

    std::tie(windowWidth, windowHeight) = window.get_window_size();
    window.clear();

    Uint8 r, g, b, a;
    window.get_draw_color(r, g, b, a);
    
    window.set_draw_color(255, 0, 0, 255);
    
    window.draw_rect_outline({ mouseX, mouseY, 20, 20});
    
    window.set_draw_color(r,g,b,a);
}


////         name of plugin     author           description?
PLUGIN_SETUP("test",      "Shakkar23",  "test") {
    Shakkar::Plugins::addorig<demo>();
}