cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_STANDARD 99)
project(FinalProjectY1 C)

set(EMULATOR_PATH src/emulate)
add_subdirectory(${EMULATOR_PATH})

set(ASSEMBLER_PATH src/assemble)
add_subdirectory(${ASSEMBLER_PATH})

set(TETRIS_PATH src/tetris)
add_subdirectory(${TETRIS_PATH})