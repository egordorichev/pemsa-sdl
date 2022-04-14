cmake_minimum_required(VERSION 3.16)
project(pemsa_sdl)

add_subdirectory(libs/pemsa)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "-g ${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE "-O3 ${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "-g ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 ${CMAKE_CXX_FLAGS_RELEASE}")

IF( DEFINED WINDOWS )
    add_compile_definitions(WINDOWS)
    set(SDL2_DIR libs/SDL2_windows)
ENDIF( DEFINED WINDOWS )

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} libs/pemsa/include/ libs/pemsa/libs/lua/ libs/pemsa/libs/fixmath/ include/)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

IF(WIN32)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:main")
add_executable(pemsa_sdl WIN32 src/sdl/main.cpp src/sdl/sdl_graphics_backend.cpp src/sdl/sdl_audio_backend.cpp src/sdl/sdl_input_backend.cpp)
ELSE()
add_executable(pemsa_sdl src/sdl/main.cpp src/sdl/sdl_graphics_backend.cpp src/sdl/sdl_audio_backend.cpp src/sdl/sdl_input_backend.cpp)
ENDIF(WIN32)

target_link_libraries(pemsa_sdl pemsa ${SDL2_LIBRARIES} Threads::Threads)

set_target_properties(pemsa_sdl PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(pemsa_sdl PROPERTIES LINK_SEARCH_END_STATIC 1)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

set_target_properties(pemsa_sdl PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dist/)
set_target_properties(pemsa_sdl PROPERTIES OUTPUT_NAME "pemsa")
target_link_options(pemsa PRIVATE -mwindows)

install(TARGETS pemsa_sdl)
