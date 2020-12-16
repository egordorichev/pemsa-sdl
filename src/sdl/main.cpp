#include "sdl/sdl_graphics_backend.hpp"
#include "sdl/sdl_audio_backend.hpp"
#include "sdl/sdl_input_backend.hpp"

#include "pemsa/pemsa.hpp"
#include "SDL2/SDL.h"

#include <filesystem>
#include <iostream>

int main(int argc, const char** argv) {
	const char* cart = "test.p8";
	const char* out = nullptr;
	bool exportAll = false;

	if (argc > 1) {
		if (strcmp(argv[1], "--export-all") == 0) {
			exportAll = true;
		} else {
			cart = argv[1];

			if (argc > 2) {
				out = argv[2];
			}
		}
	}

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("pemsa", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512,512, SDL_WINDOW_RESIZABLE);

	if (window == NULL) {
		std::cerr << "Failed to open a window\n";
		SDL_Quit();

		return 1;
	}

	SDL_SetWindowMinimumSize(window, 128, 128);

	SdlGraphicsBackend* graphics = new SdlGraphicsBackend(window);
	SdlInputBackend* input = new SdlInputBackend();
	bool running = true;

	PemsaEmulator emulator(graphics, new SdlAudioBackend(), input, &running);
	SDL_ShowCursor(0);

	if (exportAll) {
		for (auto& dirEntry : std::filesystem::directory_iterator("./")) {
			if (dirEntry.path().extension() != ".p8") {
				continue;
			}

			if (!emulator.getCartridgeModule()->load(dirEntry.path().c_str(), true)) {
				std::cerr << "Failed to load the cart " << dirEntry << "\n";
			}	else {
				emulator.getCartridgeModule()->save(("out/" + dirEntry.path().string()).c_str());
			}
		}

		SDL_DestroyWindow(window);
		SDL_Quit();

		return 0;
	}

	if (!emulator.getCartridgeModule()->load(cart, out != nullptr)) {
		std::cerr << "Failed to load the cart " << cart << "\n";

		SDL_DestroyWindow(window);
		SDL_Quit();

		return 1;
	}

	if (out != nullptr && emulator.getCartridgeModule()->save(out)) {
		std::cout << "Saved the cart\n";
		return 0;
	}

	SDL_Event event;

	Uint32 start_time = SDL_GetTicks();

	double fps = 60;
	double delta = 1 / fps;
	Uint32 ticks_per_frame = 1000 / fps;

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			} else {
				if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_F5) {
					emulator.getCartridgeModule()->initiateSelfDestruct();
				} else {
					input->handleEvent(&event);
					graphics->handleEvent(&event);
				}
			}
		}

		emulator.update(delta);
		graphics->render();

		Uint32 end_time = SDL_GetTicks();
		Uint32 difference = end_time - start_time;

		if (difference < ticks_per_frame) {
			SDL_Delay(ticks_per_frame - difference);

			end_time = SDL_GetTicks();
			difference = end_time - start_time;
		}

		delta = difference / 1000.0;
		graphics->setFps(1000.0 / difference);

		start_time = end_time;
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}