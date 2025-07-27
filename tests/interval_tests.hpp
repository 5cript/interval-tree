#pragma once

#include <limits>

using namespace lib_interval_tree;

class IntervalTests : public ::testing::Test
{
  public:
    template <typename Kind, typename NumericalT>
    auto i(NumericalT l, NumericalT h) const
    {
        return lib_interval_tree::interval<NumericalT, Kind>{l, h};
    }

    template <typename Kind, typename NumericalT>
    auto i(NumericalT l, NumericalT h, interval_border l_border, interval_border r_border) const
    {
        return lib_interval_tree::interval<NumericalT, Kind>{l, h, l_border, r_border};
    }
};

class OverlapTests : public IntervalTests
{};

class ContainmentTests : public IntervalTests
{};

class DistanceTests : public IntervalTests
{};

TEST_F(IntervalTests, FailBadBorders)
{
    auto f = [this]() {
#if __cplusplus >= 201703L
        [[maybe_unused]]
#endif
        auto ival = i<closed>(1, 0);
    };

    ASSERT_THROW(f(), std::invalid_argument);
}

TEST_F(IntervalTests, ShallCreateInterval)
{
    auto ival = i<closed>(1, 24);
    EXPECT_EQ(ival.low(), 1);
    EXPECT_EQ(ival.high(), 24);
}

TEST_F(IntervalTests, ShallCreateInterval2)
{
    auto ival = i<closed>(-23, 24);
    EXPECT_EQ(ival.low(), -23);
    EXPECT_EQ(ival.high(), 24);
}

TEST_F(IntervalTests, ShallCreateInterval3)
{
    auto ival = i<closed>(-21, -12);
    EXPECT_EQ(ival.low(), -21);
    EXPECT_EQ(ival.high(), -12);
}

TEST_F(IntervalTests, ShallCreateInterval4)
{
    auto ival = i<closed>(1, 24);
    EXPECT_EQ(ival.low(), 1);
    EXPECT_EQ(ival.high(), 24);
}

TEST_F(IntervalTests, ShallCreateInterval5)
{
    auto ival = i<closed>(1, 1);
    EXPECT_EQ(ival.low(), 1);
    EXPECT_EQ(ival.high(), 1);
}

