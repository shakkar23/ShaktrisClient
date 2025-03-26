#pragma once


#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <exception>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "RenderWindow.hpp"



class Texture {
public:
	Texture() = default;
	Texture(Window& window, std::string_view path) :path(path) {
		auto surface = IMG_Load(path.data());
		if (surface == nullptr) [[unlikely]] {
			throw std::runtime_error("a surface didnt load");
		}

		width = surface->w;
		height = surface->h;

		this->texture = SDL_CreateTextureFromSurface(window.get_renderer(), surface);
		SDL_FreeSurface(surface);

	}
	Texture(const Texture& other) :
		texture(other.texture),
		path(other.path),
		width(other.width),
		height(other.height) {
		if (texture)
			SDL_DestroyTexture(texture);
		this->texture = other.texture;
	}
	~Texture() {
		if (texture)
			SDL_DestroyTexture(texture);
	}


	SDL_Texture* texture;
	std::string path;
	uint16_t width;
	uint16_t height;
};


class Renderable {
public:
	Renderable() :texture(nullptr) {}
	Renderable(Texture* tex) :texture(tex) {}
	virtual void load(Window& window, std::string path, int32_t tile_x, int32_t tile_y) = 0;
	virtual void render(Window& renderer) = 0;
	Texture* texture;
};


class Sprite : public Renderable {
public:
	Sprite() :Renderable(), srcRect({ 0,0,0,0 }), destRect({ 0,0,0,0 }) {};
	Sprite(Window& window, std::string_view path, Texture* tex) {

		// set class members
		this->texture = tex;
		this->srcRect = { 0, 0, tex->width, tex->height };
		this->destRect = { 0, 0, 0, 0 };
	}

	void load(Window& window, std::string path, int32_t x = 0, int32_t y = 0) override;
	void render(Window& renderer) override;

	SDL_Rect srcRect = { 0,0,0,0 };
	SDL_Rect destRect = { 0,0,0,0 };
};

// a sprite sheet is a collection of other smaller "sprites" that are represented by NxN N being "sections"
// you are freely allowed to manipulate where 
class SpriteSheet : public Renderable {
public:
	SpriteSheet() :Renderable(), srcRect({ 0,0,0,0 }), destRect({ 0,0,0,0 }), tile_x(0), tile_y(0) {};
	SpriteSheet(uint8_t tile_x, uint8_t tile_y, Texture* tex, std::string path) : Renderable(tex), tile_x(tile_x), tile_y(tile_y) {

		// set class members
		this->srcRect = { 0, 0, tile_x, tile_y };
		this->destRect = { 0, 0, tile_x, tile_y };
	}

	~SpriteSheet() {}

	// the x and y are 0,0 on the top left of the texture
	void load(Window& window, std::string path, int32_t tile_x, int32_t tile_y) override;
	void render(Window& window) override;
	void updateSection(uint8_t x, uint8_t y);
	SDL_Rect srcRect = { 0,0,0,0 };
	SDL_Rect destRect = { 0,0,0,0 };
	uint8_t tile_x = 0;
	uint8_t tile_y = 0;
};


