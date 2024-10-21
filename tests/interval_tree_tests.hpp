#pragma once

#include "test_utility.hpp"
#include "multi_join_interval.hpp"

#include <ctime>
#include <random>
#include <cmath>

class IntervalTreeTests : public ::testing::Test
{
  public:
    using types = IntervalTypes<int>;
};

TEST_F(IntervalTreeTests, CanCopyConstruct)
{
    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    auto tree2 = tree;
    EXPECT_EQ(tree2.size(), 1);
    EXPECT_EQ(tree.size(), 1);
    EXPECT_EQ(tree2.begin()->low(), 0);
    EXPECT_EQ(tree2.begin()->high(), 5);
}

TEST_F(IntervalTreeTests, CanMoveConstruct)
{
    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    auto tree2 = std::move(tree);
    EXPECT_EQ(tree2.size(), 1);
    EXPECT_EQ(tree.size(), 0);
    EXPECT_EQ(tree2.begin()->low(), 0);
    EXPECT_EQ(tree2.begin()->high(), 5);
}

TEST_F(IntervalTreeTests, CanCopyAssign)
{
    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    auto tree2 = types::tree_type{};
    tree2.insert(types::interval_type{5, 10});
    tree2.insert(types::interval_type{1, 2});
    tree2 = tree;
    EXPECT_EQ(tree2.size(), 1);
    EXPECT_EQ(tree.size(), 1);
    EXPECT_EQ(tree2.begin()->low(), 0);
    EXPECT_EQ(tree2.begin()->high(), 5);
}

TEST_F(IntervalTreeTests, CanMoveAssign)
{
    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    auto tree2 = types::tree_type{};
    tree2.insert(types::interval_type{5, 10});
    tree2.insert(types::interval_type{1, 2});
    tree2 = std::move(tree);
    EXPECT_EQ(tree2.size(), 1);
    EXPECT_EQ(tree.size(), 0);
    EXPECT_EQ(tree2.begin()->low(), 0);
    EXPECT_EQ(tree2.begin()->high(), 5);
}

TEST_F(IntervalTreeTests, CanCopyBiggerTree)
{
    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    tree.insert(types::interval_type{10, 15});
    tree.insert(types::interval_type{15, 20});
    tree.insert(types::interval_type{20, 25});
    tree.insert(types::interval_type{25, 30});
    tree.insert(types::interval_type{30, 35});
    tree.insert(types::interval_type{35, 40});
    tree.insert(types::interval_type{40, 45});
    tree.insert(types::interval_type{45, 50});
    auto tree2 = tree;
    EXPECT_EQ(tree2.size(), 10);
    EXPECT_EQ(tree.size(), 10);

    for (auto i = tree.begin(), j = tree2.begin(); i != tree.end(); ++i, ++j)
    {
        EXPECT_EQ(i->low(), j->low());
        EXPECT_EQ(i->high(), j->high());
    }
}

TEST_F(IntervalTreeTests, CanMoveBiggerTree)
{
    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    tree.insert(types::interval_type{10, 15});
    tree.insert(types::interval_type{15, 20});
    tree.insert(types::interval_type{20, 25});
    tree.insert(types::interval_type{25, 30});
    tree.insert(types::interval_type{30, 35});
    tree.insert(types::interval_type{35, 40});
    tree.insert(types::interval_type{40, 45});
    tree.insert(types::interval_type{45, 50});
    auto tree2 = std::move(tree);
    EXPECT_EQ(tree2.size(), 10);
    EXPECT_EQ(tree.size(), 0);

    for (auto i = tree2.begin(); i != tree2.end(); ++i)
    {
        EXPECT_EQ(i->low(), i->high() - 5);
    }
}