TEST_F(IntervalTests, TestLimits)
{
    auto ival = i<closed>(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    EXPECT_EQ(ival.low(), std::numeric_limits<int>::min());
    EXPECT_EQ(ival.high(), std::numeric_limits<int>::max());
}

TEST_F(IntervalTests, TestIntervalSize)
{
    using lib_interval_tree::open;

    EXPECT_EQ(i<closed>(0, 5).size(), 6);
    EXPECT_EQ(i<closed>(-12, 0).size(), 13);
    EXPECT_EQ(i<closed>(-21, 5).size(), 27);
    EXPECT_EQ(i<closed>(-20, -5).size(), 16);
    EXPECT_EQ(i<closed>(100, 125).size(), 26);

    EXPECT_EQ(i<left_open>(0, 5).size(), 5);
    EXPECT_EQ(i<left_open>(-12, 0).size(), 12);
    EXPECT_EQ(i<left_open>(-21, 5).size(), 26);
    EXPECT_EQ(i<left_open>(-20, -5).size(), 15);
    EXPECT_EQ(i<left_open>(100, 125).size(), 25);

    EXPECT_EQ(i<right_open>(0, 5).size(), 5);
    EXPECT_EQ(i<right_open>(-12, 0).size(), 12);
    EXPECT_EQ(i<right_open>(-21, 5).size(), 26);
    EXPECT_EQ(i<right_open>(-20, -5).size(), 15);
    EXPECT_EQ(i<right_open>(100, 125).size(), 25);

    EXPECT_EQ(i<open>(0, 5).size(), 4);
    EXPECT_EQ(i<open>(-12, 0).size(), 11);
    EXPECT_EQ(i<open>(-21, 5).size(), 25);
    EXPECT_EQ(i<open>(-20, -5).size(), 14);
    EXPECT_EQ(i<open>(100, 125).size(), 24);

    EXPECT_EQ(i<closed_adjacent>(0, 5).size(), 6);
    EXPECT_EQ(i<closed_adjacent>(-12, 0).size(), 13);
    EXPECT_EQ(i<closed_adjacent>(-21, 5).size(), 27);
    EXPECT_EQ(i<closed_adjacent>(-20, -5).size(), 16);
    EXPECT_EQ(i<closed_adjacent>(100, 125).size(), 26);
}

TEST_F(OverlapTests, ShallOverlapItself)
{
    using lib_interval_tree::open;

    auto base = i<closed>(0, 5);
    EXPECT_TRUE(base.overlaps(base));

    auto base2 = i<open>(-5, 0);
    EXPECT_TRUE(base2.overlaps(base2));

    auto base3 = i<left_open>(-5, 0);
    EXPECT_TRUE(base3.overlaps(base3));

    auto base4 = i<right_open>(-5, 0);
    EXPECT_TRUE(base4.overlaps(base4));

    auto base5 = i<closed_adjacent>(-5, 0);
    EXPECT_TRUE(base5.overlaps(base5));
}

TEST_F(OverlapTests, ShallOverlapItself2)
{
    using lib_interval_tree::open;

    auto base = i<closed>(0, 5);
    EXPECT_TRUE(base.overlaps({0, 5}));

    auto base2 = i<open>(-5, 0);
    EXPECT_TRUE(base2.overlaps({-5, 0}));

    auto base3 = i<left_open>(-5, 0);
    EXPECT_TRUE(base3.overlaps({-5, 0}));

    auto base4 = i<right_open>(-5, 0);
    EXPECT_TRUE(base4.overlaps({-5, 0}));

    auto base5 = i<closed_adjacent>(-5, 0);
    EXPECT_TRUE(base4.overlaps({-5, 0}));
}

TEST_F(OverlapTests, RightOverlapTests)
{
    using lib_interval_tree::open;

    // large overlap
    EXPECT_TRUE(i<closed>(0, 5).overlaps({3, 16}));
    EXPECT_TRUE(i<open>(0, 5).overlaps({3, 16}));
    EXPECT_TRUE(i<left_open>(0, 5).overlaps({3, 16}));
    EXPECT_TRUE(i<right_open>(0, 5).overlaps({3, 16}));
    EXPECT_TRUE(i<closed_adjacent>(0, 5).overlaps({3, 16}));

    // large overlap, negative values:
    EXPECT_TRUE(i<closed>(-5, 5).overlaps({-3, 16}));
    EXPECT_TRUE(i<open>(-5, 5).overlaps({-3, 16}));
    EXPECT_TRUE(i<left_open>(-5, 5).overlaps({-3, 16}));
    EXPECT_TRUE(i<right_open>(-5, 5).overlaps({-3, 16}));
    EXPECT_TRUE(i<closed_adjacent>(-5, 5).overlaps({-3, 16}));

    // large overlap, completely negative:
    EXPECT_TRUE(i<closed>(-20, -10).overlaps({-13, -5}));
    EXPECT_TRUE(i<open>(-20, -10).overlaps({-13, -5}));
    EXPECT_TRUE(i<left_open>(-20, -10).overlaps({-13, -5}));
    EXPECT_TRUE(i<right_open>(-20, -10).overlaps({-13, -5}));
    EXPECT_TRUE(i<closed_adjacent>(-20, -10).overlaps({-13, -5}));

    // edge overlap
    EXPECT_TRUE(i<closed>(0, 5).overlaps({5, 16}));
    EXPECT_FALSE(i<open>(0, 5).overlaps({5, 16}));
    EXPECT_TRUE(i<left_open>(0, 5).overlaps({5, 16}));
    EXPECT_FALSE(i<right_open>(0, 5).overlaps({5, 16}));

    // edge overlap, negative values:
    EXPECT_TRUE(i<closed>(-5, 5).overlaps({5, 16}));
    EXPECT_FALSE(i<open>(-5, 5).overlaps({5, 16}));
    EXPECT_TRUE(i<left_open>(-5, 5).overlaps({5, 16}));
    EXPECT_FALSE(i<right_open>(-5, 5).overlaps({5, 16}));

    // edge overlap, completely negative:
    EXPECT_TRUE(i<closed>(-20, -10).overlaps({-10, -5}));
    EXPECT_FALSE(i<open>(-20, -10).overlaps({-10, -5}));
    EXPECT_TRUE(i<left_open>(-20, -10).overlaps({-10, -5}));
    EXPECT_FALSE(i<right_open>(-20, -10).overlaps({-10, -5}));

    // adjacent does overlap case:
    EXPECT_TRUE(i<closed_adjacent>(0, 5).overlaps({5, 16}));
    EXPECT_TRUE(i<closed_adjacent>(0, 5).overlaps({6, 16}));
    EXPECT_FALSE(i<closed_adjacent>(0, 5).overlaps({7, 16}));

    // adjacent does overlap case, negative values:
    EXPECT_TRUE(i<closed_adjacent>(-5, 5).overlaps({5, 16}));
    EXPECT_TRUE(i<closed_adjacent>(-5, 5).overlaps({6, 16}));
    EXPECT_FALSE(i<closed_adjacent>(-5, 5).overlaps({7, 16}));

    // adjacent does overlap case, completely negative:
    EXPECT_TRUE(i<closed_adjacent>(-20, -10).overlaps({-10, -5}));
    EXPECT_TRUE(i<closed_adjacent>(-20, -10).overlaps({-9, -5}));
    EXPECT_FALSE(i<closed_adjacent>(-20, -10).overlaps({-8, -5}));

    // no overlap
    EXPECT_FALSE(i<closed>(0, 5).overlaps({6, 16}));
    EXPECT_FALSE(i<open>(0, 5).overlaps({6, 16}));
    EXPECT_FALSE(i<left_open>(0, 5).overlaps({6, 16}));
    EXPECT_FALSE(i<right_open>(0, 5).overlaps({6, 16}));

    // no overlap negative values:
    EXPECT_FALSE(i<closed>(-5, 5).overlaps({6, 16}));
    EXPECT_FALSE(i<open>(-5, 5).overlaps({6, 16}));
    EXPECT_FALSE(i<left_open>(-5, 5).overlaps({6, 16}));
    EXPECT_FALSE(i<right_open>(-5, 5).overlaps({6, 16}));

    // no overlap completely negative:
    EXPECT_FALSE(i<closed>(-20, -10).overlaps({-8, -5}));
    EXPECT_FALSE(i<open>(-20, -10).overlaps({-8, -5}));
    EXPECT_FALSE(i<left_open>(-20, -10).overlaps({-8, -5}));
    EXPECT_FALSE(i<right_open>(-20, -10).overlaps({-8, -5}));
}

TEST_F(OverlapTests, LeftOverlapTests)
{
    using lib_interval_tree::open;

    // large overlap:
    EXPECT_TRUE(i<closed>(0, 5).overlaps({-3, 16}));
    EXPECT_TRUE(i<open>(0, 5).overlaps({-3, 16}));
    EXPECT_TRUE(i<left_open>(0, 5).overlaps({-3, 16}));
    EXPECT_TRUE(i<right_open>(0, 5).overlaps({-3, 16}));
    EXPECT_TRUE(i<closed_adjacent>(0, 5).overlaps({-3, 16}));

    // large overlap, negative values:
    EXPECT_TRUE(i<closed>(-5, 5).overlaps({-13, 16}));
    EXPECT_TRUE(i<open>(-5, 5).overlaps({-13, 16}));
    EXPECT_TRUE(i<left_open>(-5, 5).overlaps({-13, 16}));
    EXPECT_TRUE(i<right_open>(-5, 5).overlaps({-13, 16}));
    EXPECT_TRUE(i<closed_adjacent>(-5, 5).overlaps({-13, 16}));

    // large overlap, completely negative:
    EXPECT_TRUE(i<closed>(-20, -10).overlaps({-23, -5}));
    EXPECT_TRUE(i<open>(-20, -10).overlaps({-23, -5}));
    EXPECT_TRUE(i<left_open>(-20, -10).overlaps({-23, -5}));
    EXPECT_TRUE(i<right_open>(-20, -10).overlaps({-23, -5}));
    EXPECT_TRUE(i<closed_adjacent>(-20, -10).overlaps({-23, -5}));

    // edge overlap:
    EXPECT_TRUE(i<closed>(0, 5).overlaps({-3, 0}));
    EXPECT_FALSE(i<open>(0, 5).overlaps({-3, 0}));
    EXPECT_FALSE(i<left_open>(0, 5).overlaps({-3, 0}));
    EXPECT_TRUE(i<right_open>(0, 5).overlaps({-3, 0}));

    // edge overlap, negative values:
    EXPECT_TRUE(i<closed>(-5, 5).overlaps({-13, -5}));
    EXPECT_FALSE(i<open>(-5, 5).overlaps({-13, -5}));
    EXPECT_FALSE(i<left_open>(-5, 5).overlaps({-13, -5}));
    EXPECT_TRUE(i<right_open>(-5, 5).overlaps({-13, -5}));

    // edge overlap, completely negative:
    EXPECT_TRUE(i<closed>(-10, -5).overlaps({-23, -10}));
    EXPECT_FALSE(i<open>(-10, -5).overlaps({-23, -10}));
    EXPECT_FALSE(i<left_open>(-10, -5).overlaps({-23, -10}));
    EXPECT_TRUE(i<right_open>(-10, -5).overlaps({-23, -10}));

    // adjacent does overlap case:
    EXPECT_TRUE(i<closed_adjacent>(0, 5).overlaps({-3, 0}));
    EXPECT_TRUE(i<closed_adjacent>(0, 5).overlaps({-4, -1}));
    EXPECT_FALSE(i<closed_adjacent>(0, 5).overlaps({-5, -2}));

    // no overlap
    EXPECT_FALSE(i<closed>(0, 5).overlaps({-6, -1}));
    EXPECT_FALSE(i<open>(0, 5).overlaps({-6, -1}));
    EXPECT_FALSE(i<left_open>(0, 5).overlaps({-6, -1}));
    EXPECT_FALSE(i<right_open>(0, 5).overlaps({-6, -1}));
    EXPECT_FALSE(i<closed_adjacent>(0, 5).overlaps({-6, -2}));
}

TEST_F(OverlapTests, ShallEncompassCompletely)
{
    using lib_interval_tree::open;
    EXPECT_TRUE(i<closed>(0, 5).overlaps({-99, 16}));
    EXPECT_TRUE(i<open>(0, 5).overlaps({-99, 16}));
    EXPECT_TRUE(i<left_open>(0, 5).overlaps({-99, 16}));
    EXPECT_TRUE(i<right_open>(0, 5).overlaps({-99, 16}));
    EXPECT_TRUE(i<closed_adjacent>(0, 5).overlaps({-99, 16}));
}

TEST_F(OverlapTests, ShallBeContainedIn)
{
    using lib_interval_tree::open;
    EXPECT_TRUE(i<closed>(0, 5).overlaps({3, 4}));
    EXPECT_TRUE(i<open>(0, 5).overlaps({3, 4}));
    EXPECT_TRUE(i<left_open>(0, 5).overlaps({3, 4}));
    EXPECT_TRUE(i<right_open>(0, 5).overlaps({3, 4}));
    EXPECT_TRUE(i<closed_adjacent>(0, 5).overlaps({3, 4}));
}

TEST_F(OverlapTests, ShallNotOverlapExclusiveLeft)
{
    auto base = i<closed>(0, 5);
    EXPECT_FALSE(base.overlaps_exclusive({-7, 0}));
}

TEST_F(OverlapTests, ShallNotOverlapExclusiveRight)
{
    auto base = i<closed>(0, 5);
    EXPECT_FALSE(base.overlaps_exclusive({5, 10}));
}

TEST_F(OverlapTests, ShallOverlapExclusiveRight)
{
    auto base = i<closed>(0, 5);
    EXPECT_TRUE(base.overlaps_exclusive({4, 10}));
}

TEST_F(OverlapTests, ShallOverlapExclusiveLeft)
{
    auto base = i<closed>(0, 5);
    EXPECT_TRUE(base.overlaps_exclusive({-4, 2}));
}

TEST_F(OverlapTests, ShallOverlapExclusiveEncompass)
{
    auto base = i<closed>(0, 5);
    EXPECT_TRUE(base.overlaps_exclusive({-6, 10}));
}

TEST_F(OverlapTests, ShallOverlapExclusiveContained)
{
    auto base = i<closed>(0, 5);
    EXPECT_TRUE(base.overlaps_exclusive({1, 4}));
}

TEST_F(OverlapTests, ExpectDisjunctExclusive)
{
    auto base = i<closed>(0, 5);
    EXPECT_FALSE(base.overlaps_exclusive({99, 101}));
}

TEST_F(ContainmentTests, ShallSingleBeWithin)
{
    using lib_interval_tree::open;
    auto cl = i<closed>(-86, 35);
    EXPECT_TRUE(cl.within(3));
    EXPECT_TRUE(cl.within(-3));
    EXPECT_TRUE(cl.within(-86));
    EXPECT_TRUE(cl.within(35));
    EXPECT_FALSE(cl.within(36));
    EXPECT_FALSE(cl.within(-87));

    auto op = i<open>(-86, 35);
    EXPECT_TRUE(op.within(3));
    EXPECT_TRUE(op.within(-3));
    EXPECT_FALSE(op.within(-86));
    EXPECT_FALSE(op.within(35));
    EXPECT_FALSE(op.within(36));
    EXPECT_FALSE(op.within(-87));

    auto lo = i<left_open>(-86, 35);
    EXPECT_TRUE(lo.within(3));
    EXPECT_TRUE(lo.within(-3));
    EXPECT_FALSE(lo.within(-86));
    EXPECT_TRUE(lo.within(35));
    EXPECT_FALSE(lo.within(36));
    EXPECT_FALSE(lo.within(-87));

    auto ro = i<right_open>(-86, 35);
    EXPECT_TRUE(ro.within(3));
    EXPECT_TRUE(ro.within(-3));
    EXPECT_TRUE(ro.within(-86));
    EXPECT_FALSE(ro.within(35));
    EXPECT_FALSE(ro.within(36));
    EXPECT_FALSE(ro.within(-87));

    auto ca = i<closed_adjacent>(-86, 35);
    EXPECT_TRUE(ca.within(3));
    EXPECT_TRUE(ca.within(-3));
    EXPECT_TRUE(ca.within(-86));
    EXPECT_TRUE(ca.within(35));
    EXPECT_FALSE(ca.within(36));
    EXPECT_FALSE(ca.within(-87));
}

TEST_F(ContainmentTests, ExpectIntervalWithinOther)
{
    using lib_interval_tree::open;
    auto base = i<closed>(-100, 100);
    EXPECT_EQ(base.within({-23, 10}), true);
    EXPECT_EQ(base.within({-100, 100}), true);
    EXPECT_EQ(base.within({12, 30}), true);
    EXPECT_EQ(base.within({-73, -23}), true);
    EXPECT_EQ(base.within({-100, -100}), true);
    EXPECT_EQ(base.within({100, 100}), true);
    EXPECT_EQ(base.within({0, 0}), true);

    auto base2 = i<open>(-100, 100);
    EXPECT_TRUE(base2.within({-23, 10}));
    EXPECT_FALSE(base2.within({-100, 100}));
    EXPECT_FALSE(base2.within({-100, 50}));
    EXPECT_FALSE(base2.within({-50, 100}));

    auto base3 = i<left_open>(-100, 100);
    EXPECT_TRUE(base3.within({-23, 10}));
    EXPECT_FALSE(base3.within({-100, 100}));
    EXPECT_FALSE(base3.within({-100, 50}));
    EXPECT_TRUE(base3.within({-50, 100}));

    auto base4 = i<right_open>(-100, 100);
    EXPECT_TRUE(base4.within({-23, 10}));
    EXPECT_FALSE(base4.within({-100, 100}));
    EXPECT_TRUE(base4.within({-100, 50}));
    EXPECT_FALSE(base4.within({-50, 100}));

    auto base5 = i<closed_adjacent>(-100, 100);
    EXPECT_TRUE(base5.within({-23, 10}));
    EXPECT_TRUE(base5.within({-100, 100}));
    EXPECT_TRUE(base5.within({-100, 50}));
    EXPECT_TRUE(base5.within({-50, 100}));
}

TEST_F(ContainmentTests, ExpectIntervalNotWithinOther)
{
    auto base = i<closed>(-100, 100);
    EXPECT_EQ(base.within({-101, -100}), false);
    EXPECT_EQ(base.within({-100, 101}), false);
    EXPECT_EQ(base.within({-200, 0}), false);
    EXPECT_EQ(base.within({100, 102}), false);
    EXPECT_EQ(base.within({-200, -101}), false);
    EXPECT_EQ(base.within({200, 300}), false);
}

TEST_F(DistanceTests, DistanceIsZeroOnOverlap)
{
    auto base = i<closed>(-35, 96);
    auto other = i<closed>(-20, 600);
    EXPECT_EQ(base - other, 0);
}

TEST_F(DistanceTests, DistanceLeftSide)
{
    using lib_interval_tree::open;
    auto base = i<closed>(5, 10);
    auto other = i<closed>(0, 1);
    EXPECT_EQ(base - other, 4);

    auto base2 = i<open>(5, 10);
    auto other2 = i<open>(0, 1);
    EXPECT_EQ(base2 - other2, 4);

    auto base3 = i<left_open>(5, 10);
    auto other3 = i<left_open>(0, 1);
    EXPECT_EQ(base3 - other3, 4);

    auto base4 = i<right_open>(5, 10);
    auto other4 = i<right_open>(0, 1);
    EXPECT_EQ(base4 - other4, 4);

    auto base5 = i<closed_adjacent>(5, 10);
    auto other5 = i<closed_adjacent>(0, 1);

    EXPECT_EQ(base5 - other5, 4);
}

TEST_F(DistanceTests, DistanceRightSide)
{
    using lib_interval_tree::open;
    auto base = i<closed>(5, 10);
    auto other = i<closed>(15, 18);
    EXPECT_EQ(base - other, 5);

    auto base2 = i<open>(5, 10);
    auto other2 = i<open>(15, 18);
    EXPECT_EQ(base2 - other2, 5);

    auto base3 = i<left_open>(5, 10);
    auto other3 = i<left_open>(15, 18);
    EXPECT_EQ(base3 - other3, 5);

    auto base4 = i<right_open>(5, 10);
    auto other4 = i<right_open>(15, 18);
    EXPECT_EQ(base4 - other4, 5);

    auto base5 = i<closed_adjacent>(5, 10);
    auto other5 = i<closed_adjacent>(15, 18);
    EXPECT_EQ(base5 - other5, 5);
}

TEST_F(DistanceTests, DistanceAdjacent)
{
    using lib_interval_tree::open;
    auto base = i<closed>(5, 10);
    auto other = i<closed>(10, 18);
    EXPECT_EQ(base - other, 0);

    auto base2 = i<open>(5, 10);
    auto other2 = i<open>(10, 18);
    EXPECT_EQ(base2 - other2, 0);

    auto base3 = i<left_open>(5, 10);
    auto other3 = i<left_open>(10, 18);
    EXPECT_EQ(base3 - other3, 0);

    auto base4 = i<right_open>(5, 10);
    auto other4 = i<right_open>(10, 18);
    EXPECT_EQ(base4 - other4, 0);

    auto base5 = i<closed_adjacent>(5, 10);
    auto other5 = i<closed_adjacent>(10, 18);
    EXPECT_EQ(base5 - other5, 0);
}

TEST_F(DistanceTests, DistanceAdjacent2)
{
    using lib_interval_tree::open;
    auto base = i<closed>(5, 10);
    auto other = i<closed>(0, 5);
    EXPECT_EQ(base - other, 0);

    auto base2 = i<open>(5, 10);
    auto other2 = i<open>(0, 5);
    EXPECT_EQ(base2 - other2, 0);

    auto base3 = i<left_open>(5, 10);
    auto other3 = i<left_open>(0, 5);
    EXPECT_EQ(base3 - other3, 0);

    auto base4 = i<right_open>(5, 10);
    auto other4 = i<right_open>(0, 5);
    EXPECT_EQ(base4 - other4, 0);

    auto base5 = i<closed_adjacent>(5, 10);
    auto other5 = i<closed_adjacent>(0, 5);
    EXPECT_EQ(base5 - other5, 0);
}

TEST_F(OverlapTests, DynamicOverlapContainedCompletely)
{
    auto containment = i<dynamic>(-100, 100, interval_border::closed, interval_border::closed);

    for (interval_border l : {interval_border::closed, interval_border::open, interval_border::closed_adjacent})
    {
        for (interval_border r : {interval_border::closed, interval_border::open, interval_border::closed_adjacent})
        {
            EXPECT_TRUE(containment.overlaps(i<dynamic>(-100, 100, l, r)));
            EXPECT_TRUE(containment.overlaps(i<dynamic>(-10, 10, l, r)));
            EXPECT_TRUE(containment.overlaps(i<dynamic>(-200, 200, l, r)));
        }
    }
}

TEST_F(OverlapTests, DynamicBorderTests)
{
    constexpr auto c = interval_border::closed;
    constexpr auto o = interval_border::open;
    constexpr auto ca = interval_border::closed_adjacent;

    // right side:
    EXPECT_TRUE(i<dynamic>(0, 5, c, c).overlaps(i<dynamic>(5, 10, c, c)));
    EXPECT_TRUE(i<dynamic>(0, 5, c, c).overlaps(i<dynamic>(5, 10, c, o)));
    EXPECT_FALSE(i<dynamic>(0, 5, c, c).overlaps(i<dynamic>(5, 10, o, c)));
    EXPECT_FALSE(i<dynamic>(0, 5, c, c).overlaps(i<dynamic>(5, 10, o, o)));
    EXPECT_TRUE(i<dynamic>(0, 5, c, c).overlaps(i<dynamic>(5, 10, c, ca)));
    EXPECT_TRUE(i<dynamic>(0, 5, c, c).overlaps(i<dynamic>(5, 10, ca, c)));
    EXPECT_TRUE(i<dynamic>(0, 5, c, c).overlaps(i<dynamic>(5, 10, ca, ca)));
    EXPECT_TRUE(i<dynamic>(0, 5, c, c).overlaps(i<dynamic>(6, 10, ca, c)));
    EXPECT_FALSE(i<dynamic>(0, 5, c, c).overlaps(i<dynamic>(6, 10, c, c)));
    EXPECT_FALSE(i<dynamic>(0, 5, c, c).overlaps(i<dynamic>(6, 10, c, o)));
    EXPECT_FALSE(i<dynamic>(0, 5, c, c).overlaps(i<dynamic>(6, 10, o, c)));
    EXPECT_FALSE(i<dynamic>(0, 5, c, c).overlaps(i<dynamic>(6, 10, o, o)));

    // left side:
    EXPECT_TRUE(i<dynamic>(5, 10, c, c).overlaps(i<dynamic>(0, 5, c, c)));
    EXPECT_FALSE(i<dynamic>(5, 10, c, c).overlaps(i<dynamic>(0, 5, c, o)));
    EXPECT_TRUE(i<dynamic>(5, 10, c, c).overlaps(i<dynamic>(0, 5, o, c)));
    EXPECT_FALSE(i<dynamic>(5, 10, c, c).overlaps(i<dynamic>(0, 5, o, o)));
    EXPECT_TRUE(i<dynamic>(5, 10, c, c).overlaps(i<dynamic>(0, 5, c, ca)));
    EXPECT_TRUE(i<dynamic>(5, 10, c, c).overlaps(i<dynamic>(0, 5, ca, c)));
    EXPECT_TRUE(i<dynamic>(5, 10, c, c).overlaps(i<dynamic>(0, 5, ca, ca)));
    EXPECT_TRUE(i<dynamic>(5, 10, c, c).overlaps(i<dynamic>(0, 4, c, ca)));
    EXPECT_FALSE(i<dynamic>(5, 10, c, c).overlaps(i<dynamic>(0, 4, c, c)));
    EXPECT_FALSE(i<dynamic>(5, 10, c, c).overlaps(i<dynamic>(0, 4, c, o)));
    EXPECT_FALSE(i<dynamic>(5, 10, c, c).overlaps(i<dynamic>(0, 4, o, c)));
    EXPECT_FALSE(i<dynamic>(5, 10, c, c).overlaps(i<dynamic>(0, 4, o, o)));

    // right side, open:
    EXPECT_FALSE(i<dynamic>(0, 5, o, o).overlaps(i<dynamic>(5, 10, c, c)));
    EXPECT_FALSE(i<dynamic>(0, 5, o, o).overlaps(i<dynamic>(5, 10, c, o)));
    EXPECT_FALSE(i<dynamic>(0, 5, o, o).overlaps(i<dynamic>(5, 10, o, c)));
    EXPECT_FALSE(i<dynamic>(0, 5, o, o).overlaps(i<dynamic>(5, 10, o, o)));
    EXPECT_FALSE(i<dynamic>(0, 5, o, o).overlaps(i<dynamic>(5, 10, c, ca)));
    EXPECT_TRUE(i<dynamic>(0, 5, o, o).overlaps(i<dynamic>(5, 10, ca, c)));
    EXPECT_TRUE(i<dynamic>(0, 5, o, o).overlaps(i<dynamic>(5, 10, ca, ca)));
    EXPECT_FALSE(i<dynamic>(0, 5, o, o).overlaps(i<dynamic>(6, 10, ca, c)));
    EXPECT_FALSE(i<dynamic>(0, 5, o, o).overlaps(i<dynamic>(6, 10, c, c)));
    EXPECT_FALSE(i<dynamic>(0, 5, o, o).overlaps(i<dynamic>(6, 10, c, o)));
    EXPECT_FALSE(i<dynamic>(0, 5, o, o).overlaps(i<dynamic>(6, 10, o, c)));
    EXPECT_FALSE(i<dynamic>(0, 5, o, o).overlaps(i<dynamic>(6, 10, o, o)));
    EXPECT_TRUE(i<dynamic>(0, 6, o, o).overlaps(i<dynamic>(5, 10, c, c)));
    EXPECT_TRUE(i<dynamic>(0, 6, o, o).overlaps(i<dynamic>(5, 10, c, o)));
    EXPECT_FALSE(i<dynamic>(0, 6, o, o).overlaps(i<dynamic>(5, 10, o, c)));
    EXPECT_FALSE(i<dynamic>(0, 6, o, o).overlaps(i<dynamic>(5, 10, o, o)));
    EXPECT_TRUE(i<dynamic>(0, 6, o, o).overlaps(i<dynamic>(5, 10, c, ca)));
    EXPECT_TRUE(i<dynamic>(0, 6, o, o).overlaps(i<dynamic>(5, 10, ca, c)));
    EXPECT_TRUE(i<dynamic>(0, 6, o, o).overlaps(i<dynamic>(5, 10, ca, ca)));

    // left side, open:
    EXPECT_FALSE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 5, c, c)));
    EXPECT_FALSE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 5, c, o)));
    EXPECT_FALSE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 5, o, c)));
    EXPECT_FALSE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 5, o, o)));
    EXPECT_TRUE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 5, c, ca)));
    EXPECT_FALSE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 5, ca, c)));
    EXPECT_TRUE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 5, ca, ca)));
    EXPECT_FALSE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 4, c, ca)));
    EXPECT_FALSE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 4, c, c)));
    EXPECT_FALSE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 4, c, o)));
    EXPECT_FALSE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 4, o, c)));
    EXPECT_FALSE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 4, o, o)));
    EXPECT_TRUE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 6, c, c)));
    EXPECT_FALSE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 6, c, o)));
    EXPECT_TRUE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 6, o, c)));
    EXPECT_FALSE(i<dynamic>(5, 10, o, o).overlaps(i<dynamic>(0, 6, o, o)));

    // one side open or closed follows from other tests
}

