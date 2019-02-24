cmake_minimum_required(VERSION 3.10)
project(CPP-8)
set(CMAKE_CXX_STANDARD 17)

#SFML
if(DEFINED CPP8_ENGINE AND ${CPP8_ENGINE} STREQUAL "SFML")
    message("Using SFML")
    find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
    add_library(cpp8lib src/Chip8.cpp
                        src/Chip8_SFML.cpp)
    add_executable(cpp8 src/main.cpp)
    target_link_libraries(cpp8 cpp8lib sfml-graphics sfml-audio)

#SDL2
else()
    message("Using SDL2")
    find_package(SDL2 REQUIRED)
    add_library(cpp8lib src/Chip8.cpp
                        src/Chip8_SDL.cpp)

    if(COMMAND cmake_policy)
        cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)

    add_executable(cpp8 src/main.cpp)
    target_link_libraries(cpp8 cpp8lib SDL2)
endif()
