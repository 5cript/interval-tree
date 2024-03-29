#pragma once

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
OracleInterval <numerical_type, interval_kind_> makeSafeOracleInterval(Oracle* oracle, numerical_type lhs, numerical_type rhs)
{
    return OracleInterval <numerical_type, interval_kind_>{oracle, std::min(lhs, rhs), std::max(lhs, rhs)};
}

class EraseTests
    : public ::testing::Test
{
public:
    using interval_type = OracleInterval<int>;

protected:
    Oracle oracle;
    lib_interval_tree::interval_tree <OracleInterval<int>> tree;
    std::default_random_engine gen;
    std::uniform_int_distribution <int> distSmall{-500, 500};
    std::uniform_int_distribution <int> distLarge{-50000, 50000};
};

TEST_F(EraseTests, EraseSingleElement)
{
    auto inserted_interval = interval_type{&oracle, 0, 16};

    tree.insert(std::move(inserted_interval));

    EXPECT_EQ(oracle.livingInstances, 1);
    tree.erase(tree.begin());

    EXPECT_EQ(oracle.livingInstances, 0);
    EXPECT_EQ(tree.empty(), true);
    EXPECT_EQ(tree.size(), 0);
}

TEST_F(EraseTests, ManualClearTest)
{
    constexpr int amount = 10'000;

    for (int i = 0; i != amount; ++i)
        tree.insert(makeSafeOracleInterval(&oracle, distSmall(gen), distSmall(gen)));

    for (auto i = std::begin(tree); i != std::end(tree);)
        i = tree.erase(i);

    EXPECT_EQ(oracle.livingInstances, 0);
    EXPECT_EQ(tree.empty(), true);
}

TEST_F(EraseTests, ClearTest)
{
    constexpr int amount = 10'000;

    for (int i = 0; i != amount; ++i)
        tree.insert(makeSafeOracleInterval(&oracle, distSmall(gen), distSmall(gen)));

    tree.clear();

    EXPECT_EQ(oracle.livingInstances, 0);
    EXPECT_EQ(tree.empty(), true);
}

TEST_F(EraseTests, ExpectedElementIsDeleted)
{
    tree.insert(makeSafeOracleInterval(&oracle, 16, 21));
    tree.insert(makeSafeOracleInterval(&oracle, 8, 9));
    tree.insert(makeSafeOracleInterval(&oracle, 25, 30));
    tree.insert(makeSafeOracleInterval(&oracle, 5, 8));
    tree.insert(makeSafeOracleInterval(&oracle, 15, 23));
    tree.insert(makeSafeOracleInterval(&oracle, 17, 19));
    tree.insert(makeSafeOracleInterval(&oracle, 26, 26));
    tree.insert(makeSafeOracleInterval(&oracle, 0, 3));
    tree.insert(makeSafeOracleInterval(&oracle, 6, 10));
    tree.insert(makeSafeOracleInterval(&oracle, 19, 20));

    tree.erase(tree.find(makeSafeOracleInterval(&oracle, 17, 19)));
    EXPECT_EQ(tree.find(makeSafeOracleInterval(&oracle, 17, 19)), tree.end());
    EXPECT_EQ(tree.size(), 9);
}

TEST_F(EraseTests, RandomEraseTest)
{
    constexpr int amount = 10'000;
    constexpr int deleteAmount = 50;

    for (int i = 0; i != amount; ++i)
        tree.insert(makeSafeOracleInterval(&oracle, distSmall(gen), distSmall(gen)));

    for (int i = 0; i != deleteAmount; ++i)
    {
        std::uniform_int_distribution <int> dist{0, amount - i - 1};
        auto end = dist(gen);
        auto iter = tree.begin();
        for (int j = 0; j != end; ++j)
            ++iter;
        tree.erase(iter);
    }

    EXPECT_EQ(oracle.livingInstances, amount - deleteAmount);
    testMaxProperty(tree);
    testTreeHeightHealth(tree);
}