TEST_F(IntervalTests, DynamicWithinTest)
{
    auto base = i<dynamic>(-100, 100, interval_border::closed, interval_border::closed);
    EXPECT_TRUE(base.within(0));
    EXPECT_TRUE(base.within(-100));
    EXPECT_TRUE(base.within(100));
    EXPECT_FALSE(base.within(-101));
    EXPECT_FALSE(base.within(101));

    auto base2 = i<dynamic>(-100, 100, interval_border::open, interval_border::open);
    EXPECT_TRUE(base2.within(0));
    EXPECT_FALSE(base2.within(-100));
    EXPECT_FALSE(base2.within(100));
    EXPECT_FALSE(base2.within(-101));
    EXPECT_FALSE(base2.within(101));

    auto base3 = i<dynamic>(-100, 100, interval_border::open, interval_border::closed);
    EXPECT_TRUE(base3.within(0));
    EXPECT_FALSE(base3.within(-100));
    EXPECT_TRUE(base3.within(100));
    EXPECT_FALSE(base3.within(-101));
    EXPECT_FALSE(base3.within(101));

    auto base4 = i<dynamic>(-100, 100, interval_border::closed, interval_border::open);
    EXPECT_TRUE(base4.within(0));
    EXPECT_TRUE(base4.within(-100));
    EXPECT_FALSE(base4.within(100));
    EXPECT_FALSE(base4.within(-101));
    EXPECT_FALSE(base4.within(101));

    auto base5 = i<dynamic>(-100, 100, interval_border::closed, interval_border::closed_adjacent);
    EXPECT_TRUE(base5.within(0));
    EXPECT_TRUE(base5.within(-100));
    EXPECT_TRUE(base5.within(100));
    EXPECT_FALSE(base5.within(-101));
    EXPECT_FALSE(base5.within(101));

    auto base6 = i<dynamic>(-100, 100, interval_border::closed_adjacent, interval_border::closed);
    EXPECT_TRUE(base6.within(0));
    EXPECT_TRUE(base6.within(-100));
    EXPECT_TRUE(base6.within(100));
    EXPECT_FALSE(base6.within(-101));
    EXPECT_FALSE(base6.within(101));

    auto base7 = i<dynamic>(-100, 100, interval_border::closed_adjacent, interval_border::closed_adjacent);
    EXPECT_TRUE(base7.within(0));
    EXPECT_TRUE(base7.within(-100));
    EXPECT_TRUE(base7.within(100));
    EXPECT_FALSE(base7.within(-101));
    EXPECT_FALSE(base7.within(101));
}

