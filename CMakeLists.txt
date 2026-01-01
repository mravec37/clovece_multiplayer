cmake_minimum_required(VERSION 3.18)
project(semestralka_clovece C)

set(CMAKE_C_STANDARD 99)

add_executable(semestralka_clovece main.c server.c client.c server.h client.h game_logic.c game_logic.h piece.h player.h)
target_link_libraries(semestralka_clovece pthread)