class SurfaceSpriteSheet : public Renderable {
public:
	SurfaceSpriteSheet() :
		Renderable(),
		surface(nullptr),
		texture(nullptr),
		path(),
		srcRect({}),
		destRect({}),
		width(0), height(0),
		tile_x(0), tile_y(0) {};
	SurfaceSpriteSheet(Window& window, std::string_view path, uint16_t tile_x, uint16_t tile_y) {
		surface = IMG_Load(path.data());
		srcRect = { 0,0,tile_x,tile_y };
		destRect = {};
		width = surface->w;
		height = surface->h;
		this->tile_x = tile_x;
		this->tile_y = tile_y;
		this->texture = SDL_CreateTextureFromSurface(window.get_renderer(), surface);
		if (texture == NULL || surface == NULL) {
			std::string error = "setSurfaceColorMod failed because: ";
			error += SDL_GetError();
			throw std::runtime_error(error);
		}
		this->path = path;
	}
	SurfaceSpriteSheet(SurfaceSpriteSheet&& other) :
		path(std::move(other.path)),
		srcRect(other.srcRect),
		destRect(other.destRect),
		width(other.width),
		height(other.height),
		tile_x(other.tile_x),
		tile_y(other.tile_y) {
		if (surface)
			SDL_FreeSurface(this->surface);
		surface = other.surface;
		other.surface = nullptr;
		if (this->texture)
			SDL_DestroyTexture(this->texture);
		texture = other.texture;
		other.texture = nullptr;
	}
	SurfaceSpriteSheet(const SurfaceSpriteSheet& other) = delete;
	SurfaceSpriteSheet& operator= (const SurfaceSpriteSheet&) = delete;
	SurfaceSpriteSheet& operator=(SurfaceSpriteSheet&& other) {
		this->path = std::move(other.path);
		this->srcRect = other.srcRect;
		this->destRect = other.destRect;
		this->width = other.width;
		this->height = other.height;
		this->tile_x = other.tile_x;
		this->tile_y = other.tile_y;
		if (this->surface)
			SDL_FreeSurface(this->surface);
		this->surface = std::exchange(other.surface, nullptr);
		if (this->texture)
			SDL_DestroyTexture(this->texture);
		this->texture = std::exchange(other.texture, nullptr);
		return *this;
	}
	~SurfaceSpriteSheet() {
		if (surface)
			SDL_FreeSurface(this->surface);
		if (this->texture)
			SDL_DestroyTexture(this->texture);
	}

	inline void setSurfaceAlphaMod(Window& window, Uint8 a) {

		[[unlikely]] if (SDL_SetSurfaceAlphaMod(surface, a) != 0) {

			std::string error = "setSurfaceAlphaMod failed because: ";

			error += SDL_GetError();

			throw std::runtime_error(error);
		}
		createTexture(window);
	}

	inline void setSurfaceBlendMode(Window& window, SDL_BlendMode blend) {

		[[unlikely]] if (SDL_SetSurfaceBlendMode(surface, blend)) {

			std::string error = "SDL_SetSurfaceBlendMode failed because: ";

			error += SDL_GetError();

			throw std::runtime_error(error);
		}
		createTexture(window);
	}

	inline void setSurfaceColorMod(Window& window, Uint8 r, Uint8 g, Uint8 b) {

		[[unlikely]] if (SDL_SetSurfaceColorMod(surface, r, g, b) != 0) {
			std::string error = "setSurfaceColorMod failed because: ";

			error += SDL_GetError();

			throw std::runtime_error(error);

		}
		createTexture(window);
	}

	// call this after youve made modifications using the other utility functions to "save" the changes into the texture
	inline void createTexture(Window& window) {
		if (texture)
			SDL_DestroyTexture(texture);

		texture = SDL_CreateTextureFromSurface(window.get_renderer(), surface);
	}

	void load(Window& window, std::string path, int32_t x = 0, int32_t y = 0) override;
	void render(Window& renderer) override;
	void updateSection(uint16_t x, uint16_t y);

	SDL_Surface* surface;
	SDL_Texture* texture;
	std::string path;
	SDL_Rect srcRect;
	SDL_Rect destRect;
	uint16_t width;
	uint16_t height;
	uint16_t tile_x;
	uint16_t tile_y;
};

