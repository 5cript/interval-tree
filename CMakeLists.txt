cmake_minimum_required(VERSION 3.21)

include(cmake/options.cmake)

project(interval-tree)

add_library(interval-tree INTERFACE)

target_include_directories(interval-tree INTERFACE ./include)

if(DRAW_EXAMPLES)
    add_subdirectory(cairo-wrap)
    add_subdirectory(drawings)
endif()
add_subdirectory(tests)