#pragma once

#include <array>
#include <string>
#include <stdint.h>

#include "../../../Platform/SDL2/headers/RenderWindow.hpp"
#include "../../Input/Input.hpp"

class PlayMenuGUI;
class PluginManager;

namespace Shakkar {

    class Tetris {
        public:
            explicit Tetris(){}
            virtual void gameLogic(const Shakkar::inputs& input) = 0;
            virtual void updateSettings(uint32_t das, uint32_t arr) = 0;
            virtual void render(Window& window) = 0;
            //this needs to reload the game when called after the first time called
            // TODO: change later for reloading to be a stand alone function
            virtual void Init(Window& window) = 0;
            bool isPlaying = false;
        private:
        //the Play menu that will look at the names and display them so they need to have access to these names0
        friend PlayMenuGUI;
        friend PluginManager;
        std::string m_unlocalizedTetrisName;
        std::string m_unlocalizedTetrisDesc;
        std::string m_unlocalizedTetrisAuth;
    };
};
