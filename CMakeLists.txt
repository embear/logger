# minimum required cmake version
cmake_minimum_required(VERSION 2.8)

# project definition
project(logger)

# compile flag definition
add_definitions(-DLOGGER_ENABLE)
add_definitions(-DLOGGER_COLORS)
add_definitions(-DLOGGER_PREFIX_COLORS)

# destination of libraries
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# destination of executables
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# enable warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -pedantic -std=c99")

# source directory
add_subdirectory(src)

# test directory
add_subdirectory(test)
