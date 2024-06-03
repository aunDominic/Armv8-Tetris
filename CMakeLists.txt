cmake_minimum_required(VERSION 3.28)
project(myarm_v8 C)

set(CMAKE_C_STANDARD 23)

add_executable(myarm_v8
        src/assemble.c
        src/emulate.c
        src/emulate/memory.h
        src/emulate/memory.c
        src/emulate/registers.c
        src/types.h
        src/emulate/registers.h
        src/util.h
        src/util.c
        src/instruction_set.h
        src/instruction_set.c
        src/instructions/data_processing.h
        src/instructions/data_processing.c)
