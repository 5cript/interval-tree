#pragma once

#include "interval_io.hpp"
#include "test_utility.hpp"

#include <ctime>
#include <random>
#include <cmath>
#include <iostream>

struct Oracle
{
    int livingInstances = 0;
};

template <typename numerical_type, typename interval_kind_ = lib_interval_tree::closed>
class OracleInterval : public lib_interval_tree::interval<numerical_type, interval_kind_>
{
  public:
    using lib_interval_tree::interval<numerical_type, interval_kind_>::low_;
    using lib_interval_tree::interval<numerical_type, interval_kind_>::high_;

    OracleInterval(Oracle* oracle, numerical_type start, numerical_type end)
        : lib_interval_tree::interval<numerical_type, interval_kind_>(start, end)
        , oracle_{oracle}
    {
        ++oracle_->livingInstances;
    }
    OracleInterval(OracleInterval const& other)
        : lib_interval_tree::interval<numerical_type, interval_kind_>(other.low_, other.high_)
        , oracle_{other.oracle_}
    {
        ++oracle_->livingInstances;
    }
    OracleInterval(OracleInterval&& other)
        : lib_interval_tree::interval<numerical_type, interval_kind_>(other.low_, other.high_)
        , oracle_{other.oracle_}
    {
        other.oracle_ = nullptr;
    }
    OracleInterval& operator=(OracleInterval const& other)
    {
        lib_interval_tree::interval<numerical_type, interval_kind_>::operator=(other);
        oracle_ = other.oracle_;
        return *this;
    }
    OracleInterval& operator=(OracleInterval&& other)
    {
        lib_interval_tree::interval<numerical_type, interval_kind_>::operator=(other);
        oracle_ = other.oracle_;
        other.oracle_ = nullptr;
        return *this;
    }
    ~OracleInterval()
    {
        if (oracle_ != nullptr)
            --oracle_->livingInstances;
    }
    OracleInterval join(OracleInterval const& other) const
    {
        return OracleInterval{oracle_, std::min(low_, other.low_), std::max(high_, other.high_)};
    }

  private:
    Oracle* oracle_;
};

template <typename numerical_type, typename interval_kind_ = lib_interval_tree::closed>
OracleInterval<numerical_type, interval_kind_>
makeSafeOracleInterval(Oracle* oracle, numerical_type lhs, numerical_type rhs)
{
    return OracleInterval<numerical_type, interval_kind_>{oracle, std::min(lhs, rhs), std::max(lhs, rhs)};
}

class EraseTests : public ::testing::Test
{
  public:
    using interval_type = OracleInterval<int>;

  public:
    auto makeTree()
    {
        lib_interval_tree::interval_tree_t<int> regularTree;
        regularTree.insert({16, 21});
        regularTree.insert({8, 9});
        regularTree.insert({25, 30});
        regularTree.insert({5, 8});
        regularTree.insert({15, 23});
        regularTree.insert({17, 19});
        regularTree.insert({26, 26});
        regularTree.insert({0, 3});
        regularTree.insert({6, 10});
        regularTree.insert({19, 20});
        return regularTree;
    }

  protected:
    Oracle oracle;
    lib_interval_tree::interval_tree<OracleInterval<int>> oracleTree;
    std::default_random_engine gen;
    std::uniform_int_distribution<int> distSmall{-500, 500};
    std::uniform_int_distribution<int> distLarge{-50000, 50000};
};

TEST_F(EraseTests, EraseSingleElement)
{
    auto inserted_interval = interval_type{&oracle, 0, 16};

    oracleTree.insert(std::move(inserted_interval));

    EXPECT_EQ(oracle.livingInstances, 1);
    oracleTree.erase(oracleTree.begin());

    EXPECT_EQ(oracle.livingInstances, 0);
    EXPECT_EQ(oracleTree.empty(), true);
    EXPECT_EQ(oracleTree.size(), 0);
}

TEST_F(EraseTests, ManualClearTest)
{
    constexpr int amount = 10'000;

    for (int i = 0; i != amount; ++i)
        oracleTree.insert(makeSafeOracleInterval(&oracle, distSmall(gen), distSmall(gen)));

    for (auto i = std::begin(oracleTree); i != std::end(oracleTree);)
        i = oracleTree.erase(i);

    EXPECT_EQ(oracle.livingInstances, 0);
    EXPECT_EQ(oracleTree.empty(), true);
}