TEST_F(IntervalTests, DynamicJoinTestBorderPromotion)
{
    EXPECT_EQ(
        i<dynamic>(-100, 100, interval_border::closed, interval_border::closed)
            .join(i<dynamic>(-100, 100, interval_border::closed, interval_border::closed)),
        i<dynamic>(-100, 100, interval_border::closed, interval_border::closed)
    );

    EXPECT_EQ(
        i<dynamic>(-100, 100, interval_border::closed, interval_border::closed)
            .join(i<dynamic>(-100, 100, interval_border::open, interval_border::open)),
        i<dynamic>(-100, 100, interval_border::closed, interval_border::closed)
    );

    EXPECT_EQ(
        i<dynamic>(-100, 100, interval_border::closed, interval_border::closed)
            .join(i<dynamic>(-100, 100, interval_border::open, interval_border::closed)),
        i<dynamic>(-100, 100, interval_border::closed, interval_border::closed)
    );

    EXPECT_EQ(
        i<dynamic>(-100, 100, interval_border::closed, interval_border::closed)
            .join(i<dynamic>(-100, 100, interval_border::closed, interval_border::open)),
        i<dynamic>(-100, 100, interval_border::closed, interval_border::closed)
    );

    EXPECT_EQ(
        i<dynamic>(-100, 100, interval_border::closed, interval_border::closed)
            .join(i<dynamic>(-100, 100, interval_border::closed, interval_border::closed_adjacent)),
        i<dynamic>(-100, 100, interval_border::closed, interval_border::closed_adjacent)
    );

    EXPECT_EQ(
        i<dynamic>(-100, 100, interval_border::closed, interval_border::closed)
            .join(i<dynamic>(-100, 100, interval_border::closed_adjacent, interval_border::closed)),
        i<dynamic>(-100, 100, interval_border::closed_adjacent, interval_border::closed)
    );

    EXPECT_EQ(
        i<dynamic>(-100, 100, interval_border::closed, interval_border::closed)
            .join(i<dynamic>(-100, 100, interval_border::closed_adjacent, interval_border::closed_adjacent)),
        i<dynamic>(-100, 100, interval_border::closed_adjacent, interval_border::closed_adjacent)
    );

    EXPECT_EQ(
        i<dynamic>(-100, 100, interval_border::closed, interval_border::closed)
            .join(i<dynamic>(-100, 100, interval_border::closed_adjacent, interval_border::open)),
        i<dynamic>(-100, 100, interval_border::closed_adjacent, interval_border::closed)
    );

    EXPECT_EQ(
        i<dynamic>(-100, 100, interval_border::closed, interval_border::closed)
            .join(i<dynamic>(-100, 100, interval_border::open, interval_border::closed_adjacent)),
        i<dynamic>(-100, 100, interval_border::closed, interval_border::closed_adjacent)
    );
}

