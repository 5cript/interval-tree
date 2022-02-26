cmake_minimum_required(VERSION 3.11)

add_library(interval-tree INTERFACE)

target_include_directories(interval-tree INTERFACE ./include)

add_subdirectory(tests)