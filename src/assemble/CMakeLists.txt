cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_STANDARD 99)
project(FinalProjectY1_Assembler C)

file(GLOB SOURCE_FILES *.c)
file(GLOB HEADER_FILES *.h)

add_executable(assemble ${SOURCE_FILES} ${HEADER_FILES}
		../debug.h)