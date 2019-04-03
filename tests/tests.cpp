#include "gtest/gtest.h"
#include "../interval_tree.hpp"

// following headers expect to be included after gtest headers and interval_tree
#include "find_tests.hpp"

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
