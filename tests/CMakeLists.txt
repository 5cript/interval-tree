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

# Add Files
file(GLOB sources "*.cpp")

# Add Executable
add_executable(tree-tests ${sources})

target_link_libraries(tree-tests PRIVATE gtest gmock interval-tree)

# Compiler Options
set(DEBUG_OPTIONS -fexceptions -g -Wall -pedantic-errors -pedantic)
target_compile_options(tree-tests PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")

set(RELEASE_OPTIONS -fexceptions -O3 -Wall -pedantic)
target_compile_options(tree-tests PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")