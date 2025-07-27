#pragma once

#include <interval-tree/interval_tree.hpp>

#include <limits>

class PunchTests : public ::testing::Test
{
  public:
    template <typename value_type>
    struct closed
    {
        using interval_type = lib_interval_tree::interval<value_type, lib_interval_tree::closed>;
        using tree_type = lib_interval_tree::interval_tree<interval_type>;
        using iterator_type = typename tree_type::iterator;
    };
    template <typename value_type>
    struct open
    {
        using interval_type = lib_interval_tree::interval<value_type, lib_interval_tree::open>;
        using tree_type = lib_interval_tree::interval_tree<interval_type>;
        using iterator_type = typename tree_type::iterator;
    };
    template <typename value_type>
    struct left_open
    {
        using interval_type = lib_interval_tree::interval<value_type, lib_interval_tree::left_open>;
        using tree_type = lib_interval_tree::interval_tree<interval_type>;
        using iterator_type = typename tree_type::iterator;
    };
    template <typename value_type>
    struct right_open
    {
        using interval_type = lib_interval_tree::interval<value_type, lib_interval_tree::right_open>;
        using tree_type = lib_interval_tree::interval_tree<interval_type>;
        using iterator_type = typename tree_type::iterator;
    };
    template <typename value_type>
    struct closed_adjacent
    {
        using interval_type = lib_interval_tree::interval<value_type, lib_interval_tree::closed_adjacent>;
        using tree_type = lib_interval_tree::interval_tree<interval_type>;
        using iterator_type = typename tree_type::iterator;
    };
    template <typename value_type>
    struct dynamic
    {
        using interval_type = lib_interval_tree::interval<value_type, lib_interval_tree::dynamic>;
        using tree_type = lib_interval_tree::interval_tree<interval_type>;
        using iterator_type = typename tree_type::iterator;
    };

    template <typename Kind, typename NumericalT>
    auto i(NumericalT l, NumericalT h) const
    {
        return lib_interval_tree::interval<NumericalT, Kind>{l, h};
    }

    template <typename Kind, typename NumericalT>
    auto
    i(NumericalT l,
      NumericalT h,
      lib_interval_tree::interval_border l_border,
      lib_interval_tree::interval_border r_border) const
    {
        return lib_interval_tree::interval<NumericalT, Kind>{l, h, l_border, r_border};
    }
};

TEST_F(PunchTests, PunchEmptyTree)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    auto result = tree.punch(types::interval_type{0, 5});
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), 0);
    EXPECT_EQ(result.begin()->high(), 5);
}

TEST_F(PunchTests, PunchFullyRightOfTree)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    auto result = tree.punch(types::interval_type{20, 25});
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), 20);
    EXPECT_EQ(result.begin()->high(), 25);
}

TEST_F(PunchTests, PunchFullyLeftOfTree)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    auto result = tree.punch(types::interval_type{-10, -5});

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), -10);
    EXPECT_EQ(result.begin()->high(), -5);
}

TEST_F(PunchTests, PunchAjdacentLeftOfClosedTree)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    auto result = tree.punch(types::interval_type{-10, 0});

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), -10);
    EXPECT_EQ(result.begin()->high(), -1);
}

TEST_F(PunchTests, PunchAjdacentLeftOfOpenTree)
{
    using types = open<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    auto result = tree.punch(types::interval_type{-10, 0});

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), -10);
    EXPECT_EQ(result.begin()->high(), 0);
}

TEST_F(PunchTests, PunchAjdacentLeftOfLeftOpenTree)
{
    using types = left_open<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    auto result = tree.punch(types::interval_type{-10, 0});

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), -10);
    EXPECT_EQ(result.begin()->high(), 0);
}

TEST_F(PunchTests, PunchAjdacentLeftOfRightOpenTree)
{
    using types = left_open<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    auto result = tree.punch(types::interval_type{-10, 0});

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), -10);
    EXPECT_EQ(result.begin()->high(), 0);
}

TEST_F(PunchTests, PunchOverlapsLeftOfTreeLeftHanging)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    auto result = tree.punch(types::interval_type{-10, 2});

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), -10);
    EXPECT_EQ(result.begin()->high(), -1);
}

