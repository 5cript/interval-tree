#pragma once

#include "test_utility.hpp"

class OverlapFindTests
    : public ::testing::Test
{
public:
    using types = IntervalTypes <int>;
protected:
    IntervalTypes <int>::tree_type tree;
};

TEST_F(OverlapFindTests, WillReturnEndIfTreeIsEmpty)
{
    EXPECT_EQ(tree.overlap_find({2, 7}), std::end(tree));
}

TEST_F(OverlapFindTests, WillNotFindOverlapWithRootIfItDoesntOverlap)
{
    tree.insert({0, 1});
    EXPECT_EQ(tree.overlap_find({2, 7}), std::end(tree));
}

TEST_F(OverlapFindTests, WillFindOverlapWithRoot)
{
    tree.insert({2, 4});
    EXPECT_EQ(tree.overlap_find({2, 7}), std::begin(tree));
}

TEST_F(OverlapFindTests, WillFindOverlapWithRootIfMatchingExactly)
{
    tree.insert({2, 7});
    EXPECT_EQ(tree.overlap_find({2, 7}), std::begin(tree));
}

TEST_F(OverlapFindTests, WillFindOverlapWithRootIfTouching)
{
    tree.insert({2, 7});
    EXPECT_EQ(tree.overlap_find({7, 9}), std::begin(tree));
}

TEST_F(OverlapFindTests, WillFindMultipleOverlaps)
{
    tree.insert({ 0, 5 });
    tree.insert({ 5, 10 });
    tree.insert({ 10, 15 });
    tree.insert({ 15, 20 });

    auto iter = tree.overlap_find({0, 9}), end = tree.end();
    EXPECT_EQ(iter->low(), 0);
    EXPECT_EQ(iter->high(), 5);

    iter = tree.overlap_find_next(iter, {0, 9});
    EXPECT_EQ(iter->low(), 5);
    EXPECT_EQ(iter->high(), 10);

    iter = tree.overlap_find_next(iter, {0, 9});
    EXPECT_EQ(iter, end);
}

TEST_F(OverlapFindTests, WillFindMultipleOverlapsIfTouching)
{
    tree.insert({ 0, 5 });
    tree.insert({ 5, 10 });
    tree.insert({ 10, 15 });
    tree.insert({ 15, 20 });

    auto iter = tree.overlap_find({5, 5}), end = tree.end();
    EXPECT_EQ(iter->low(), 0);
    EXPECT_EQ(iter->high(), 5);

    iter = tree.overlap_find_next(iter, {5, 5});
    EXPECT_EQ(iter->low(), 5);
    EXPECT_EQ(iter->high(), 10);

    iter = tree.overlap_find_next(iter, {5, 5});
    EXPECT_EQ(iter, end);
}


