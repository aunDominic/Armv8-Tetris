cmake_minimum_required(VERSION 3.28)
project(Emulator C)

set(CMAKE_C_STANDARD 99)

add_library(Emulator STATIC
        memory.h
        memory.c
        dataprocessing.c
        dataprocessing.h
        shifter.c
        shifter.h
        datatransfer.c
        datatransfer.h
        branch.c
        branch.h
        special.c
        special.h
        bitmanipulation.c
        bitmanipulation.h
)
# Include the 'src' directory
include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(Emulator datatransfer.c)