TEST_F(PunchTests, PunchOverlapsLeftOfTreeFullyExact)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    auto result = tree.punch(types::interval_type{-10, 5});

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), -10);
    EXPECT_EQ(result.begin()->high(), -1);
}

TEST_F(PunchTests, PunchOverlapsLeftOfTreeRightFullyOverNoGap)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    auto result = tree.punch(types::interval_type{-10, 7});

    ASSERT_EQ(result.size(), 1);
    auto iter = result.begin();

    EXPECT_EQ(iter->low(), -10);
    EXPECT_EQ(iter->high(), -1);
}

TEST_F(PunchTests, PunchOverlapsLeftOfTreeRightFullyOver)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{10, 15});
    auto result = tree.punch(types::interval_type{-10, 7});

    ASSERT_EQ(result.size(), 2);
    auto iter = result.begin();

    EXPECT_EQ(iter->low(), -10);
    EXPECT_EQ(iter->high(), -1);

    EXPECT_EQ((++iter)->low(), 6);
    EXPECT_EQ(iter->high(), 7);
}

TEST_F(PunchTests, PunchOverlapsLeftOfTreeFullyInsideInterval)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    auto result = tree.punch(types::interval_type{1, 3});

    EXPECT_TRUE(result.empty());
}

TEST_F(PunchTests, PunchOverlapsMiddleOfTreeFullyInsideInterval)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    tree.insert(types::interval_type{10, 15});
    auto result = tree.punch(types::interval_type{6, 9});

    EXPECT_TRUE(result.empty());
}

TEST_F(PunchTests, PunchOverlapsRightOfTreeFullyInsideInterval)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    auto result = tree.punch(types::interval_type{6, 9});

    // Expect nothing
    EXPECT_TRUE(result.empty());
}

TEST_F(PunchTests, ClosedPunchOverhangsLastIntervalOnRight)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    auto result = tree.punch(types::interval_type{8, 15});

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), 11);
    EXPECT_EQ(result.begin()->high(), 15);
}

TEST_F(PunchTests, OpenPunchOverhangsLastIntervalOnRight)
{
    using types = open<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    auto result = tree.punch(types::interval_type{8, 12});

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), 10);
    EXPECT_EQ(result.begin()->high(), 12);
}

TEST_F(PunchTests, LeftOpenPunchOverhangsLastIntervalOnRight)
{
    using types = left_open<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    auto result = tree.punch(types::interval_type{8, 12});

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), 10);
    EXPECT_EQ(result.begin()->high(), 12);
}

TEST_F(PunchTests, RightOpenPunchOverhangsLastIntervalOnRight)
{
    using types = right_open<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{5, 10});
    auto result = tree.punch(types::interval_type{8, 12});

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), 10);
    EXPECT_EQ(result.begin()->high(), 12);
}

TEST_F(PunchTests, PunchSingleElementTreeEncompassing)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    auto result = tree.punch(types::interval_type{0, 5});

    EXPECT_TRUE(result.empty());
}

TEST_F(PunchTests, PunchSingleElementTreeEncompassingWithRightOverlap)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    auto result = tree.punch(types::interval_type{0, 6});

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), 6);
    EXPECT_EQ(result.begin()->high(), 6);
}

TEST_F(PunchTests, PunchSingleElementTreeEncompassingWithLeftOverlap)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    auto result = tree.punch(types::interval_type{-1, 5});

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), -1);
    EXPECT_EQ(result.begin()->high(), -1);
}

