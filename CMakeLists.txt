cmake_minimum_required(VERSION 3.10)
project(platformer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fsanitize=address -fsanitize=undefined -g -fdiagnostics-color=always)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")

include_directories(/usr/local/include)

set(SOURCES
        platformer.cpp
        graphics.cpp
        level.cpp
        player.cpp
        enemy.cpp
)

add_executable(platformer ${SOURCES})

target_link_libraries(platformer PRIVATE raylib)
