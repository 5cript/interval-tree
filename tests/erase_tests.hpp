#pragma once

#include <ctime>
#include <random>
#include <cmath>

class EraseTests
    : public ::testing::Test
{
public:
    using types = IntervalTypes <int>;

protected:
    IntervalTypes <int>::tree_type tree;
    std::default_random_engine gen;
    std::uniform_int_distribution <int> distSmall{-500, 500};
    std::uniform_int_distribution <int> distLarge{-50000, 50000};
};

TEST_F(EraseTests, EraseSingleElement)
{
    auto inserted_interval = types::interval_type{0, 16};

    tree.insert(inserted_interval);

    tree.erase(tree.begin());

    EXPECT_EQ(tree.empty(), true);
    EXPECT_EQ(tree.size(), 0);
}

TEST_F(EraseTests, ManualClearTest)
{
    constexpr int amount = 10'000;

    for (int i = 0; i != amount; ++i)
        tree.insert(lib_interval_tree::make_safe_interval(distSmall(gen), distSmall(gen)));

    for (auto i = std::begin(tree); i != std::end(tree);)
        i = tree.erase(i);

    EXPECT_EQ(tree.empty(), true);
}

TEST_F(EraseTests, ClearTest)
{
    constexpr int amount = 10'000;

    for (int i = 0; i != amount; ++i)
        tree.insert(lib_interval_tree::make_safe_interval(distSmall(gen), distSmall(gen)));

    tree.clear();

    EXPECT_EQ(tree.empty(), true);
}

TEST_F(EraseTests, RandomEraseTest)
{
    constexpr int amount = 10'000;
    constexpr int deleteAmount = 20;

    for (int i = 0; i != amount; ++i)
        tree.insert(lib_interval_tree::make_safe_interval(distSmall(gen), distSmall(gen)));

    for (int i = 0; i != deleteAmount; ++i)
    {
        std::uniform_int_distribution <int> dist{0, amount - i - 1};
        auto end = dist(gen);
        auto iter = tree.begin();
        for (int j = 0; j != end; ++j)
            ++iter;
    }

    testMaxProperty(tree);
    testRedBlackPropertyViolation(tree);
    testTreeHeightHealth(tree);
}
