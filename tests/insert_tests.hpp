#pragma once

#include "test_utility.hpp"
#include "multi_join_interval.hpp"

#include <ctime>
#include <random>
#include <cmath>

class InsertTests : public ::testing::Test
{
  public:
    using types = IntervalTypes<int>;

  protected:
    IntervalTypes<int>::tree_type tree;
    std::default_random_engine gen;
    std::uniform_int_distribution<int> distSmall{-500, 500};
    std::uniform_int_distribution<int> distLarge{-50000, 50000};
};

TEST_F(InsertTests, InsertIntoEmpty1)
{
    auto inserted_interval = types::interval_type{0, 16};

    tree.insert(inserted_interval);
    EXPECT_EQ(*tree.begin(), inserted_interval);
    EXPECT_EQ(tree.size(), 1);
}

TEST_F(InsertTests, InsertIntoEmpty2)
{
    auto inserted_interval = types::interval_type{-45, 16};

    tree.insert(inserted_interval);
    EXPECT_EQ(*tree.begin(), inserted_interval);
    EXPECT_EQ(tree.size(), 1);
}

TEST_F(InsertTests, InsertMultipleIntoEmpty)
{
    auto firstInterval = types::interval_type{0, 16};
    auto secondInterval = types::interval_type{5, 13};

    tree.insert(firstInterval);
    tree.insert(secondInterval);

    EXPECT_EQ(tree.size(), 2);

    EXPECT_EQ(*tree.begin(), firstInterval);
    EXPECT_EQ(*(++tree.begin()), secondInterval);
}

TEST_F(InsertTests, TreeHeightHealthynessTest)
{
    constexpr int amount = 100'000;

    for (int i = 0; i != amount; ++i)
        tree.insert(lib_interval_tree::make_safe_interval(distSmall(gen), distSmall(gen)));

    testTreeHeightHealth(tree);
}

TEST_F(InsertTests, MaxValueTest1)
{
    constexpr int amount = 100'000;

    for (int i = 0; i != amount; ++i)
        tree.insert(lib_interval_tree::make_safe_interval(distSmall(gen), distSmall(gen)));

    testMaxProperty(tree);
}

TEST_F(InsertTests, RBPropertyInsertTest)
{
    constexpr int amount = 1000;

    for (int i = 0; i != amount; ++i)
        tree.insert(lib_interval_tree::make_safe_interval(distSmall(gen), distSmall(gen)));

    testRedBlackPropertyViolation(tree);
}

TEST_F(InsertTests, IntervalsMayReturnMultipleIntervalsForJoin)
{
    using interval_type = multi_join_interval<int>;
    using tree_type = lib_interval_tree::interval_tree<interval_type>;

    auto multiJoinTree = tree_type{};

    multiJoinTree.insert({0, 2});
    multiJoinTree.insert_overlap({0, 4});

    EXPECT_EQ(multiJoinTree.size(), 2);
    EXPECT_EQ(*multiJoinTree.begin(), (interval_type{0, 1}))
        << multiJoinTree.begin()->low() << multiJoinTree.begin()->high();
    EXPECT_EQ(*++multiJoinTree.begin(), (interval_type{3, 4}));
}

TEST_F(InsertTests, IntervalsMayReturnMultipleIntervalsForJoinAndJoinRecursively)
{
    using interval_type = multi_join_interval<int>;
    using tree_type = lib_interval_tree::interval_tree<interval_type>;

    auto multiJoinTree = tree_type{};

    multiJoinTree.insert({0, 10});
    multiJoinTree.insert({5, 10});
    multiJoinTree.insert_overlap({0, 20}, false, true);

    EXPECT_EQ(multiJoinTree.size(), 3);

    auto iter = multiJoinTree.begin();

    EXPECT_EQ(*iter, (interval_type{0, 4})) << multiJoinTree.begin()->low() << multiJoinTree.begin()->high();
    EXPECT_EQ(*++iter, (interval_type{6, 10})) << multiJoinTree.begin()->low() << multiJoinTree.begin()->high();
    EXPECT_EQ(*++iter, (interval_type{11, 20})) << multiJoinTree.begin()->low() << multiJoinTree.begin()->high();
}

TEST_F(InsertTests, CanInsertOverlapRecursively)
{
    using tree_type = lib_interval_tree::interval_tree<types::interval_type>;

    auto tree = tree_type{};
    tree.insert({0, 9});
    tree.insert({20, 29});
    tree.insert_overlap({8, 21}, false, true);

    EXPECT_EQ(tree.size(), 1);
    EXPECT_EQ(tree.begin()->low(), 0);
    EXPECT_EQ(tree.begin()->high(), 29);
}
