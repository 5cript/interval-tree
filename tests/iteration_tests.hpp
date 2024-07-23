#pragma once

#include <ctime>
#include <random>
#include <cmath>

class IterationTests
    : public ::testing::Test
{
public:
    using types = IntervalTypes <int>;
    void makeTree1()
    {
        tree.insert({1, 5});
        tree.insert({8, 9});
        tree.insert({3, 7});
        tree.insert({10, 15});
        tree.insert({-5, 8});
        tree.insert({20, 25});
        tree.insert({-10, -5});
    }

    void makeTree2()
    {
        for (int i = 0; i < 100; ++i)
            tree.insert({i * 2, i * 2 + 1});
    }

protected:
    IntervalTypes <int>::tree_type tree;
    std::default_random_engine gen;
    std::uniform_int_distribution <int> distLarge{-50000, 50000};
};

TEST_F(IterationTests, TreeIterationIsInOrder)
{
    makeTree1();
    auto iter = std::begin(tree);
    EXPECT_EQ(iter->low(), -10);
    ++iter;
    EXPECT_EQ(iter->low(), -5);
    ++iter;
    EXPECT_EQ(iter->low(), 1);
    ++iter;
    EXPECT_EQ(iter->low(), 3);
    ++iter;
    EXPECT_EQ(iter->low(), 8);
    ++iter;
    EXPECT_EQ(iter->low(), 10);
    ++iter;
    EXPECT_EQ(iter->low(), 20);
}

TEST_F(IterationTests, TreeIterationIsInOrder2)
{
    makeTree2();
    auto iter = std::begin(tree);
    for (int i = 0; i < 100; ++i)
    {
        EXPECT_EQ(iter->low(), i * 2);
        ++iter;
    }
}

TEST_F(IterationTests, ReverseIterationIsReverseInOrder)
{
    makeTree1();
    auto iter = std::rbegin(tree);
    EXPECT_EQ(iter->low(), 20);
    ++iter;
    EXPECT_EQ(iter->low(), 10);
    ++iter;
    EXPECT_EQ(iter->low(), 8);
    ++iter;
    EXPECT_EQ(iter->low(), 3);
    ++iter;
    EXPECT_EQ(iter->low(), 1);
    ++iter;
    EXPECT_EQ(iter->low(), -5);
    ++iter;
    EXPECT_EQ(iter->low(), -10);
}

TEST_F(IterationTests, ReverseIterationIsReverseInOrder2)
{
    makeTree2();
    auto iter = std::rbegin(tree);
    for (int i = 99; i >= 0; --i)
    {
        EXPECT_EQ(iter->low(), i * 2);
        ++iter;
    }
}