TEST_F(EraseTests, ClearTest)
{
    constexpr int amount = 10'000;

    for (int i = 0; i != amount; ++i)
        oracleTree.insert(makeSafeOracleInterval(&oracle, distSmall(gen), distSmall(gen)));

    oracleTree.clear();

    EXPECT_EQ(oracle.livingInstances, 0);
    EXPECT_EQ(oracleTree.empty(), true);
}

TEST_F(EraseTests, ExpectedElementIsDeleted)
{
    oracleTree.insert(makeSafeOracleInterval(&oracle, 16, 21));
    oracleTree.insert(makeSafeOracleInterval(&oracle, 8, 9));
    oracleTree.insert(makeSafeOracleInterval(&oracle, 25, 30));
    oracleTree.insert(makeSafeOracleInterval(&oracle, 5, 8));
    oracleTree.insert(makeSafeOracleInterval(&oracle, 15, 23));
    oracleTree.insert(makeSafeOracleInterval(&oracle, 17, 19));
    oracleTree.insert(makeSafeOracleInterval(&oracle, 26, 26));
    oracleTree.insert(makeSafeOracleInterval(&oracle, 0, 3));
    oracleTree.insert(makeSafeOracleInterval(&oracle, 6, 10));
    oracleTree.insert(makeSafeOracleInterval(&oracle, 19, 20));

    oracleTree.erase(oracleTree.find(makeSafeOracleInterval(&oracle, 17, 19)));
    EXPECT_EQ(oracleTree.find(makeSafeOracleInterval(&oracle, 17, 19)), oracleTree.end());
    EXPECT_EQ(oracleTree.size(), 9);
}

TEST_F(EraseTests, RandomEraseTest)
{
    constexpr int amount = 10'000;
    constexpr int deleteAmount = 50;

    for (int i = 0; i != amount; ++i)
        oracleTree.insert(makeSafeOracleInterval(&oracle, distSmall(gen), distSmall(gen)));

    for (int i = 0; i != deleteAmount; ++i)
    {
        std::uniform_int_distribution<int> dist{0, amount - i - 1};
        auto end = dist(gen);
        auto iter = oracleTree.begin();
        for (int j = 0; j != end; ++j)
            ++iter;
        oracleTree.erase(iter);
    }

    EXPECT_EQ(oracle.livingInstances, amount - deleteAmount);
    testMaxProperty(oracleTree);
    testTreeHeightHealth(oracleTree);
}

TEST_F(EraseTests, MassiveDeleteEntireTreeWithEraseReturnIterator)
{
    constexpr int amount = 1000;

    for (int i = 0; i != amount; ++i)
        oracleTree.insert(makeSafeOracleInterval(&oracle, distSmall(gen), distSmall(gen)));

    for (auto iter = oracleTree.begin(); !oracleTree.empty();)
    {
        iter = oracleTree.erase(iter);
    }

    EXPECT_EQ(oracle.livingInstances, 0);
    testMaxProperty(oracleTree);
    testTreeHeightHealth(oracleTree);
}

TEST_F(EraseTests, ReturnedIteratorPointsToNextInOrderNode)
{
    auto regularTree = makeTree();
    auto iter = regularTree.erase(regularTree.find({16, 21}));
    EXPECT_EQ(*iter, (decltype(regularTree)::interval_type{17, 19})) << *iter;

    regularTree = makeTree();
    iter = regularTree.erase(regularTree.find({8, 9}));
    EXPECT_EQ(*iter, (decltype(regularTree)::interval_type{15, 23})) << *iter;

    regularTree = makeTree();
    iter = regularTree.erase(regularTree.find({25, 30}));
    EXPECT_EQ(*iter, (decltype(regularTree)::interval_type{26, 26})) << *iter;

    regularTree = makeTree();
    iter = regularTree.erase(regularTree.find({5, 8}));
    EXPECT_EQ(*iter, (decltype(regularTree)::interval_type{6, 10})) << *iter;

    regularTree = makeTree();
    iter = regularTree.erase(regularTree.find({15, 23}));
    EXPECT_EQ(*iter, (decltype(regularTree)::interval_type{16, 21})) << *iter;

    regularTree = makeTree();
    iter = regularTree.erase(regularTree.find({17, 19}));
    EXPECT_EQ(*iter, (decltype(regularTree)::interval_type{19, 20})) << *iter;

    regularTree = makeTree();
    iter = regularTree.erase(regularTree.find({26, 26}));
    EXPECT_EQ(iter, regularTree.end());

    regularTree = makeTree();
    iter = regularTree.erase(regularTree.find({0, 3}));
    EXPECT_EQ(*iter, (decltype(regularTree)::interval_type{5, 8})) << *iter;

    regularTree = makeTree();
    iter = regularTree.erase(regularTree.find({6, 10}));
    EXPECT_EQ(*iter, (decltype(regularTree)::interval_type{8, 9})) << *iter;

    regularTree = makeTree();
    iter = regularTree.erase(regularTree.find({19, 20}));
    EXPECT_EQ(*iter, (decltype(regularTree)::interval_type{25, 30})) << *iter;
}

