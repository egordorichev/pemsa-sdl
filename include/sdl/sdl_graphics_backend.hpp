#ifndef SDL_GRAPHICS_BACKEND_HPP
#define SDL_GRAPHICS_BACKEND_HPP

#include "pemsa/graphics/pemsa_graphics_backend.hpp"

#ifdef WINDOWS
#include "SDL.h"
#else
#include "SDL2/SDL.h"
#endif

class SdlGraphicsBackend : public PemsaGraphicsBackend {
	public:
		SdlGraphicsBackend(SDL_Window* window);
		~SdlGraphicsBackend();

		void createSurface() override;
		void flip() override;

		SDL_Surface* getSurface();

		float getScale();
		int getOffsetX();
		int getOffsetY();

		void handleEvent(SDL_Event* event);
		void render() override;

		void setFps(int fps);
		int getFps() override;

		void toggleIntScales();
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Surface* surface;
		SDL_Surface* systemSurface;

		bool intScales;
		float scale;
		int offsetX;
		int offsetY;
		int fps;

		void resize();
		void actualRender(bool system);
		void doFlip(bool transparent);
};

#endif