TEST_F(IntervalTests, DynamicJoinTest)
{
    EXPECT_EQ(
        i<dynamic>(-50, 100, interval_border::closed, interval_border::open)
            .join(i<dynamic>(-100, 50, interval_border::open, interval_border::open)),
        i<dynamic>(-100, 100, interval_border::open, interval_border::open)
    );

    EXPECT_EQ(
        i<dynamic>(-50, 100, interval_border::open, interval_border::open)
            .join(i<dynamic>(-100, 50, interval_border::closed, interval_border::open)),
        i<dynamic>(-100, 100, interval_border::closed, interval_border::open)
    );

    EXPECT_EQ(
        i<dynamic>(-50, 100, interval_border::open, interval_border::open)
            .join(i<dynamic>(-100, 50, interval_border::open, interval_border::open)),
        i<dynamic>(-100, 100, interval_border::open, interval_border::open)
    );

    EXPECT_EQ(
        i<dynamic>(-50, 100, interval_border::open, interval_border::open)
            .join(i<dynamic>(-100, 50, interval_border::closed, interval_border::closed)),
        i<dynamic>(-100, 100, interval_border::closed, interval_border::open)
    );

    EXPECT_EQ(
        i<dynamic>(-50, 100, interval_border::open, interval_border::open)
            .join(i<dynamic>(-100, 50, interval_border::closed, interval_border::closed_adjacent)),
        i<dynamic>(-100, 100, interval_border::closed, interval_border::open)
    );

    EXPECT_EQ(
        i<dynamic>(-50, 100, interval_border::open, interval_border::open)
            .join(i<dynamic>(-100, 50, interval_border::closed_adjacent, interval_border::closed)),
        i<dynamic>(-100, 100, interval_border::closed_adjacent, interval_border::open)
    );

    EXPECT_EQ(
        i<dynamic>(-50, 100, interval_border::closed_adjacent, interval_border::open)
            .join(i<dynamic>(-100, 50, interval_border::open, interval_border::closed_adjacent)),
        i<dynamic>(-100, 100, interval_border::open, interval_border::open)
    );

    EXPECT_EQ(
        i<dynamic>(0, 5, interval_border::open, interval_border::open)
            .join(i<dynamic>(10, 15, interval_border::closed_adjacent, interval_border::closed)),
        i<dynamic>(0, 15, interval_border::open, interval_border::closed)
    );

    EXPECT_EQ(
        i<dynamic>(0, 5, interval_border::open, interval_border::closed)
            .join(i<dynamic>(0, 1, interval_border::closed, interval_border::open)),
        i<dynamic>(0, 5, interval_border::closed, interval_border::closed)
    );

    // left open adjusted test
    EXPECT_EQ(
        i<dynamic>(0, 5, interval_border::open, interval_border::closed)
            .join(i<dynamic>(1, 2, interval_border::closed, interval_border::open)),
        i<dynamic>(1, 5, interval_border::closed, interval_border::closed)
    );
    EXPECT_EQ(
        i<dynamic>(0, 5, interval_border::open, interval_border::closed)
            .join(i<dynamic>(1, 2, interval_border::closed_adjacent, interval_border::open)),
        i<dynamic>(1, 5, interval_border::closed_adjacent, interval_border::closed)
    );

    // right open adjust test
    EXPECT_EQ(
        i<dynamic>(0, 5, interval_border::closed, interval_border::open)
            .join(i<dynamic>(3, 4, interval_border::closed, interval_border::closed)),
        i<dynamic>(0, 4, interval_border::closed, interval_border::closed)
    );
    EXPECT_EQ(
        i<dynamic>(0, 5, interval_border::closed, interval_border::open)
            .join(i<dynamic>(3, 4, interval_border::closed, interval_border::closed_adjacent)),
        i<dynamic>(0, 4, interval_border::closed, interval_border::closed_adjacent)
    );
}

