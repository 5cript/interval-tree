#include "gtest/gtest.h"

#include "../interval_tree.hpp"
#include "typedefs.hpp"
#include "example_drawings.hpp"

// following headers expect to be included after gtest headers and interval_tree
#include "interval_tests.hpp"
#include "insert_tests.hpp"

int main(int argc, char** argv)
{
#ifdef INTERVAL_TREE_DO_DRAWINGS
    drawAll();
#endif // INTERVAL_TREE_DO_DRAWINGS

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
