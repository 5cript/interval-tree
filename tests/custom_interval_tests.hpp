#pragma once

#include "test_utility.hpp"
#include "multi_join_interval.hpp"

#include <ctime>
#include <random>
#include <cmath>
#include <functional>

class CustomIntervalTests : public ::testing::Test
{};

template <typename numerical_type, typename interval_kind_ = lib_interval_tree::closed>
struct custom_interval : public lib_interval_tree::interval<numerical_type, interval_kind_>
{
  public:
    using value_type = numerical_type;
    using interval_kind = interval_kind_;

    using lib_interval_tree::interval<numerical_type, interval_kind_>::low_;
    using lib_interval_tree::interval<numerical_type, interval_kind_>::high_;

    /**
     *  Constructs an custom_interval. low MUST be smaller than high.
     */
    custom_interval(value_type low, value_type high)
        : lib_interval_tree::interval<numerical_type, interval_kind_>{low, high}
    {}

    std::function<bool(value_type, value_type)> on_overlaps;
    bool overlaps(custom_interval const& other) const
    {
        if (on_overlaps)
            return on_overlaps(other.low_, other.high_);
        return interval_kind::overlaps(low_, high_, other.low_, other.high_);
    }

    std::function<bool(custom_interval const& other)> on_overlaps_exclusive_ival;
    bool overlaps_exclusive(custom_interval const& other) const
    {
        if (on_overlaps_exclusive_ival)
            return on_overlaps_exclusive_ival(other);
        return low_ < other.high_ && other.low_ < high_;
    }

    std::function<custom_interval(custom_interval const& other)> on_join;
    custom_interval join(custom_interval const& other) const
    {
        if (on_join)
            return on_join(other);
        return {std::min(low_, other.low_), std::max(high_, other.high_)};
    }
};

TEST_F(CustomIntervalTests, CanInsertCustomIntervalJoined)
{
    lib_interval_tree::interval_tree<custom_interval<int>> tree;
    tree.insert({0, 5});
    tree.insert_overlap({4, 10});

    ASSERT_EQ(tree.size(), 1);
    EXPECT_EQ(tree.begin()->low(), 0);
    EXPECT_EQ(tree.begin()->high(), 10);
}

TEST_F(CustomIntervalTests, CustomJoinIsCalled)
{
    lib_interval_tree::interval_tree<custom_interval<int>> tree;
    auto ival1 = custom_interval<int>{0, 5};
    auto ival2 = custom_interval<int>{4, 10};

    bool join_called = false;
    ival1.on_join = [&](custom_interval<int> const& other) -> custom_interval<int> {
        join_called = true;
        return {std::min(ival1.low_, other.low_), std::max(ival1.high_, other.high_)};
    };

    tree.insert(ival1);
    tree.insert_overlap(ival2);

    EXPECT_TRUE(join_called);
}

TEST_F(CustomIntervalTests, CustomOverlapsIsCalled)
{
    lib_interval_tree::interval_tree<custom_interval<int>> tree;
    auto ival1 = custom_interval<int>{0, 5};
    auto ival2 = custom_interval<int>{4, 10};

    bool overlaps_called = false;
    ival1.on_overlaps = [&](int l, int h) -> bool {
        overlaps_called = true;
        return custom_interval<int>::interval_kind::overlaps(ival1.low_, ival1.high_, l, h);
    };

    tree.insert(ival1);
    tree.insert_overlap(ival2);

    EXPECT_TRUE(overlaps_called);
}

TEST_F(CustomIntervalTests, CustomOverlapsExclusiveIvalIsCalled)
{
    lib_interval_tree::interval_tree<custom_interval<int>> tree;
    auto ival1 = custom_interval<int>{0, 5};
    auto ival2 = custom_interval<int>{4, 10};

    bool overlaps_exclusive_ival_called = false;
    ival1.on_overlaps_exclusive_ival = [&](custom_interval<int> const& other) -> bool {
        overlaps_exclusive_ival_called = true;
        return ival1.low_ < other.high_ && other.low_ < ival1.high_;
    };

    tree.insert(ival1);
    tree.insert_overlap(ival2, true);

    EXPECT_TRUE(overlaps_exclusive_ival_called);
}