TEST_F(PunchTests, DynamicPunchOverhangsLastIntervalOnRight)
{
    using namespace lib_interval_tree;

    using types = dynamic<int>;

    auto tree_closed = types::tree_type{};
    tree_closed.insert(types::interval_type{0, 5, interval_border::closed, interval_border::closed});
    tree_closed.insert(types::interval_type{5, 10, interval_border::closed, interval_border::closed});
    auto result_closed =
        tree_closed.punch(types::interval_type{8, 12, interval_border::closed, interval_border::closed});
    EXPECT_EQ(result_closed.size(), 1);
    EXPECT_EQ(result_closed.begin()->low(), 10);
    EXPECT_EQ(result_closed.begin()->left_border(), interval_border::open);
    EXPECT_EQ(result_closed.begin()->high(), 12);

    auto tree_open = types::tree_type{};
    tree_open.insert(types::interval_type{0, 5, interval_border::open, interval_border::open});
    tree_open.insert(types::interval_type{5, 10, interval_border::open, interval_border::open});
    auto result_open = tree_open.punch(types::interval_type{8, 12, interval_border::open, interval_border::open});
    EXPECT_EQ(result_open.size(), 1);
    EXPECT_EQ(result_open.begin()->low(), 10);
    EXPECT_EQ(result_open.begin()->left_border(), interval_border::closed);
    EXPECT_EQ(result_open.begin()->high(), 12);

    auto tree_left_open = types::tree_type{};
    tree_left_open.insert(types::interval_type{0, 5, interval_border::open, interval_border::closed});
    tree_left_open.insert(types::interval_type{5, 10, interval_border::open, interval_border::closed});
    auto result_left_open =
        tree_left_open.punch(types::interval_type{8, 12, interval_border::open, interval_border::closed});
    EXPECT_EQ(result_left_open.size(), 1);
    EXPECT_EQ(result_left_open.begin()->low(), 10);
    EXPECT_EQ(result_left_open.begin()->left_border(), interval_border::open);
    EXPECT_EQ(result_left_open.begin()->high(), 12);

    auto tree_right_open = types::tree_type{};
    tree_right_open.insert(types::interval_type{0, 5, interval_border::closed, interval_border::open});
    tree_right_open.insert(types::interval_type{5, 10, interval_border::closed, interval_border::open});
    auto result_right_open =
        tree_right_open.punch(types::interval_type{8, 12, interval_border::closed, interval_border::open});
    EXPECT_EQ(result_right_open.size(), 1);
    EXPECT_EQ(result_right_open.begin()->low(), 10);
    EXPECT_EQ(result_right_open.begin()->left_border(), interval_border::closed);
    EXPECT_EQ(result_right_open.begin()->high(), 12);

    auto tree_closed_adjacent = types::tree_type{};
    tree_closed_adjacent.insert(
        types::interval_type{0, 5, interval_border::closed_adjacent, interval_border::closed_adjacent}
    );
    tree_closed_adjacent.insert(
        types::interval_type{5, 10, interval_border::closed_adjacent, interval_border::closed_adjacent}
    );
    auto result_closed_adjacent = tree_closed_adjacent.punch(
        types::interval_type{8, 12, interval_border::closed_adjacent, interval_border::closed_adjacent}
    );
    EXPECT_EQ(result_closed_adjacent.size(), 1);
    EXPECT_EQ(result_closed_adjacent.begin()->low(), 10);
    EXPECT_EQ(result_closed_adjacent.begin()->left_border(), interval_border::open);
    EXPECT_EQ(result_closed_adjacent.begin()->high(), 12);
}