TEST_F(EraseTests, CanEraseEntireTreeUsingReturnedIterator)
{
    auto tree = makeTree();
    for (auto iter = tree.begin(); iter != tree.end();)
        iter = tree.erase(iter);
    EXPECT_EQ(tree.empty(), true);
}

TEST_F(EraseTests, FromNuiTest)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 0});
    tree.insert({4, 4});
    tree.insert({13, 13});

    auto iter = tree.erase(tree.find({4, 4}));
    EXPECT_EQ(*iter, (decltype(tree)::interval_type{13, 13})) << *iter;
}

TEST_F(EraseTests, EraseRangeOnEmptyTreeDoesNothing)
{
    lib_interval_tree::interval_tree_t<int> tree;
    ASSERT_NO_FATAL_FAILURE(tree.erase_range({0, 10}, false));
    EXPECT_TRUE(tree.empty());
}

TEST_F(EraseTests, EraseRangeOnIntervalInsideRangeIsRemoved)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});

    tree.erase_range({-10, 20}, false);
    EXPECT_TRUE(tree.empty());
}

TEST_F(EraseTests, EraseRangeOnIntervalEncompassingRangeIsRemoved)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});

    tree.erase_range({3, 5}, false);
    EXPECT_TRUE(tree.empty());
}

TEST_F(EraseTests, NonOverlappingIntervalIsNotRemoved)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});

    tree.erase_range({20, 30}, false);
    EXPECT_EQ(tree.size(), 1);
    EXPECT_EQ(*tree.begin(), (decltype(tree)::interval_type{0, 10}));
}

TEST_F(EraseTests, NonOverlappingIntervalIsNotRemoved2)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});
    tree.insert({25, 35});

    tree.erase_range({20, 30}, false);
    EXPECT_EQ(tree.size(), 1);
    EXPECT_EQ(*tree.begin(), (decltype(tree)::interval_type{0, 10}));
}

TEST_F(EraseTests, EraseRangeOnIntervalWithLeftSlice)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});

    tree.erase_range({-5, 5}, true);
    EXPECT_EQ(tree.size(), 1);
    EXPECT_EQ(*tree.begin(), (decltype(tree)::interval_type{5, 10}));
}

TEST_F(EraseTests, EraseRangeOnIntervalWithRightSlice)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});

    tree.erase_range({5, 15}, true);
    EXPECT_EQ(tree.size(), 1);
    EXPECT_EQ(*tree.begin(), (decltype(tree)::interval_type{0, 5}));
}

TEST_F(EraseTests, EraseRangeMiddleCutOut)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});

    tree.erase_range({3, 5}, true);
    EXPECT_EQ(tree.size(), 2);
    EXPECT_EQ(*tree.begin(), (decltype(tree)::interval_type{0, 3}));
    EXPECT_EQ(*(++tree.begin()), (decltype(tree)::interval_type{5, 10}));
}

TEST_F(EraseTests, EraseRangeLeftSliceIsNotReinsertedIfParamIsFalse)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});

    tree.erase_range({-5, 5}, false);
    EXPECT_TRUE(tree.empty());
}

TEST_F(EraseTests, EraseRangeRightSliceIsNotReinsertedIfParamIsFalse)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});

    tree.erase_range({5, 15}, false);
    EXPECT_TRUE(tree.empty());
}

TEST_F(EraseTests, SlicesAreReinsertedWithMultioverlap)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});
    tree.insert({5, 15});
    tree.insert({10, 20});

    tree.erase_range({3, 12}, true);
    EXPECT_EQ(tree.size(), 2);
    EXPECT_EQ(*tree.begin(), (decltype(tree)::interval_type{0, 3}));
    EXPECT_EQ(*(++tree.begin()), (decltype(tree)::interval_type{12, 20}));
}