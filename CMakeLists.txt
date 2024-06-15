cmake_minimum_required(VERSION 3.28)
project(Emulator C)

set(CMAKE_C_STANDARD 99)

add_library(Emulator STATIC
		src/emulate/memory.h
		src/emulate/memory.c
		src/emulate/dataprocessing.c
		src/emulate/dataprocessing.h
		src/emulate/shifter.c
		src/emulate/shifter.h
		src/emulate/datatransfer.c
		src/emulate/datatransfer.h
		src/emulate/branch.c
		src/emulate/branch.h
		src/emulate/special.c
		src/emulate/special.h
		src/assemble/assemble.c
		src/emulate/emulate.c
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
		src/assemble/parsing_common.c
		src/assemble/parsing_common.h
		src/emulate/bitmanipulation.c
)