TEST_F(PunchTests, DynamicPunchOverhangsFirstIntervalOnLeft)
{
    using namespace lib_interval_tree;

    using types = dynamic<int>;

    auto tree_closed = types::tree_type{};
    tree_closed.insert(types::interval_type{0, 5, interval_border::closed, interval_border::closed});
    tree_closed.insert(types::interval_type{5, 10, interval_border::closed, interval_border::closed});
    auto result_closed =
        tree_closed.punch(types::interval_type{-10, 3, interval_border::closed, interval_border::closed});
    EXPECT_EQ(result_closed.size(), 1);
    EXPECT_EQ(result_closed.begin()->low(), -10);
    EXPECT_EQ(result_closed.begin()->right_border(), interval_border::open);
    EXPECT_EQ(result_closed.begin()->high(), 0);

    auto tree_open = types::tree_type{};
    tree_open.insert(types::interval_type{0, 5, interval_border::open, interval_border::open});
    tree_open.insert(types::interval_type{5, 10, interval_border::open, interval_border::open});
    auto result_open = tree_open.punch(types::interval_type{-10, 3, interval_border::open, interval_border::open});
    EXPECT_EQ(result_open.size(), 1);
    EXPECT_EQ(result_open.begin()->low(), -10);
    EXPECT_EQ(result_open.begin()->right_border(), interval_border::closed);
    EXPECT_EQ(result_open.begin()->high(), 0);

    auto tree_left_open = types::tree_type{};
    tree_left_open.insert(types::interval_type{0, 5, interval_border::open, interval_border::closed});
    tree_left_open.insert(types::interval_type{5, 10, interval_border::open, interval_border::closed});
    auto result_left_open =
        tree_left_open.punch(types::interval_type{-10, 3, interval_border::open, interval_border::closed});
    EXPECT_EQ(result_left_open.size(), 1);
    EXPECT_EQ(result_left_open.begin()->low(), -10);
    EXPECT_EQ(result_left_open.begin()->right_border(), interval_border::closed);
    EXPECT_EQ(result_left_open.begin()->high(), 0);

    auto tree_right_open = types::tree_type{};
    tree_right_open.insert(types::interval_type{0, 5, interval_border::closed, interval_border::open});
    tree_right_open.insert(types::interval_type{5, 10, interval_border::closed, interval_border::open});
    auto result_right_open =
        tree_right_open.punch(types::interval_type{-10, 3, interval_border::closed, interval_border::open});
    EXPECT_EQ(result_right_open.size(), 1);
    EXPECT_EQ(result_right_open.begin()->low(), -10);
    EXPECT_EQ(result_right_open.begin()->right_border(), interval_border::open);
    EXPECT_EQ(result_right_open.begin()->high(), 0);

    auto tree_closed_adjacent = types::tree_type{};
    tree_closed_adjacent.insert(
        types::interval_type{0, 5, interval_border::closed_adjacent, interval_border::closed_adjacent}
    );
    tree_closed_adjacent.insert(
        types::interval_type{5, 10, interval_border::closed_adjacent, interval_border::closed_adjacent}
    );
    auto result_closed_adjacent = tree_closed_adjacent.punch(
        types::interval_type{-10, 3, interval_border::closed_adjacent, interval_border::closed_adjacent}
    );
    EXPECT_EQ(result_closed_adjacent.size(), 1);
    EXPECT_EQ(result_closed_adjacent.begin()->low(), -10);
    EXPECT_EQ(result_closed_adjacent.begin()->right_border(), interval_border::open);
    EXPECT_EQ(result_closed_adjacent.begin()->high(), 0);
}

TEST_F(PunchTests, PunchEncompassesTree)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{10, 15});
    tree.insert(types::interval_type{20, 25});
    tree.insert(types::interval_type{30, 35});
    auto result = tree.punch(types::interval_type{0, 35});

    ASSERT_EQ(result.size(), 3);
    auto iter = result.begin();
    EXPECT_EQ(iter->low(), 6);
    EXPECT_EQ(iter->high(), 9);

    EXPECT_EQ((++iter)->low(), 16);
    EXPECT_EQ(iter->high(), 19);

    EXPECT_EQ((++iter)->low(), 26);
    EXPECT_EQ(iter->high(), 29);
}

TEST_F(PunchTests, PunchEncompassesOpenTree)
{
    using types = open<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{10, 15});
    tree.insert(types::interval_type{20, 25});
    tree.insert(types::interval_type{30, 35});
    auto result = tree.punch(types::interval_type{0, 35});

    ASSERT_EQ(result.size(), 3);
    auto iter = result.begin();
    EXPECT_EQ(iter->low(), 5);
    EXPECT_EQ(iter->high(), 10);
    EXPECT_EQ((++iter)->low(), 15);
    EXPECT_EQ(iter->high(), 20);
    EXPECT_EQ((++iter)->low(), 25);
    EXPECT_EQ(iter->high(), 30);
}

TEST_F(PunchTests, PunchEncompassesTreeWithOverlap)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{10, 15});
    tree.insert(types::interval_type{20, 25});
    tree.insert(types::interval_type{30, 35});
    auto result = tree.punch(types::interval_type{-5, 40});

    ASSERT_EQ(result.size(), 5);
    auto iter = result.begin();
    EXPECT_EQ(iter->low(), -5);
    EXPECT_EQ(iter->high(), -1);

    EXPECT_EQ((++iter)->low(), 6);
    EXPECT_EQ(iter->high(), 9);

    EXPECT_EQ((++iter)->low(), 16);
    EXPECT_EQ(iter->high(), 19);

    EXPECT_EQ((++iter)->low(), 26);
    EXPECT_EQ(iter->high(), 29);

    EXPECT_EQ((++iter)->low(), 36);
    EXPECT_EQ(iter->high(), 40);
}

