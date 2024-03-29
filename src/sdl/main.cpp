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
	bool fullscreen = true;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			const char *arg = argv[i];

			if (strcmp(arg, "--export-all") == 0) {
				exportAll = true;
			} else if (strcmp(arg, "--no-splash") == 0) {
				enableSplash = false;
			} else if (strcmp(arg, "--no-fullscreen") == 0) {
				fullscreen = false;
			} else if (strcmp(arg, "--help") == 0) {
				std::cout << "pemsa [cart] [flags]\n"
         "\t--export-all\tExports carts from input folder to output\n"
         "\t--no-splash\tDisables startup splash\n"
         "\t--save [file]\tSaves the compiled cart\n"
         "\t--no-fullscreen\tDisables fullscreen by default\n";

				return 0;
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
	SDL_Window* window = SDL_CreateWindow("pemsa", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512,512, SDL_WINDOW_RESIZABLE | (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));

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
		if (!std::filesystem::exists("input")) {
			std::cerr << "No 'input' directory detected\n";
			return 1;
		}

		if (!std::filesystem::exists("output")) {
			std::filesystem::create_directory("output");
		}

		int count = 0;

		for (auto& dirEntry : std::filesystem::directory_iterator("./input")) {
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
				emulator.getCartridgeModule()->save(("output/" + dirEntry.path().filename().string()).c_str());
				count++;
			}
		}

		std::cout << "Exported " << count << " carts\n";

		SDL_DestroyWindow(window);
		SDL_Quit();

		return 0;
	}

	std::cout << "Attempting to load " << cart << "\n";

	if (!emulator.getCartridgeModule()->load(cart, out != nullptr)) {
		if (cartSet) {
			std::cerr << "Failed to load the cart " << cart << "\n";

			SDL_DestroyWindow(window);
			SDL_Quit();

			return 1;
		} else {
			bool loaded = false;

			for (auto& dirEntry : std::filesystem::directory_iterator("./")) {
				if (dirEntry.path().extension() != ".p8") {
					continue;
				}

				auto str = dirEntry.path().c_str();

#ifdef _WIN32 // Dear windows, just why?
				_bstr_t b(str);
				cart = b;
#else
				cart = str;
#endif

				std::cout << "Attempting to load " << cart << "\n";

				if (emulator.getCartridgeModule()->load(cart, out != nullptr)) {
					loaded = true;
					break;
				}
			}

			if (!loaded) {
				emulator.getCartridgeModule()->loadFromString("nocart", noCartPlaceholder, false);
			}
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
	bool skip = false;

	Uint32 ticks_per_frame = 1000 / fps;

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				emulator.stop();
			} else {
				if (event.type == SDL_KEYDOWN) {
					switch (event.key.keysym.scancode) {
						case SDL_SCANCODE_F5: emulator.getCartridgeModule()->initiateSelfDestruct(); break;
						case SDL_SCANCODE_F2: graphics->toggleIntScales(); break;

						case SDL_SCANCODE_F11: {
							fullscreen = !fullscreen;
							SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

							break;
						}

						case SDL_SCANCODE_RETURN: {
							if (SDL_GetModState() & KMOD_ALT) {
								skip = !skip;

								if (!skip) {
									fullscreen = !fullscreen;
									SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
								}
							}

							break;
						}
					}
				}

				input->handleEvent(&event);
				graphics->handleEvent(&event);
			}
		}

		if (emulator.update(delta)) {
			running = false;
		}

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