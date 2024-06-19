cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_STANDARD 99)
project(FinalProjectY1_Tetris)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

# Our Project
set(TETRIS_TARGET_NAME tetris)
add_executable(${TETRIS_TARGET_NAME})
add_subdirectory(src)

set_target_properties(${TETRIS_TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${TETRIS_PATH})

add_custom_command(
	TARGET ${TETRIS_TARGET_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/${TETRIS_PATH}/src/resources $<TARGET_FILE_DIR:${TETRIS_TARGET_NAME}>/resources
	DEPENDS ${TETRIS_TARGET_NAME})

set(raylib_VERBOSE 1)
target_link_libraries(${TETRIS_TARGET_NAME} raylib)

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${TETRIS_TARGET_NAME} "-framework IOKit")
    target_link_libraries(${TETRIS_TARGET_NAME} "-framework Cocoa")
    target_link_libraries(${TETRIS_TARGET_NAME} "-framework OpenGL")
endif()
