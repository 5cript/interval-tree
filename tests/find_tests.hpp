#pragma once

#include <ctime>
#include <random>
#include <cmath>
#include <algorithm>

class FindTests : public ::testing::Test
{
  public:
    using types = IntervalTypes<int>;

  protected:
    IntervalTypes<int>::tree_type tree;
    std::default_random_engine gen;
    std::uniform_int_distribution<int> distLarge{-50000, 50000};
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
    [](auto const& tree) {
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
    tree.find_all({5, 8}, [&findCount](decltype(tree)::iterator iter) {
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
    tree.find_all({5, 8}, [&findCount](decltype(tree)::iterator iter) {
        ++findCount;
        EXPECT_EQ(*iter, (decltype(tree)::interval_type{5, 8}));
        return findCount < 3;
    });
    EXPECT_EQ(findCount, 3);
}

TEST_F(FindTests, CanFindAllElementsBack)
{
    constexpr int amount = 10'000;

    std::vector<decltype(tree)::interval_type> intervals;
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

    std::vector<decltype(tree)::interval_type> intervals;
    intervals.reserve(amount);
    for (int i = 0; i != amount; ++i)
    {
        const auto interval = lib_interval_tree::make_safe_interval(i * 2, i * 2 + 1);
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

    std::vector<decltype(tree)::interval_type> intervals;
    intervals.reserve(amount);
    for (int i = 0; i != amount; ++i)
    {
        const auto interval = lib_interval_tree::make_safe_interval(i - 1, i + 1);
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
    std::vector<decltype(tree)::interval_type> intervals;
    auto findWithConstTree = [&intervals, &targetInterval](auto const& tree) {
        tree.find_all(targetInterval, [&intervals](auto const& iter) {
            intervals.emplace_back(*iter);
            return true;
        });
    };
    findWithConstTree(tree);

    ASSERT_EQ(intervals.size(), 1);
    EXPECT_EQ(intervals[0], targetInterval);
}

TEST_F(FindTests, WillReturnEndForNextInSubtreeIfTreeIsEmpty)
{
    EXPECT_EQ(tree.find_next_in_subtree(tree.root(), {2, 7}), std::end(tree));
}

TEST_F(FindTests, WillFindNextExactMatchInSubtree)
{
    const auto targetInterval = decltype(tree)::interval_type{0, 5};
    tree.insert(targetInterval);
    tree.insert({10, 15});
    tree.insert(targetInterval);
    tree.insert({30, 35});

    // The node started from is excluded, so this must find the duplicate and not the root itself.
    const auto found = tree.find_next_in_subtree(tree.root(), targetInterval);
    ASSERT_NE(found, std::end(tree));
    EXPECT_EQ(*found, targetInterval);
    EXPECT_NE(found, tree.root());

    EXPECT_EQ(tree.find_next_in_subtree(tree.root(), {99, 100}), std::end(tree));
}

TEST_F(FindTests, WillFindNextExactMatchInSubtreeOnConstTree)
{
    const auto targetInterval = decltype(tree)::interval_type{0, 5};
    tree.insert(targetInterval);
    tree.insert({10, 15});
    tree.insert(targetInterval);
    tree.insert({30, 35});

    [&targetInterval](auto const& tree) {
        const auto found = tree.find_next_in_subtree(tree.root(), targetInterval);
        static_assert(
            std::is_same<decltype(found), typename std::decay_t<decltype(tree)>::const_iterator const>::value,
            "the const overload must yield a const_iterator"
        );
        ASSERT_NE(found, std::end(tree));
        EXPECT_EQ(*found, targetInterval);
    }(tree);
}

TEST_F(FindTests, CanFindNextExactMatchInSubtreeOnConstTreeFromMutableIterator)
{
    const auto targetInterval = decltype(tree)::interval_type{0, 5};
    tree.insert(targetInterval);
    tree.insert({10, 15});
    tree.insert(targetInterval);
    tree.insert({30, 35});

    // A mutable iterator is accepted by the const overloads without converting it by hand.
    const auto from = tree.root();
    const auto compare = [](auto const& lhs, auto const& rhs) {
        return lhs == rhs;
    };
    [&from, &targetInterval, &compare](auto const& tree) {
        const auto found = tree.find_next_in_subtree(from, targetInterval);
        ASSERT_NE(found, std::end(tree));
        EXPECT_EQ(*found, targetInterval);

        const auto foundWithCompare = tree.find_next_in_subtree(from, targetInterval, compare);
        ASSERT_NE(foundWithCompare, std::end(tree));
        EXPECT_EQ(*foundWithCompare, targetInterval);
    }(tree);
}

TEST_F(FindTests, FuzzyFindAllInTree)
{
    std::mt19937 gen{0};
    std::uniform_int_distribution<int> distSmall{-500, 500};

    for (int i = 0; i < 200; ++i)
    {
        const auto generated = distSmall(gen);
        tree.insert(lib_interval_tree::make_safe_interval(generated, generated + 20));
    }
    const auto searchInterval = decltype(tree)::interval_type{20, 50};
    tree.insert(searchInterval);
    tree.insert(searchInterval);
    tree.insert(searchInterval);

    int findCount = 0;
    bool findIsConsistent = true;
    std::vector<decltype(tree)::interval_type> foundIntervals;
    tree.find_all(
        searchInterval,
        [&findIsConsistent, &searchInterval, &findCount, &foundIntervals](decltype(tree)::iterator iter) {
            ++findCount;
            if (*iter != searchInterval)
            {
                findIsConsistent = false;
                return false;
            }
            foundIntervals.emplace_back(*iter);
            return true;
        }
    );

    EXPECT_EQ(findCount, 3);
    ASSERT_TRUE(findIsConsistent);
}

TEST_F(FindTests, FindAndFindAllWithCompareAgreeWithLinearScan)
{
    std::mt19937 gen{0};
    std::uniform_int_distribution<int> distLow{0, 20};
    std::uniform_int_distribution<int> distLength{0, 30};
    for (int i = 0; i < 500; ++i)
    {
        const auto low = distLow(gen);
        tree.insert({low, low + distLength(gen)});
    }

    // Never accepts a high below the searched high, as the comparator overloads require.
    const auto compare = [](auto const& lhs, auto const& rhs) {
        return lhs.low() == rhs.low() && lhs.high() >= rhs.high();
    };
    auto const& constTree = tree;
    for (int low = 0; low <= 20; ++low)
    {
        const auto searchInterval = decltype(tree)::interval_type{low, low + 15};

        int expected = 0;
        for (auto const& ival : tree)
        {
            if (compare(ival, searchInterval))
                ++expected;
        }
        ASSERT_GT(expected, 1);

        int found = 0;
        tree.find_all(
            searchInterval,
            [&found, &searchInterval, &compare](decltype(tree)::iterator iter) {
                EXPECT_TRUE(compare(*iter, searchInterval));
                ++found;
                return true;
            },
            compare
        );
        EXPECT_EQ(found, expected) << "low = " << low;

        int foundConst = 0;
        constTree.find_all(
            searchInterval,
            [&foundConst](decltype(tree)::const_iterator) {
                ++foundConst;
                return true;
            },
            compare
        );
        EXPECT_EQ(foundConst, expected) << "low = " << low;

        const auto first = tree.find(searchInterval, compare);
        ASSERT_NE(first, std::end(tree)) << "low = " << low;
        EXPECT_TRUE(compare(*first, searchInterval));

        // There are at least two matches, so one lies below the root.
        const auto next = tree.find_next_in_subtree(tree.root(), searchInterval, compare);
        ASSERT_NE(next, std::end(tree)) << "low = " << low;
        EXPECT_TRUE(compare(*next, searchInterval));
    }
}

TEST_F(FindTests, WillFindWithFloatToleranceWhenSearchedHighIsLoweredByIt)
{
    constexpr double tolerance = 1e-6;
    IntervalTypes<double>::tree_type floatTree;
    floatTree.insert({5.0, 5.0});
    floatTree.insert({0.0, 1.0});

    // The stored high is slightly below the high searched for, so the search starts the tolerance lower.
    const double searchedHigh = 1.0 + 1e-9;
    const auto found = floatTree.find({0.0, searchedHigh - tolerance}, [](auto const& lhs, auto const& rhs) {
        return std::abs(lhs.low() - rhs.low()) < tolerance && lhs.high() >= rhs.high() &&
            lhs.high() - rhs.high() < 2 * tolerance;
    });
    ASSERT_NE(found, std::end(floatTree));
    EXPECT_EQ(found->low(), 0.0);
    EXPECT_EQ(found->high(), 1.0);
}

TEST_F(FindTests, ExactFindAndFindAllAgreeWithLinearScan)
{
    std::mt19937 gen{0};
    std::uniform_int_distribution<int> distLow{0, 20};
    std::uniform_int_distribution<int> distLength{0, 5};
    std::vector<decltype(tree)::interval_type> intervals;
    for (int i = 0; i < 500; ++i)
    {
        const auto low = distLow(gen);
        intervals.emplace_back(low, low + distLength(gen));
        tree.insert(intervals.back());
    }

    for (auto const& searchInterval : intervals)
    {
        const auto expected = std::count(std::begin(intervals), std::end(intervals), searchInterval);

        int found = 0;
        tree.find_all(searchInterval, [&found, &searchInterval](decltype(tree)::iterator iter) {
            EXPECT_EQ(*iter, searchInterval);
            ++found;
            return true;
        });
        EXPECT_EQ(found, expected);

        const auto first = tree.find(searchInterval);
        ASSERT_NE(first, std::end(tree));
        EXPECT_EQ(*first, searchInterval);
    }
    EXPECT_EQ(tree.find({21, 21}), std::end(tree));
}

struct FindVisitCountingHook : public lib_interval_tree::hooks::regular
{
    struct hook_state
    {
        mutable int find_visits = 0;
        mutable int find_all_visits = 0;
    };

    template <typename tree_type, typename compare_function_type>
    static inline void on_find(
        tree_type const& tree,
        typename tree_type::node_type*,
        typename tree_type::interval_type const&,
        compare_function_type
    ) noexcept
    {
        ++tree.find_visits;
    }

    template <typename tree_type, typename compare_function_type>
    static inline void on_find_all(
        tree_type const& tree,
        typename tree_type::node_type*,
        typename tree_type::interval_type const&,
        compare_function_type
    ) noexcept
    {
        ++tree.find_all_visits;
    }
};

TEST_F(FindTests, ExactFindAndFindAllPruneByMax)
{
    constexpr int amount = 1'000;

    lib_interval_tree::interval_tree<decltype(tree)::interval_type, FindVisitCountingHook> countingTree;
    for (int i = 0; i != amount; ++i)
        countingTree.insert({i * 2, i * 2 + 1});

    // Only the path to the last interval has a max at or above its high.
    const auto lastInterval = decltype(tree)::interval_type{amount * 2 - 2, amount * 2 - 1};

    ASSERT_NE(countingTree.find(lastInterval), std::end(countingTree));
    EXPECT_LT(countingTree.find_visits, amount / 10);

    int found = 0;
    countingTree.find_all(lastInterval, [&found](auto) {
        ++found;
        return true;
    });
    EXPECT_EQ(found, 1);
    EXPECT_LT(countingTree.find_all_visits, amount / 10);
}
