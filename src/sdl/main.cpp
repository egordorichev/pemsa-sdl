#include "sdl/sdl_graphics_backend.hpp"
#include "sdl/sdl_audio_backend.hpp"
#include "sdl/sdl_input_backend.hpp"
#include "sdl/no_cart.hpp"

#include "pemsa/pemsa.hpp"

#ifdef WINDOWS
#include "SDL.h"
#else
#include "SDL2/SDL.h"
#endif

#include <filesystem>
#include <cstring>
#include <iostream>

#ifdef _WIN32
#include <comdef.h>
#endif

int main(int argc, char* argv[]) {
	const char* cart = "splore.p8";
	const char* out = nullptr;

	bool exportAll = false;
	bool enableSplash = true;
	bool cartSet = false;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			const char *arg = argv[i];

			if (strcmp(arg, "--export-all") == 0) {
				exportAll = true;
			} else if (strcmp(arg, "--no-splash") == 0) {
				enableSplash = false;
			} else if (strcmp(arg, "--save") == 0) {
				if (argc > i + 1) {
					out = argv[i + 1];
					i++;
				}
			} else {
				cart = argv[i];
				cartSet = true;
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

	PemsaEmulator emulator(graphics, new SdlAudioBackend(), input, &running, enableSplash);
	SDL_ShowCursor(0);

	if (exportAll) {
		if (!std::filesystem::exists("out")) {
			std::filesystem::create_directory("out");
		}

		for (auto& dirEntry : std::filesystem::directory_iterator("./")) {
			if (dirEntry.path().extension() != ".p8") {
				continue;
			}

			auto str = dirEntry.path().c_str();

			#ifdef _WIN32 // Dear windows, just why?
					_bstr_t b(str);
					const char* out = b;
			#else
					const char* out = str;
			#endif

			if (!emulator.getCartridgeModule()->load(out, true)) {
				std::cerr << "Failed to load the cart " << dirEntry << "\n";
			}	else {
				emulator.getCartridgeModule()->save(("out/" + dirEntry.path().string().substr(2)).c_str());
			}
		}

		SDL_DestroyWindow(window);
		SDL_Quit();

		return 0;
	}

	if (!emulator.getCartridgeModule()->load(cart, out != nullptr)) {
		if (cartSet) {
			std::cerr << "Failed to load the cart " << cart << "\n";

			SDL_DestroyWindow(window);
			SDL_Quit();

			return 1;
		} else {
			emulator.getCartridgeModule()->loadFromString("nocart", noCartPlaceholder, false);
		}
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
				emulator.stop();
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

		Uint32 end_time = SDL_GetTicks();
		Uint32 difference = end_time - start_time;

		if (difference < ticks_per_frame) {
			SDL_Delay(ticks_per_frame - difference);
		}

		emulator.render();

		end_time = SDL_GetTicks();
		difference = end_time - start_time;

		delta = difference / 1000.0;
		graphics->setFps(1000.0 / difference);

		start_time = end_time;
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}