# pemsa-sdl
SDL2 backend for pemsa pico8 runtime

## Compiling

### Linux

```
git submodule update --init --recursive
cmake .
make
```

### Windows

Download the SDL2 windows development libraries [here](https://www.libsdl.org/download-2.0.php) and place it under `libs/SDL_windows`

```
git submodule update --init --recursive
cmake . -D WINDOWS=1
cmake --build . --target pemsa_sdl --config Debug
```