// surface textures are always going to render its entire self no matter what
class SurfaceTexture : public Renderable {
	// the rule of 5, 
	// if you define the destructor, move constructor, move assignment operator, copy constructor, or copy assignment operator, 
	// then you should define all of them, even if only to delete them
public:
	SurfaceTexture() :
		Renderable(),
		surface(nullptr),
		texture(nullptr),
		path(),
		destRect({}) {};
	SurfaceTexture(Window& window, int w, int h) {
		this->surface = SDL_CreateRGBSurface(0, w, h, 32,
			0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
		this->texture = nullptr;
		this->destRect = { 0, 0, w, h };

		window.set_draw_color(0, 0, 0, 255);
		this->drawRectFilled(window, destRect);
	}

	SurfaceTexture(Window& window, std::string_view path) {

		surface = IMG_Load(path.data());

		destRect = {};

		this->texture = SDL_CreateTextureFromSurface(window.get_renderer(), surface);
		if (texture == NULL || surface == NULL) {
			std::string error = "setSurfaceColorMod failed because: ";

			error += SDL_GetError();

			throw std::runtime_error(error);
		}

		this->path = path;
	}

	SurfaceTexture(SurfaceTexture&& other) :
		path(std::move(other.path)),
		destRect(other.destRect) {

		if (surface)
			SDL_FreeSurface(this->surface);

		surface = other.surface;
		other.surface = nullptr;

		if (this->texture)
			SDL_DestroyTexture(this->texture);

		texture = other.texture;
		other.texture = nullptr;

	}

	SurfaceTexture(const SurfaceTexture& other) = delete;

	SurfaceTexture& operator= (const SurfaceTexture&) = delete;

	SurfaceTexture& operator=(SurfaceTexture&& other) {
		this->path = std::move(other.path);
		this->destRect = other.destRect;

		if (this->surface)
			SDL_FreeSurface(this->surface);

		this->surface = std::exchange(other.surface, nullptr);

		if (this->texture)
			SDL_DestroyTexture(this->texture);

		this->texture = std::exchange(other.texture, nullptr);
		return *this;
	}

	~SurfaceTexture() {
		if (surface)
			SDL_FreeSurface(this->surface);

		if (this->texture)
			SDL_DestroyTexture(this->texture);
	}
	void load(Window& window, std::string path, int32_t x = 0, int32_t y = 0) override;
	void createSurface(Window& window, int32_t x, int32_t y);

	// util
	void drawRectFilled(Window& window, SDL_Rect dest);
	void blitSurface(Window& window, SDL_Surface* src, SDL_Rect dest);
	void blitSpriteSurface(Window& window, SurfaceSpriteSheet& src, SDL_Rect dest);
	void render(Window& renderer) override;

	// call this after youve made modifications using the other utility functions to "save" the changes into the texture
	inline void createTexture(Window& window) {
		if (texture)
			SDL_DestroyTexture(texture);

		texture = SDL_CreateTextureFromSurface(window.get_renderer(), surface);
	}

	inline void setSurfaceAlphaMod(Window& window, Uint8 a) {

		[[unlikely]] if (SDL_SetSurfaceAlphaMod(surface, a) != 0) {

			std::string error = "setSurfaceAlphaMod failed because: ";

			error += SDL_GetError();

			throw std::runtime_error(error);
		}
		createTexture(window);
	}

	inline void setSurfaceBlendMode(Window& window, SDL_BlendMode blend) {

		[[unlikely]] if (SDL_SetSurfaceBlendMode(surface, blend)) {

			std::string error = "SDL_SetSurfaceBlendMode failed because: ";

			error += SDL_GetError();

			throw error;
		}
		createTexture(window);
	}

	inline void setSurfaceColorMod(Window& window, Uint8 r, Uint8 g, Uint8 b) {

		[[unlikely]] if (SDL_SetSurfaceColorMod(surface, r, g, b) != 0) {
			std::string error = "setSurfaceColorMod failed because: ";

			error += SDL_GetError();

			throw std::runtime_error(error);

		}
		createTexture(window);
	}

	SDL_Surface* surface;
	SDL_Texture* texture;
	std::string path;
	SDL_Rect destRect;
};



class TextureDictionary {
public:
	static Sprite		  reloadSP(Window& window, std::string_view path);
	static Sprite		  getSprite(Window& window, std::string_view path);
	static SurfaceTexture getSurfaceTexture(Window& window, std::string_view path);
	static SurfaceTexture reloadST(Window& window, std::string_view path);
	static SpriteSheet	  getSpriteSheet(Window& window, std::string_view path, uint32_t tile_x, uint32_t tile_y);
	static SpriteSheet    reloadSS(Window& window, std::string_view path, uint32_t tile_x, uint32_t tile_y);
	static SurfaceSpriteSheet getSurfaceSpriteSheet(Window& window, std::string_view path, uint8_t tile_x, uint8_t tile_y);
	static SurfaceSpriteSheet reloadSSS(Window& window, std::string_view path, uint8_t tile_x, uint8_t tile_y);

private:
	static Texture* loadTexture(Window& window, std::string_view p_filePath);
	static inline std::map<std::string, Texture*> textures;
};