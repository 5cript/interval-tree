add_executable(from_readme main.cpp)
target_link_libraries(from_readme interval-tree)

set_target_properties(from_readme
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/interval_tree/examples"
)