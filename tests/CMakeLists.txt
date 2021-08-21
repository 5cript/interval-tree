# Version Check
cmake_minimum_required(VERSION 3.11)

# Default CXX Standard
if (NOT "${CMAKE_CXX_STANDARD}")
	set(CMAKE_CXX_STANDARD 17)
endif()

# Default Build Type
if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
	if (NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
	endif()
endif()

option(DRAW_EXAMPLES "Draws some examples in a subdirectory. run make_drawable.sh before this" OFF )

# Project
project(tree-tests)

# Add Files
file(GLOB sources "*.cpp")

# Add Executable
add_executable(tree-tests ${sources})

target_link_libraries(tree-tests gtest)

# Options
if(DRAW_EXAMPLES)
	target_compile_definitions(tree-tests PRIVATE -DINTERVAL_TREE_DO_DRAWINGS=1)	
	find_library(LCAIRO_WRAP NAMES cairo_wrap PATHS "../../cairo-wrap/build" STATIC)	
	target_link_libraries(tree-tests ${LCAIRO_WRAP} cairo)
	target_include_directories(tree-tests PRIVATE .. ../..)
endif()

# Compiler Options
set(DEBUG_OPTIONS -fexecptions -g -Wall -pedantic-errors -pedantic)
target_compile_options(tree-tests PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")

set(RELEASE_OPTIONS -fexceptions -O3 -Wall -pedantic)
target_compile_options(tree-tests PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")