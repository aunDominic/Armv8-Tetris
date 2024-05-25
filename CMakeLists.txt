cmake_minimum_required(VERSION 3.28)
project(Emulator C)

set(CMAKE_C_STANDARD 99)

add_library(Emulator STATIC
        src/registers.h
        src/loader.c
        src/registers.c
        src/instructions/dataprocessing.c
        src/instructions/dataprocessing.h
        src/instructions/shifter.c
        src/instructions/shifter.h
        src/instructions/datatransfer.c
        src/instructions/datatransfer.h
        src/instructions/branch.c
        src/instructions/branch.h
        src/instructions/special.c
        src/instructions/special.h
)
