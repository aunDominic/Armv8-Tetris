cmake_minimum_required(VERSION 3.28)
project(Emulator C)

set(CMAKE_C_STANDARD 99)

add_library(Emulator STATIC
        src/memory.h
        src/memory.c
        src/dataprocessing.c
        src/dataprocessing.h
        src/shifter.c
        src/shifter.h
        src/datatransfer.c
        src/datatransfer.h
        src/branch.c
        src/branch.h
        src/special.c
        src/special.h
)
