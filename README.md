# pemsa-sdl
SDL2 backend for pemsa pico8 runtime

```
pemsa [cart] [flags]
	--export-all	Exports carts from input folder to output
	--no-splash	Disables startup splash
	--save [file]	Saves the compiled cart
	--no-fullscreen	Disables fullscreen by default
```

## Compiling

### Linux

Install SDL2:

```
sudo apt install libsdl2-dev
```

And compile:

```
git submodule update --init --recursive
cmake .
make
```

### Windows

Download the Visual C++ SDL2 windows development libraries [here](https://www.libsdl.org/download-2.0.php) and place its contents under `libs/SDL_windows` (i.e. include and lib folders)

```
git submodule update --init --recursive
cmake . -D WINDOWS=1
cmake --build . --target pemsa_sdl --config Debug
```
