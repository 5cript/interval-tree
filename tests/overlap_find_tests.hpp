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

TEST_F(OverlapFindTests, WillFindOverlapWithRootOnConstTree)
{
    tree.insert({2, 4});
    [](auto const& tree) {
        EXPECT_EQ(tree.overlap_find({2, 7}), std::begin(tree));
    }(tree);
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

TEST_F(OverlapFindTests, WillNotFindOverlapIfNothingOverlaps)
{
    tree.insert({0, 5});
    tree.insert({5, 10});
    tree.insert({10, 15});
    tree.insert({15, 20});
    EXPECT_EQ(tree.overlap_find({77, 99}), std::end(tree));
}

TEST_F(OverlapFindTests, WillNotFindOverlapOnBorderIfExclusive)
{
    tree.insert({0, 5});
    tree.insert({5, 10});
    tree.insert({10, 15});
    tree.insert({15, 20});
    EXPECT_EQ(tree.overlap_find({5, 5}, true), std::end(tree));
}

TEST_F(OverlapFindTests, WillFindMultipleOverlaps)
{
    tree.insert({0, 5});
    tree.insert({5, 10});
    tree.insert({10, 15});
    tree.insert({15, 20});

    std::vector <decltype(tree)::interval_type> intervals;
    tree.overlap_find_all({5, 5}, [&intervals](auto iter) {
        intervals.push_back(*iter);
        return true;
    });
    using ::testing::UnorderedElementsAre;
    ASSERT_THAT(intervals, UnorderedElementsAre(decltype(tree)::interval_type{0, 5}, decltype(tree)::interval_type{5, 10}));
}

TEST_F(OverlapFindTests, FindAllWillFindNothingIfEmpty)
{
    int findCount = 0;
    tree.overlap_find_all({2, 7}, [&findCount](auto){
        ++findCount;
        return true;
    });
    EXPECT_EQ(findCount, 0);
}

TEST_F(OverlapFindTests, FindAllWillFindNothingIfNothingOverlaps)
{
    tree.insert({16, 21});
    tree.insert({8, 9});
    tree.insert({25, 30});
    tree.insert({5, 8});
    tree.insert({15, 23});
    int findCount = 0;
    tree.overlap_find_all({1000, 2000}, [&findCount](auto){
        ++findCount;
        return true;
    });
    EXPECT_EQ(findCount, 0);
}

TEST_F(OverlapFindTests, FindAllWillFindAllWithLotsOfDuplicates)
{
    tree.insert({0, 5});
    tree.insert({0, 5});
    tree.insert({0, 5});
    tree.insert({0, 5});
    tree.insert({0, 5});

    int findCount = 0;
    tree.overlap_find_all({2, 3}, [&findCount](decltype(tree)::iterator iter){
        ++findCount;
        EXPECT_EQ(*iter, (decltype(tree)::interval_type{0, 5}));
        return true;
    });
    EXPECT_EQ(findCount, tree.size());
}

TEST_F(OverlapFindTests, CanExitPreemptivelyByReturningFalse)
{
    tree.insert({0, 5});
    tree.insert({0, 5});
    tree.insert({0, 5});
    tree.insert({0, 5});
    tree.insert({0, 5});

    int findCount = 0;
    tree.overlap_find_all({2, 3}, [&findCount](decltype(tree)::iterator iter){
        ++findCount;
        EXPECT_EQ(*iter, (decltype(tree)::interval_type{0, 5}));
        return findCount < 3;
    });
    EXPECT_EQ(findCount, 3);
}

TEST_F(OverlapFindTests, WillFindSingleOverlapInBiggerTree)
{
    tree.insert({16, 21});
    tree.insert({8, 9});
    tree.insert({25, 30});
    tree.insert({5, 8});
    tree.insert({15, 23});
    tree.insert({17, 19});
    tree.insert({26, 26});
    tree.insert({1000, 2000});
    tree.insert({6, 10});
    tree.insert({19, 20});
    auto iter = tree.overlap_find({1000, 1001});
    EXPECT_NE(iter, std::end(tree));
    EXPECT_EQ(iter->low(), 1000);
    EXPECT_EQ(iter->high(), 2000);
}

TEST_F(FindTests, CanOverlapFindAllOnConstTree)
{
    const auto targetInterval = lib_interval_tree::make_safe_interval(16, 21);
    tree.insert(targetInterval);
    tree.insert({8, 9});
    tree.insert({25, 30});
    std::vector <decltype(tree)::interval_type> intervals;
    auto findWithConstTree = [&intervals, &targetInterval](auto const& tree)
    {
        tree.overlap_find_all(targetInterval, [&intervals](auto const& iter) {
            intervals.emplace_back(*iter);
            return true;
        });
    };
    findWithConstTree(tree);

    ASSERT_EQ(intervals.size(), 1);
    EXPECT_EQ(intervals[0], targetInterval);
}