TEST_F(PunchTests, PunchEncompassesOpenTreeWithOverlap)
{
    using types = open<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{10, 15});
    tree.insert(types::interval_type{20, 25});
    tree.insert(types::interval_type{30, 35});
    auto result = tree.punch(types::interval_type{-5, 40});

    ASSERT_EQ(result.size(), 5);
    auto iter = result.begin();
    EXPECT_EQ(iter->low(), -5);
    EXPECT_EQ(iter->high(), 0);

    EXPECT_EQ((++iter)->low(), 5);
    EXPECT_EQ(iter->high(), 10);

    EXPECT_EQ((++iter)->low(), 15);
    EXPECT_EQ(iter->high(), 20);

    EXPECT_EQ((++iter)->low(), 25);
    EXPECT_EQ(iter->high(), 30);

    EXPECT_EQ((++iter)->low(), 35);
    EXPECT_EQ(iter->high(), 40);
}

TEST_F(PunchTests, UnsignedPunchHasNoProblemWithZero)
{
    using types = closed<unsigned int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});

    auto result = tree.punch(types::interval_type{0, 7});
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), 6);
    EXPECT_EQ(result.begin()->high(), 7);

    auto tree2 = types::tree_type{};
    tree2.insert(types::interval_type{1, 5});

    auto result2 = tree2.punch(types::interval_type{0, 7});

    ASSERT_EQ(result2.size(), 2);
    auto iter = result2.begin();
    EXPECT_EQ(iter->low(), 0);
    EXPECT_EQ(iter->high(), 0);
    EXPECT_EQ((++iter)->low(), 6);
    EXPECT_EQ(iter->high(), 7);
}

TEST_F(PunchTests, PunchDoesNotInsertIntervalWhenClosedGapIsEmpty)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{6, 10});

    auto result = tree.punch(types::interval_type{0, 12});
    ASSERT_EQ(result.size(), 1);

    EXPECT_EQ(result.begin()->low(), 11);
    EXPECT_EQ(result.begin()->high(), 12);
}

TEST_F(PunchTests, PunchDoesNotInsertIntervalWhenOpenGapIsEmpty)
{
    using types = open<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{6, 10});

    auto result = tree.punch(types::interval_type{0, 12});

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.begin()->low(), 10);
    EXPECT_EQ(result.begin()->high(), 12);
}

TEST_F(PunchTests, OpenFloatGap)
{
    using types = open<float>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0.0f, 5.0f});
    tree.insert(types::interval_type{6.0f, 10.0f});

    auto result = tree.punch(types::interval_type{0.0f, 12.0f});
    ASSERT_EQ(result.size(), 2);

    auto iter = result.begin();
    EXPECT_FLOAT_EQ(iter->low(), 5.0f);
    EXPECT_FLOAT_EQ(iter->high(), 6.0f);
    EXPECT_FLOAT_EQ((++iter)->low(), 10.0f);
    EXPECT_FLOAT_EQ(iter->high(), 12.0f);
}

TEST_F(PunchTests, ClosedFloatGap)
{
    using types = closed<float>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0.0f, 5.0f});
    tree.insert(types::interval_type{6.0f, 10.0f});

    auto result = tree.punch(types::interval_type{0.0f, 12.0f});
    ASSERT_EQ(result.size(), 2);

    auto iter = result.begin();
    EXPECT_NEAR(iter->low(), 5.0f, 0.001f);
    EXPECT_NEAR(iter->high(), 6.0f, 0.001f);
    EXPECT_NEAR((++iter)->low(), 10.0f, 0.001f);
    EXPECT_FLOAT_EQ(iter->high(), 12.0f);
}

TEST_F(PunchTests, PunchWithoutArgsEncompassesTree)
{
    using types = closed<int>;

    auto tree = types::tree_type{};
    tree.insert(types::interval_type{0, 5});
    tree.insert(types::interval_type{10, 15});
    tree.insert(types::interval_type{20, 25});
    tree.insert(types::interval_type{30, 35});
    auto result = tree.punch();

    ASSERT_EQ(result.size(), 3);
    auto iter = result.begin();
    EXPECT_EQ(iter->low(), 6);
    EXPECT_EQ(iter->high(), 9);

    EXPECT_EQ((++iter)->low(), 16);
    EXPECT_EQ(iter->high(), 19);

    EXPECT_EQ((++iter)->low(), 26);
    EXPECT_EQ(iter->high(), 29);
}