cmake_minimum_required(VERSION 2.4)

project(game_master)

include_directories(${PROJECT_SOURCE_DIR})
add_executable(game_master
    src/game.hpp
    src/game.cpp
    src/game_field.hpp
    src/game_field.cpp
    src/main.cpp)

find_package(Threads)

target_include_directories(game_master
    PUBLIC ../socket.io-client-cpp/src/
    )
target_link_libraries(game_master 
    PUBLIC /home/revol-xut/workspace/socket.io-client-cpp/libsioclient.a
    ${CMAKE_THREAD_LIBS_INIT}
    )
add_definitions(
    -lasio
    )

add_executable(random_player
    src/random_player.cpp
    )

