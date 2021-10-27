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

```
git submodule update --init --recursive
cmake . -D WINDOWS=1
cmake --build . --target pemsa_sdl --config Debug
```