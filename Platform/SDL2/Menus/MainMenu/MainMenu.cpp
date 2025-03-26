#include "MainMenu.hpp"

mainMenuGUI::mainMenuGUI()
{

	playButton.destRect.w = 177;
	playButton.destRect.h = 61;
	//playButton.textureRegion;

	settingsButton.destRect.w = 370;
	settingsButton.destRect.h = 63;// x = 61
	settingsButton.destRect.y = playButton.destRect.h;
	//settingsButton.textureRegion;

	exitButton.destRect.w = 160;// x(61+63),160,61};
	exitButton.destRect.h = 61;
	exitButton.destRect.y = settingsButton.destRect.h + settingsButton.destRect.y;

	//exitButton.textureRegion;
	texs = { &playButton, &settingsButton, &exitButton };
}

void mainMenuGUI::init(Window& window) {
	background.load(window, "Asset/Sprites/MainMenu/background.png");
	settingsButton.load(window, "Asset/Sprites/MainMenu/settingsText.png");
	playButton.load(window, "Asset/Sprites/MainMenu/playText.png");
	exitButton.load(window, "Asset/Sprites/MainMenu/exitText.png");

	auto[w,h] = window.get_window_size();

	//change this
	background.destRect.w = w;
	background.destRect.h = h;
	//background.textureRegion;

	playButton.destRect.w = 177;
	playButton.destRect.h = 61;
	//playButton.textureRegion;

	settingsButton.destRect.w = 370;
	settingsButton.destRect.h = 63;//x = 61
	settingsButton.destRect.y = playButton.destRect.h;
	//settingsButton.textureRegion;

	exitButton.destRect.w = 160;//x(61+63),160,61};
	exitButton.destRect.h = 61;
	exitButton.destRect.y = settingsButton.destRect.h + settingsButton.destRect.y;

	//exitButton.textureRegion;
	texs = { &playButton, &settingsButton, &exitButton };
	highlighted = 0;
	isInitialized = true;
};


GUI_payload mainMenuGUI::update(const Shakkar::inputs& input) {
	// in this scenario the top option is 0, and you increment to go down

	Shakkar::Key menuDown = input.getKey(SDLK_DOWN);
	Shakkar::Key menuUp = input.getKey(SDLK_UP);
	Shakkar::Key menuSelect = input.getKey(SDLK_RETURN);
	Shakkar::Key menuExit = input.getKey(SDLK_ESCAPE);

	if (menuDown.pressed) {
		highlighted = (highlighted + 1) % numberOfOptions;
	}
	if (menuUp.pressed) { //prevent menu underflow
		if (highlighted == 0) {
			highlighted = numberOfOptions;
		}
		highlighted = (highlighted - 1) % numberOfOptions;
	}
	if (menuSelect.pressed) {
		switch ((GameState)highlighted)
		{
		case mainMenuGUI::Play:

			return { new PlayMenuGUI(), true };
			break;
		case mainMenuGUI::Settings:
			return{ new settingsMenuGUI(), true };
			break;
		case mainMenuGUI::Exit:
			return { nullptr, false };
			break;
		default:
			break;
		}
	}
	return { nullptr, true };
}

void mainMenuGUI::render(Window& window) {

	Uint8 highlighted_red = 255, highlighted_green = 255, highlighted_blue = 0;
	Uint8 default_red = 255, default_green = 255, default_blue = 255;
	switch ((GameState)highlighted)
	{
	case mainMenuGUI::Play:
		this->texs[Play]->setSurfaceColorMod(window,
			highlighted_red, highlighted_green, highlighted_blue);
		this->texs[Settings]->setSurfaceColorMod(window,
			default_red, default_green, default_blue);
		this->texs[Exit]->setSurfaceColorMod(window,
			default_red, default_green, default_blue);

		break;
	case mainMenuGUI::Settings:
		this->texs[Play]->setSurfaceColorMod(window,
			default_red, default_green, default_blue);
		this->texs[Settings]->setSurfaceColorMod(window,
			highlighted_red, highlighted_green, highlighted_blue);
		this->texs[Exit]->setSurfaceColorMod(window,
			default_red, default_green, default_blue);
		break;
	case mainMenuGUI::Exit:

		this->texs[Play]->setSurfaceColorMod(window,
			default_red, default_green, default_blue);
		this->texs[Settings]->setSurfaceColorMod(window,
			default_red, default_green, default_blue);
		this->texs[Exit]->setSurfaceColorMod(window,
			highlighted_red, highlighted_green, highlighted_blue);
		break;
	default:
		break;
	}

	window.clear();
	auto [win_w, win_h] = window.get_window_size();
	background.destRect = window.calculate_outer_rect({ 0,0,win_w, win_h }, (float)background.destRect.w / background.destRect.h);
	background.render(window);
	for (auto* e : texs)
		e->render(window);
}