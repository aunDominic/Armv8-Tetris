cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_STANDARD 99)
project(FinalProjectY1_Tetris_SRC C)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.c)

target_sources(${TETRIS_TARGET_NAME} PRIVATE ${SOURCE_FILES})
