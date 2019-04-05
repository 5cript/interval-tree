#pragma once

#include <boost/preprocessor/comma.hpp>

#include <limits>

class IntervalTests
    : public ::testing::Test
{
public:
    using types = IntervalTypes <int>;
};

class OverlapTests
    : public ::testing::Test
{
public:
    using types = IntervalTypes <int>;
};

class ContainmentTests
    : public ::testing::Test
{
public:
    using types = IntervalTypes <int>;
};

class DistanceTests
    : public ::testing::Test
{
public:
    using types = IntervalTypes <int>;
};

TEST_F(IntervalTests, FailBadBorders)
{
    ASSERT_DEATH(({
        [[maybe_unused]] auto ival = types::interval_type{1 BOOST_PP_COMMA() 0};
    }), "low <= high");
}

TEST_F(IntervalTests, ShallCreateInterval)
{
    auto ival = types::interval_type{1, 24};
    EXPECT_EQ(ival.low(), 1);
    EXPECT_EQ(ival.high(), 24);
}

TEST_F(IntervalTests, ShallCreateInterval2)
{
    auto ival = types::interval_type{-23, 24};
    EXPECT_EQ(ival.low(), -23);
    EXPECT_EQ(ival.high(), 24);
}

TEST_F(IntervalTests, ShallCreateInterval3)
{
    auto ival = types::interval_type{-21, -12};
    EXPECT_EQ(ival.low(), -21);
    EXPECT_EQ(ival.high(), -12);
}

TEST_F(IntervalTests, ShallCreateInterval4)
{
    auto ival = types::interval_type{1, 24};
    EXPECT_EQ(ival.low(), 1);
    EXPECT_EQ(ival.high(), 24);
}

TEST_F(IntervalTests, ShallCreateInterval5)
{
    auto ival = types::interval_type{1, 1};
    EXPECT_EQ(ival.low(), 1);
    EXPECT_EQ(ival.high(), 1);
}

TEST_F(IntervalTests, TestLimits)
{
    auto ival = types::interval_type{std::numeric_limits<types::value_type>::min(), std::numeric_limits<types::value_type>::max()};
    EXPECT_EQ(ival.low(), std::numeric_limits<types::value_type>::min());
    EXPECT_EQ(ival.high(), std::numeric_limits<types::value_type>::max());
}

TEST_F(OverlapTests, ShallOverlapItself)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps(base), true);
}

TEST_F(OverlapTests, ShallOverlapItself2)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({0, 5}), true);
}

TEST_F(OverlapTests, ShallOverlapRight)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({3, 16}), true);
}

TEST_F(OverlapTests, ShallOverlapLeft)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({-8, 1}), true);
}

TEST_F(OverlapTests, ShallEncompassCompletely)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({-99, 16}), true);
}

TEST_F(OverlapTests, ShallBeContainedIn)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({3, 4}), true);
}

TEST_F(OverlapTests, ExpectDisjunct)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({7, 19}), false);
}

TEST_F(OverlapTests, ShallBarelyOverlapLeft)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({-3, 0}), true);
}

TEST_F(OverlapTests, ShallBarelyOverlapRight)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({5, 10}), true);
}

TEST_F(OverlapTests, ShallNotOverlapExclusiveLeft)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps_exclusive({-7, 0}), false);
}

TEST_F(OverlapTests, ShallNotOverlapExclusiveRight)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps_exclusive({5, 10}), false);
}

TEST_F(OverlapTests, ShallOverlapExclusiveRight)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps_exclusive({4, 10}), true);
}

TEST_F(OverlapTests, ShallOverlapExclusiveLeft)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps_exclusive({-4, 2}), true);
}

TEST_F(OverlapTests, ShallOverlapExclusiveEncompass)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps_exclusive({-6, 10}), true);
}

TEST_F(OverlapTests, ShallOverlapExclusiveContained)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps_exclusive({1, 4}), true);
}

TEST_F(OverlapTests, ExpectDisjunctExclusive)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps_exclusive({99, 101}), false);
}

TEST_F(ContainmentTests, ShallSingleBeWithin)
{
    auto base = types::interval_type{-86, 35};
    EXPECT_EQ(base.within(3), true);
    EXPECT_EQ(base.within(-3), true);
    EXPECT_EQ(base.within(-86), true);
    EXPECT_EQ(base.within(35), true);
}

TEST_F(ContainmentTests, ExpectIntervalWithinOther)
{
    auto base = types::interval_type{-100, 100};
    EXPECT_EQ(base.within({-23, 10}), true);
    EXPECT_EQ(base.within({-100, 100}), true);
    EXPECT_EQ(base.within({12, 30}), true);
    EXPECT_EQ(base.within({-73, -23}), true);
    EXPECT_EQ(base.within({-100, -100}), true);
    EXPECT_EQ(base.within({100, 100}), true);
    EXPECT_EQ(base.within({0, 0}), true);
}

TEST_F(ContainmentTests, ExpectIntervalNotWithinOther)
{
    auto base = types::interval_type{-100, 100};
    EXPECT_EQ(base.within({-101, -100}), false);
    EXPECT_EQ(base.within({-100, 101}), false);
    EXPECT_EQ(base.within({-200, 0}), false);
    EXPECT_EQ(base.within({100, 102}), false);
    EXPECT_EQ(base.within({-200, -101}), false);
    EXPECT_EQ(base.within({200, 300}), false);
}

TEST_F(DistanceTests, DistanceIsZeroOnOverlap)
{
    auto base = types::interval_type{-35, 96};

    auto other = types::interval_type{-20, 600};
    EXPECT_EQ(base - other, 0);
}
