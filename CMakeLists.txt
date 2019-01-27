project(CPP-8)
set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
add_library(cpp8lib src/Chip8.cpp
                 src/Chip8_SFML.cpp)
add_executable(cpp8 src/main.cpp)
target_link_libraries(cpp8 cpp8lib sfml-graphics)
