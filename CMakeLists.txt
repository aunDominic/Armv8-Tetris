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
		src/assemble/assemble.c
		src/emulate.c
		src/assemble/symbol_table.h
		src/assemble/symbol_table.c
		src/assemble/line_reader.h
		src/assemble/line_reader.c
		src/assemble/opcode_table.h
		src/assemble/opcode_table.c
		src/assemble/directives.c
		src/assemble/directives.h
		src/assemble/common_types.h
		src/assemble/branch_assembly.c
		src/assemble/branch_assembly.h
		src/assemble/loadstore.c
		src/assemble/loadstore.h
		src/assemble/data_processing_assembly.c
		src/assemble/data_processing_assembly.h
)
