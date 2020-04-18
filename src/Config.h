#pragma once
#ifndef __CONFIG__
#define __CONFIG__
#include <SDL_ttf.h>

class Config {
public:
	static const int SCREEN_WIDTH = 1024;
	static const int SCREEN_HEIGHT = 768;
	static const int ROW_NUM = 24;
	static const int COL_NUM = 32;
	static const int TILE_SIZE = 32;
	static const int TILE_COST = 1;
	static const int WALL_NUM = 30;
	static const int MOVE_SPEED = 5;

	// Define Custom Deleters for shared_ptr types
	static void SDL_DelRes(SDL_Window* r) { SDL_DestroyWindow(r); }
	static void SDL_DelRes(SDL_Renderer* r) { SDL_DestroyRenderer(r); }
	static void SDL_DelRes(SDL_Texture* r) { SDL_DestroyTexture(r); }
	static void SDL_DelRes(SDL_Surface* r) { SDL_FreeSurface(r); }
	static void SDL_DelRes(TTF_Font* r) { TTF_CloseFont(r); }

	// template function to create and return shared_ptr instance
	template <typename T>
	static std::shared_ptr<T> make_resource(T* t) {
		return std::shared_ptr<T>(t, [](T* t) { Config::SDL_DelRes(t); });
	}

};

#endif /* defined (__CONFIG__) */