TEST_F(IntervalTests, CanInsertDynamicIntervalIntoTree)
{
    interval_tree<interval<int, dynamic>> dynamic_interval_tree;
    auto iter = dynamic_interval_tree.insert(i<dynamic>(0, 5, interval_border::closed, interval_border::closed));
    EXPECT_EQ(iter->low(), 0);
    EXPECT_EQ(iter->high(), 5);
    EXPECT_EQ(iter->left_border(), interval_border::closed);
    EXPECT_EQ(iter->right_border(), interval_border::closed);
    EXPECT_EQ(dynamic_interval_tree.begin(), iter);
}

TEST_F(IntervalTests, CanFindDynamicIntervalInTree)
{
    interval_tree<interval<int, dynamic>> dynamic_interval_tree;
    dynamic_interval_tree.insert(i<dynamic>(0, 5, interval_border::closed, interval_border::closed));
    auto iter = dynamic_interval_tree.find(i<dynamic>(0, 5, interval_border::closed, interval_border::closed));
    EXPECT_EQ(iter->low(), 0);
    EXPECT_EQ(iter->high(), 5);
    EXPECT_EQ(iter->left_border(), interval_border::closed);
    EXPECT_EQ(iter->right_border(), interval_border::closed);
}

TEST_F(IntervalTests, CanEraseDynamicIntervalFromTree)
{
    interval_tree<interval<int, dynamic>> dynamic_interval_tree;
    dynamic_interval_tree.insert(i<dynamic>(0, 5, interval_border::closed, interval_border::closed));
    dynamic_interval_tree.erase(dynamic_interval_tree.begin());
    EXPECT_TRUE(dynamic_interval_tree.empty());
}

