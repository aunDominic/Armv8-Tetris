cmake_minimum_required(VERSION 3.25)
project(tetris)
find_package(raylib 3.0 REQUIRED)
set(CMAKE_C_STANDARD 11)
add_library(tetris STATIC
		game.c
		raylib_game.c
		main.c
		game.h
		screen_ending.c
		screen_gameplay.c
		screen_logo.c
		screen_options.c
		screen_title.c
		screens.h
		tempCodeRunnerFile.c
		tetriminoes.c
		tetriminoes.h
		game.c
		network.c
		random_piece.c
		random_piece.h)

target_link_libraries(tetris raylib)