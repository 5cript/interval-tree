cmake_minimum_required(VERSION 3.11)

add_executable(interval-tree-drawings "main.cpp")

target_link_libraries(interval-tree-drawings PRIVATE interval-tree cairo cairo-wrap)

set_target_properties(interval-tree-drawings PROPERTIES OUTPUT_NAME "make_drawings")