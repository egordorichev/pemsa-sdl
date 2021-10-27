#ifndef SDL_INPUT_BACKEND_HPP
#define SDL_INPUT_BACKEND_HPP

#include "pemsa/input/pemsa_input_backend.hpp"
#include "pemsa/input/pemsa_input_module.hpp"

#ifdef WINDOWS
#include "SDL.h"
#else
#include "SDL2/SDL.h"
#endif

#include <vector>

class SdlInputBackend : public PemsaInputBackend {
	public:
		SdlInputBackend();
		~SdlInputBackend();

		void handleEvent(SDL_Event* event);
		bool isButtonDown(int i, int p) override;
		bool isButtonPressed(int i, int p) override;
		void update() override;

		int getMouseX() override;
		int getMouseY() override;
		int getMouseMask() override;

		const char* readKey() override;
		bool hasKey() override;
		void reset() override;

		const char* getClipboardText() override;
	private:
		uint8_t state[PEMSA_PLAYER_COUNT][PEMSA_BUTTON_COUNT];
		uint8_t mouseState;

		std::vector<SDL_GameController*> controllers;
		SDL_Scancode lastKey;
		bool isDown;
};

#endif