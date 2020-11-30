cmake_minimum_required(VERSION 3.17)
project(pemsa_sdl)

add_subdirectory(libs/pemsa)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} libs/pemsa/include/ libs/pemsa/libs/lua/ libs/pemsa/libs/fixmath/ include/)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(pemsa_sdl src/sdl/main.cpp src/sdl/sdl_graphics_backend.cpp src/sdl/sdl_audio_backend.cpp src/sdl/sdl_input_backend.cpp)
target_link_libraries(pemsa_sdl pemsa ${SDL2_LIBRARIES} Threads::Threads)
set_target_properties(pemsa_sdl PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dist/)