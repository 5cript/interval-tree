#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <interval-tree/interval_tree.hpp>
#include "typedefs.hpp"

// following headers expect to be included after gtest headers and interval_tree
#include "interval_tests.hpp"
#include "interval_tree_tests.hpp"
#include "insert_tests.hpp"
#include "erase_tests.hpp"
#include "find_tests.hpp"
#include "overlap_find_tests.hpp"
#include "float_overlap_tests.hpp"
#include "iteration_tests.hpp"
#include "hook_tests.hpp"

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
