#include "example_drawings.hpp"

#if __cplusplus >= 201703L
#    include <filesystem>
#endif

int main()
{
#if __cplusplus >= 201703L
    if (!std::filesystem::exists("drawings"))
        std::filesystem::create_directory("drawings");
#endif

    drawAll();
}