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

find_package(GTest REQUIRED)

target_link_libraries(tree-tests PRIVATE interval-tree GTest::gtest GTest::gmock GTest::gmock_main)

# Compiler Options
set(DEBUG_OPTIONS -fexceptions -g -Wall -pedantic-errors -pedantic)
target_compile_options(tree-tests PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")

if (${MSVC})
    set(WARNING_FLAGS -W4)
else()
    set(WARNING_FLAGS -Wall)
    list(APPEND WARNING_FLAGS -Wextra)
endif()
set(RELEASE_OPTIONS -fexceptions -O3 ${WARNING_FLAGS} -pedantic)
target_compile_options(tree-tests PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")

if (INT_TREE_DRAW_EXAMPLES)
    target_link_libraries(tree-tests PRIVATE cairo-wrap cairo)
endif()

# If msys2, copy dynamic libraries to executable directory, visual studio does this automatically.
# And there is no need on linux.
if (DEFINED ENV{MSYSTEM})
    add_custom_command(TARGET tree-tests POST_BUILD
        COMMAND bash -c "ldd $<TARGET_FILE:tree-tests>" | "grep" "clang" | awk "NF == 4 { system(\"${CMAKE_COMMAND} -E copy \" \$3 \" $<TARGET_FILE_DIR:tree-tests>\") }"
        VERBATIM
    )
endif()