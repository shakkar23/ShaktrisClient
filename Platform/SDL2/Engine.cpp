#include "Engine.hpp"

#include <iostream>
#include <utility>


class SDL_init {
public:
	SDL_init() {

		if (SDL_Init(SDL_INIT_AUDIO) < 0)
			std::cerr << "HEY.. Audio_Init HAS FAILED. SDL_ERROR: " << SDL_GetError()
			<< std::endl;

		if (SDL_Init(SDL_INIT_VIDEO) < 0)
			std::cerr << "HEY.. SDL_Init HAS FAILED. SDL_ERROR: " << SDL_GetError()
			<< std::endl;

		if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		{
			std::cerr << "HEY.. Mix_OpenAudio HAS FAILED. SDL_ERROR: " << SDL_GetError() << std::endl;
			std::cerr << "Maybe install SDL2_mixer[mpg123]:arch-OS or just find a way to link mpg123" << std::endl;
		}

		if(Mix_Init(MIX_INIT_MP3) == 0)
			std::cerr << "HEY.. Mix_Init HAS FAILED. SDL_ERROR: " << SDL_GetError()
			<< std::endl;

		if (TTF_Init() == -1)
			std::cerr << "HEY.. ttf_Init HAS FAILED. SDL_ERROR: " << SDL_GetError()
			<< std::endl;

		if (IMG_Init(IMG_INIT_PNG) == 0)
			std::cerr << "IMG_init has failed. Error: " << SDL_GetError() << std::endl;
	}
	~SDL_init() {
		IMG_Quit();
		TTF_Quit();
		Mix_CloseAudio();
		Mix_Quit();
		SDL_Quit();
	}
};

void ProcessInputs(SDL_Event& event, bool& shouldDisplay, bool& windowSizedChanged, Shakkar::inputs& input, bool& gameRunning);

// c callable function to render while the window is being resized
static int render_game(void* userData, SDL_Event* event) {
	auto[game_manager, window] = *(std::pair<gameManager&, Window&>*)userData;

	if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_EXPOSED) {
		game_manager.render(window);
	}
	return 0;
}

int main(int argc, char* args[]) {
	SDL_init sdl;

	Window window("Shaktris", 960, 960);

	gameManager game_manager(window);

	Shakkar::inputs input;

	bool gameRunning = true;
	SDL_Event event;

	// rendering constantly, and not actually displaying causes memory to stack higher and higher until the frames can be shown
	bool shouldDisplay = false;
	bool windowSizedChanged = false;

	double alpha = 0.0;
	Uint64 last_time = SDL_GetPerformanceCounter();
	Uint64 ticks = 0;

	// allow rendering while screen is being resized
	const std::pair<gameManager&, Window&> userData = { game_manager, window };
	SDL_AddEventWatch(render_game, (void*) &userData);

	while (gameRunning) {
		

		ProcessInputs(event, shouldDisplay, windowSizedChanged, input, gameRunning);

		// skip frames that cant be shown due to window not currently accepting frames to display
		if (!shouldDisplay) {

			const auto now = SDL_GetPerformanceCounter();
			alpha += (double)((double)(now - last_time) / SDL_GetPerformanceFrequency() * UPDATES_A_SECOND);
			last_time = now;

			while (alpha > 1.0) {
				if (!game_manager.update(input)) {
					gameRunning = false;
					break;
				}
				input.update();

				alpha -= 1.0;
			}

			if (gameRunning) {
				game_manager.render(window);
			}
		}
	}

	return 0;
}

void ProcessInputs(SDL_Event& event, bool& shouldDisplay, bool& windowSizedChanged, Shakkar::inputs& input, bool& gameRunning)
{
	while (SDL_PollEvent(&event)) {

		if (event.type == SDL_WINDOWEVENT) {
			if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
				shouldDisplay = false;
			}
			else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
				shouldDisplay = false;
			}
			else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				windowSizedChanged = true;
			}
		}
		else if ((event.key.type == SDL_KEYDOWN) || (event.key.type == SDL_KEYUP)) {
			if (event.key.state == SDL_PRESSED) {
				if (event.key.repeat)
					continue;
				input.addKey(event.key.keysym.sym);
				//std::cout << "Physical" << SDL_GetScancodeName(event.key.keysym.scancode) << " key acting as the " << SDL_GetKeyName(event.key.keysym.sym) << " key" << std::endl;
			}
			else {
				input.removeKey(event.key.keysym.sym);
			}
		}
		// mouse events
		else if (event.type == SDL_MOUSEBUTTONDOWN) {
			input.updateMouseButtons(event.button.button, true);
		}
		else if (event.type == SDL_MOUSEBUTTONUP) {
			input.updateMouseButtons(event.button.button, false);
		}
		else if (event.type == SDL_MOUSEMOTION) {
			input.updateMousePos(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
		}
		else if (event.type == SDL_MOUSEWHEEL) {
			input.updateMouseWheel(event.wheel.x, event.wheel.y, event.wheel.direction);
		}
		else if (event.type == SDL_QUIT) // X button on window, or something else that closes the window from the OS
			gameRunning = false;

	}
}
