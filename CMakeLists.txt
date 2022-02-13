cmake_minimum_required(VERSION 2.4)

project(hello_world)

include_directories(${PROJECT_SOURCE_DIR})
add_executable(game_master
    src/game.hpp
    src/game.cpp
    src/game_field.hpp
    src/game_field.cpp
    src/main.cpp)


add_executable(random_player
    src/random_player.cpp
    )

