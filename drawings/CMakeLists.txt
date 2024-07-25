cmake_minimum_required(VERSION 3.11)

add_executable(interval-tree-drawings "main.cpp")

target_link_libraries(interval-tree-drawings PRIVATE interval-tree cairo cairo-wrap)

set_target_properties(interval-tree-drawings PROPERTIES OUTPUT_NAME "make_drawings")

# If msys2, copy dynamic libraries to executable directory, visual studio does this automatically.
# And there is no need on linux.
if (DEFINED ENV{MSYSTEM})
    add_custom_command(TARGET interval-tree-drawings POST_BUILD
        COMMAND bash -c "ldd $<TARGET_FILE:interval-tree-drawings>" | "grep" "clang" | awk "NF == 4 { system(\"${CMAKE_COMMAND} -E copy \" \$3 \" $<TARGET_FILE_DIR:interval-tree-drawings>\") }"
        VERBATIM
    )
endif()