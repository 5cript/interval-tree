add_executable(dotgraph main.cpp)
target_link_libraries(dotgraph interval-tree)

set_target_properties(dotgraph
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/interval_tree/examples"
)