TEST_F(IntervalTests, CanOverlapFindDynamicIntervalInTree)
{
    interval_tree<interval<int, dynamic>> dynamic_interval_tree;
    dynamic_interval_tree.insert(i<dynamic>(0, 5, interval_border::closed, interval_border::closed));
    auto iter =
        dynamic_interval_tree.overlap_find(i<dynamic>(3, 7, interval_border::open, interval_border::closed_adjacent));
    EXPECT_EQ(iter->low(), 0);
    EXPECT_EQ(iter->high(), 5);
    EXPECT_EQ(iter->left_border(), interval_border::closed);
    EXPECT_EQ(iter->right_border(), interval_border::closed);
}

TEST_F(IntervalTests, CanInsertOverlappingDynamicIntervalIntoTree)
{
    interval_tree<interval<int, dynamic>> dynamic_interval_tree;
    dynamic_interval_tree.insert(i<dynamic>(0, 5, interval_border::closed, interval_border::closed));
    dynamic_interval_tree.insert_overlap(i<dynamic>(3, 7, interval_border::open, interval_border::closed_adjacent));
    auto iter = dynamic_interval_tree.find(i<dynamic>(0, 7, interval_border::closed, interval_border::closed_adjacent));
    EXPECT_EQ(iter->low(), 0);
    EXPECT_EQ(iter->high(), 7);
    EXPECT_EQ(iter->left_border(), interval_border::closed);
    EXPECT_EQ(iter->right_border(), interval_border::closed_adjacent);
}

