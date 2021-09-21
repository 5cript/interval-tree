#pragma once

#include <ctime>
#include <random>
#include <cmath>

class FindTests
    : public ::testing::Test
{
public:
    using types = IntervalTypes <int>;
protected:
    IntervalTypes <int>::tree_type tree;
    std::default_random_engine gen;
    std::uniform_int_distribution <int> distLarge{-50000, 50000};
};

TEST_F(FindTests, WillReturnEndIfTreeIsEmpty)
{
    EXPECT_EQ(tree.find({2, 7}), std::end(tree));
}

TEST_F(FindTests, WillNotFindRootIfItIsntTheSame)
{
    tree.insert({0, 1});
    EXPECT_EQ(tree.find({2, 7}), std::end(tree));
}

TEST_F(FindTests, WillFindRoot)
{
    tree.insert({0, 1});
    EXPECT_EQ(tree.find({0, 1}), std::begin(tree));
}

TEST_F(FindTests, WillFindRootOnConstTree)
{
    tree.insert({0, 1});
    [](auto const& tree)
    {
        EXPECT_EQ(tree.find({0, 1}), std::begin(tree));
    }(tree);
}

TEST_F(FindTests, WillFindInBiggerTree)
{
    tree.insert({16, 21});
    tree.insert({8, 9});
    tree.insert({25, 30});
    tree.insert({5, 8});
    tree.insert({15, 23});
    tree.insert({17, 19});
    tree.insert({26, 26});
    tree.insert({0, 3});
    tree.insert({6, 10});
    tree.insert({19, 20});
    auto iter = tree.find({15, 23});
    EXPECT_NE(iter, std::end(tree));
    EXPECT_EQ(iter->low(), 15);
    EXPECT_EQ(iter->high(), 23);
}

TEST_F(FindTests, WillFindAllInTreeWithDuplicates)
{
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    int findCount = 0;
    tree.find_all({5, 8}, [&findCount](decltype(tree)::iterator iter){
        ++findCount;
        EXPECT_EQ(*iter, (decltype(tree)::interval_type{5, 8}));
        return true;
    });
    EXPECT_EQ(findCount, tree.size());
}

TEST_F(FindTests, WillFindAllCanExitPreemptively)
{
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    int findCount = 0;
    tree.find_all({5, 8}, [&findCount](decltype(tree)::iterator iter){
        ++findCount;
        EXPECT_EQ(*iter, (decltype(tree)::interval_type{5, 8}));
        return findCount < 3;
    });
    EXPECT_EQ(findCount, 3);
}

TEST_F(FindTests, CanFindAllElementsBack)
{
    constexpr int amount = 10'000;

    std::vector <decltype(tree)::interval_type> intervals;
    intervals.reserve(amount);
    for (int i = 0; i != amount; ++i)
    {
        const auto interval = lib_interval_tree::make_safe_interval(distLarge(gen), distLarge(gen));
        intervals.emplace_back(interval);
        tree.insert(interval);
    }
    for (auto const& ival : intervals)
    {
        ASSERT_NE(tree.find(ival), std::end(tree));
    }
}

TEST_F(FindTests, CanFindAllElementsBackInStrictlyAscendingNonOverlappingIntervals)
{
    constexpr int amount = 10'000;

    std::vector <decltype(tree)::interval_type> intervals;
    intervals.reserve(amount);
    for (int i = 0; i != amount; ++i)
    {
        const auto interval = lib_interval_tree::make_safe_interval(i * 2,  i * 2 + 1);
        intervals.emplace_back(interval);
        tree.insert(interval);
    }
    for (auto const& ival : intervals)
    {
        ASSERT_NE(tree.find(ival), std::end(tree));
    }
}

TEST_F(FindTests, CanFindAllElementsBackInStrictlyAscendingOverlappingIntervals)
{
    constexpr int amount = 10'000;

    std::vector <decltype(tree)::interval_type> intervals;
    intervals.reserve(amount);
    for (int i = 0; i != amount; ++i)
    {
        const auto interval = lib_interval_tree::make_safe_interval(i - 1,  i + 1);
        intervals.emplace_back(interval);
        tree.insert(interval);
    }
    for (auto const& ival : intervals)
    {
        ASSERT_NE(tree.find(ival), std::end(tree));
    }
}

TEST_F(FindTests, CanFindAllOnConstTree)
{
    const auto targetInterval = lib_interval_tree::make_safe_interval(16, 21);
    tree.insert(targetInterval);
    tree.insert({8, 9});
    tree.insert({25, 30});
    std::vector <decltype(tree)::interval_type> intervals;
    auto findWithConstTree = [&intervals, &targetInterval](auto const& tree)
    {
        tree.find_all(targetInterval, [&intervals](auto const& iter) {
            intervals.emplace_back(*iter);
            return true;
        });
    };
    findWithConstTree(tree);

    ASSERT_EQ(intervals.size(), 1);
    EXPECT_EQ(intervals[0], targetInterval);
}