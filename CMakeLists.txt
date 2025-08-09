cmake_minimum_required(VERSION 3.11)

include(cmake/options.cmake)

project(interval-tree)

add_library(interval-tree INTERFACE)

if(INT_TREE_USE_OPTIONAL_POLYFILL)
    target_compile_definitions(interval-tree INTERFACE
        -DINTERVAL_TREE_USE_OPTIONAL_POLYFILL
    )
endif()

target_include_directories(interval-tree INTERFACE ./include)

if(${MSVC})
    target_compile_options(interval-tree INTERFACE /Zc:__cplusplus)
endif()

if(INT_TREE_DRAW_EXAMPLES)
    add_subdirectory(cairo-wrap)
    add_subdirectory(drawings)
endif()
if (INT_TREE_ENABLE_TESTS)
    add_subdirectory(tests)
endif()
if (INT_TREE_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()