//not my code
#include "TextureManager.hpp"
#include <string_view>


void SurfaceTexture::load(Window& window, std::string path, int32_t x, int32_t y) {
	(*this) = TextureDictionary::getSurfaceTexture(window, path);
}

void SurfaceTexture::createSurface(Window& window, int32_t x, int32_t y) {
	// destroy the old surface if it exists
	if (this->surface) {
		SDL_FreeSurface(this->surface);
		this->surface = nullptr;
	}
	this->surface = SDL_CreateRGBSurface(0, x, y, 32, 0, 0, 0, 0);
	this->destRect = { 0,0,x,y };
	// the texture likely is changing every frame so only create it when rendering
	this->texture = nullptr;
}

void SurfaceTexture::drawRectFilled(Window& window, SDL_Rect dest) {
	// get the draw color from the window
	Uint8 r, g, b, a;
	window.get_draw_color(r, g, b, a);

	SDL_FillRect(this->surface, &dest, SDL_MapRGBA(this->surface->format, r, g, b, a));
}

void SurfaceTexture::blitSurface(Window& window, SDL_Surface* src, SDL_Rect dest) {
	SDL_Rect src_rect = { 0,0,src->w,src->h };
	SDL_BlitScaled(src, &src_rect, this->surface, &dest);
}


void SurfaceTexture::blitSpriteSurface(Window& window, SurfaceSpriteSheet& src, SDL_Rect dest) {
	int err = SDL_BlitSurface(src.surface, &src.srcRect, this->surface, &dest);

	if (err < 0) {
		SDL_Log("SDL_BlitSurface failed: %s\n", SDL_GetError());
	}

}

void SurfaceTexture::render(Window& window) {

#ifdef WIN32
	[[unlikely]]
#endif
	// this should never fail on PC
	if (window.render({ 0,0,0,0 }, this->destRect, this->texture))
		return;

	if (this->surface) {
		this->createTexture(window);
		if (window.render({ 0,0,0,0 }, this->destRect, this->texture))
			return;
	}

	// if all else fails, reload the texture from the path
	(*this) = TextureDictionary::reloadST(window, this->path);
	window.render({ 0,0,0,0 }, this->destRect, this->texture);
}


void Sprite::load(Window& window, std::string path, [[maybe_unused]] int32_t x, [[maybe_unused]] int32_t y) {
	(*this) = TextureDictionary::getSprite(window, path);
}

void Sprite::render(Window& window) {
	// this reloading is only neccesary when on android as when you rotate the screen the texture is for some reason not tied to the renderer anymore
#ifdef WIN32
	[[unlikely]]
#endif
	if (window.render(this->srcRect, this->destRect, this->texture->texture))
		return;

	(*this) = TextureDictionary::reloadSP(window, this->texture->path);
	window.render(this->srcRect, this->destRect, this->texture->texture);

}



void SpriteSheet::load(Window& window, std::string path, int32_t x, int32_t y) {

	(*this) = TextureDictionary::getSpriteSheet(window, path, x, y);
}

void SpriteSheet::render(Window& window) {
	// this reloading is only neccesary when on android as when you rotate the screen the texture is for some reason not tied to the renderer anymore
#ifdef WIN32
	[[unlikely]]
#endif
	if (window.render(this->srcRect, this->destRect, this->texture->texture))
		return;

	(*this) = TextureDictionary::reloadSS(window, this->texture->path, tile_x, tile_y);
	window.render(this->srcRect, this->destRect, this->texture->texture);
}

void SpriteSheet::updateSection(uint8_t x, uint8_t y) {
	this->srcRect = { x * tile_x, y * tile_y, tile_x, tile_y };
}

Texture* TextureDictionary::loadTexture(Window& window, std::string_view p_filePath) {
	return new Texture(window, p_filePath);
}




SurfaceTexture TextureDictionary::getSurfaceTexture(Window& window, std::string_view path) {
	// no search for if its in the dictionary, as surface textures are user independent
	return SurfaceTexture(window, path.data());
}

SurfaceTexture TextureDictionary::reloadST(Window& window, std::string_view path) {
	return getSurfaceTexture(window, path);
}




SpriteSheet TextureDictionary::getSpriteSheet(Window& window, std::string_view path, uint32_t tile_x, uint32_t tile_y) {

	if (textures.find(path.data()) == textures.end()) {
		// element not found 
		// this is the first time we encounter this path, generate a new Texture*
		textures[path.data()] = loadTexture(window, path.data());
	}
	return SpriteSheet(tile_x, tile_y, textures[path.data()], path.data());
}

SpriteSheet TextureDictionary::reloadSS(Window& window, std::string_view path, uint32_t tile_x, uint32_t tile_y) {
	(*textures.at(path.data())) = Texture(window, path);
	return getSpriteSheet(window, path, tile_x, tile_y);
}

SurfaceSpriteSheet TextureDictionary::getSurfaceSpriteSheet(Window& window, std::string_view path, uint8_t tile_x, uint8_t tile_y) {
	return SurfaceSpriteSheet(window, path.data(), tile_x, tile_y);
}

SurfaceSpriteSheet TextureDictionary::reloadSSS(Window& window, std::string_view path, uint8_t tile_x, uint8_t tile_y) {
	return getSurfaceSpriteSheet(window, path, tile_x, tile_y);
}


Sprite TextureDictionary::getSprite(Window& window, std::string_view path) {

	if (textures.find(path.data()) == textures.end()) {
		// element not found 
		// this is the first time we encounter this path, generate a new Texture*
		textures[path.data()] = loadTexture(window, path.data());
	}
	return Sprite(window, path.data(), textures[path.data()]);
}

Sprite TextureDictionary::reloadSP(Window& window, std::string_view path) {
	textures.erase(path.data());
	(*textures.at(path.data())) = Texture(window, path);
	return getSprite(window, path);
}

void SurfaceSpriteSheet::load(Window& window, std::string path, int32_t x, int32_t y) {
	(*this) = TextureDictionary::getSurfaceSpriteSheet(window, path, x, y);
}

void SurfaceSpriteSheet::render(Window& renderer) {

	// this reloading is only neccesary when on android as when you rotate the screen the texture is for some reason not tied to the renderer anymore
#ifdef WIN32
	[[unlikely]]
#endif
	if (renderer.render(this->srcRect, this->destRect, this->texture))
		return;
	(*this) = TextureDictionary::reloadSSS(renderer, this->path, tile_x, tile_y);
	renderer.render(this->srcRect, this->destRect, this->texture);
}

void SurfaceSpriteSheet::updateSection(uint16_t x, uint16_t y) {
	this->srcRect = { x * tile_x, y * tile_y, tile_x, tile_y };
}