TEST_F(IntervalTests, CanFindDynamicIntervalUsingComparisonFunction)
{
    interval_tree<interval<int, dynamic>> dynamic_interval_tree;
    dynamic_interval_tree.insert(i<dynamic>(0, 5, interval_border::closed, interval_border::closed));
    auto iter = dynamic_interval_tree.find(
        i<dynamic>(0, 5, interval_border::open, interval_border::open),
        // ignore border types:
        [](const auto& a, const auto& b) {
            return a.low() == b.low() && a.high() == b.high();
        }
    );
    EXPECT_EQ(iter->low(), 0);
    EXPECT_EQ(iter->high(), 5);
    EXPECT_EQ(iter->left_border(), interval_border::closed);
    EXPECT_EQ(iter->right_border(), interval_border::closed);
}

TEST_F(IntervalTests, ClosedAdjacentSliceLeftOverlap)
{
    // [   this  ]
    // [ls][param]
    const auto result = i<closed_adjacent>(1, 8).slice(i<closed_adjacent>(5, 100));
    ASSERT_TRUE(result.left_slice);
    EXPECT_EQ(result.left_slice->low(), 1);
    EXPECT_EQ(result.left_slice->high(), 4);
    EXPECT_FALSE(result.right_slice);
}

TEST_F(IntervalTests, ClosedSliceLeftOverlap)
{
    // [   this  ]
    // [ls][param]
    const auto result = i<closed>(1, 8).slice(i<closed>(5, 100));
    ASSERT_TRUE(result.left_slice);
    EXPECT_EQ(result.left_slice->low(), 1);
    EXPECT_EQ(result.left_slice->high(), 5);
    EXPECT_FALSE(result.right_slice);
}

TEST_F(IntervalTests, OpenSliceLeftOverlap)
{
    // [   this  ]
    // [ls][param]
    using lib_interval_tree::open;
    const auto result = i<open>(1, 8).slice(i<open>(5, 10));
    ASSERT_TRUE(result.left_slice);
    EXPECT_EQ(result.left_slice->low(), 1);
    EXPECT_EQ(result.left_slice->high(), 5);
    EXPECT_FALSE(result.right_slice);
}

TEST_F(IntervalTests, ClosedAdjacentRightRemains)
{
    // [   this  ]
    // [param][rs]
    const auto result = i<closed_adjacent>(8, 15).slice(i<closed_adjacent>(8, 12));
    ASSERT_TRUE(result.right_slice);
    EXPECT_EQ(result.right_slice->low(), 13);
    EXPECT_EQ(result.right_slice->high(), 15);
    EXPECT_FALSE(result.left_slice);
}

TEST_F(IntervalTests, ClosedRightRemains)
{
    // [   this  ]
    // [param][rs]
    using lib_interval_tree::closed;
    const auto result = i<closed>(8, 15).slice(i<closed>(8, 12));
    ASSERT_TRUE(result.right_slice);
    EXPECT_EQ(result.right_slice->low(), 12);
    EXPECT_EQ(result.right_slice->high(), 15);
    EXPECT_FALSE(result.left_slice);
}

TEST_F(IntervalTests, OpenRightRemains)
{
    // [   this  ]
    // [param][rs]
    using lib_interval_tree::open;
    const auto result = i<open>(8, 15).slice(i<open>(8, 12));
    ASSERT_TRUE(result.right_slice);
    EXPECT_EQ(result.right_slice->low(), 12);
    EXPECT_EQ(result.right_slice->high(), 15);
    EXPECT_FALSE(result.left_slice);
}

TEST_F(IntervalTests, ClosedAdjacentMiddleExtrusion)
{
    // [   this      ]
    // [ls][param][rs]
    const auto result = i<closed_adjacent>(0, 10).slice(i<closed_adjacent>(5, 8));
    ASSERT_TRUE(result.left_slice);
    EXPECT_EQ(result.left_slice->low(), 0);
    EXPECT_EQ(result.left_slice->high(), 4);
    ASSERT_TRUE(result.right_slice);
    EXPECT_EQ(result.right_slice->low(), 9);
    EXPECT_EQ(result.right_slice->high(), 10);
}

TEST_F(IntervalTests, OpenMiddleExtrusion)
{
    // [   this      ]
    // [ls][param][rs]
    using lib_interval_tree::open;
    const auto result = i<open>(0, 10).slice(i<open>(5, 8));
    ASSERT_TRUE(result.left_slice);
    EXPECT_EQ(result.left_slice->low(), 0);
    EXPECT_EQ(result.left_slice->high(), 5);
    ASSERT_TRUE(result.right_slice);
    EXPECT_EQ(result.right_slice->low(), 8);
    EXPECT_EQ(result.right_slice->high(), 10);
}

TEST_F(IntervalTests, ClosedMiddleExtrusion)
{
    // [   this      ]
    // [ls][param][rs]
    using lib_interval_tree::closed;
    const auto result = i<closed>(0, 10).slice(i<closed>(5, 8));
    ASSERT_TRUE(result.left_slice);
    EXPECT_EQ(result.left_slice->low(), 0);
    EXPECT_EQ(result.left_slice->high(), 5);
    ASSERT_TRUE(result.right_slice);
    EXPECT_EQ(result.right_slice->low(), 8);
    EXPECT_EQ(result.right_slice->high(), 10);
}