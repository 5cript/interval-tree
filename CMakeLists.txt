cmake_minimum_required(VERSION 3.21)

include(cmake/options.cmake)

project(interval-tree)

add_library(interval-tree INTERFACE)

target_include_directories(interval-tree INTERFACE ./include)

if(INT_TREE_DRAW_EXAMPLES)
    add_subdirectory(cairo-wrap)
    add_subdirectory(drawings)
endif()
if (INT_TREE_ENABLE_TESTS)
    add_subdirectory(tests)
endif()