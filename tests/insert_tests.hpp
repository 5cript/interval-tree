#include <ctime>
#include <random>
#include <cmath>

class InsertTests
    : public ::testing::Test
{
public:
    using types = IntervalTypes <int>;

protected:
    lib_interval_tree::interval_tree <int> tree;
    std::default_random_engine gen;
    std::uniform_int_distribution <int> dist{-500, 500};
};

TEST_F(InsertTests, InsertIntoEmpty1)
{
    auto inserted_interval = types::interval_type{0, 16};

    tree.insert(inserted_interval);
    EXPECT_EQ(*tree.begin(), inserted_interval);
    EXPECT_EQ(tree.size(), 1);
}

TEST_F(InsertTests, InsertIntoEmpty2)
{
    auto inserted_interval = types::interval_type{-45, 16};

    tree.insert(inserted_interval);
    EXPECT_EQ(*tree.begin(), inserted_interval);
    EXPECT_EQ(tree.size(), 1);
}

TEST_F(InsertTests, InsertMultipleIntoEmpty)
{
    auto firstInterval = types::interval_type{0, 16};
    auto secondInterval = types::interval_type{5, 13};

    tree.insert(firstInterval);
    tree.insert(secondInterval);

    EXPECT_EQ(tree.size(), 2);

    EXPECT_EQ(*tree.begin(), firstInterval);
    EXPECT_EQ(*(++tree.begin()), secondInterval);
}

TEST_F(InsertTests, TreeHeightHealthynessTest)
{
    constexpr int amount = 100'000;

    for (int i = 0; i != amount; ++i)
        tree.insert(lib_interval_tree::make_safe_interval(dist(gen), dist(gen)));

    auto maxHeight{0};
    for (auto i = std::begin(tree); i != std::end(tree); ++i)
        maxHeight = std::max(maxHeight, i->height());

    EXPECT_LE(maxHeight, 2 * std::log2(amount + 1));
}

TEST_F(InsertTests, MaxValueTest1)
{
    tree.insert(types::interval_type{-51, 11});
    tree.insert(types::interval_type{26, 68});
    tree.insert(types::interval_type{11, 100});
    tree.insert(types::interval_type{-97, 65});
    tree.insert(types::interval_type{-85, 18});
    tree.insert(types::interval_type{-31, -20});
    tree.insert(types::interval_type{-91, -6});
    tree.insert(types::interval_type{-17, 71});
    tree.insert(types::interval_type{-58, 37});
    tree.insert(types::interval_type{-50, -1});
    tree.insert(types::interval_type{11, 61});
    tree.insert(types::interval_type{6, 74});
    tree.insert(types::interval_type{13, 78});
    tree.insert(types::interval_type{-83, -62});
    tree.insert(types::interval_type{-80, 93});
    tree.insert(types::interval_type{-2, 84});
    tree.insert(types::interval_type{-62, -18});
    tree.insert(types::interval_type{-96, -53});
    tree.insert(types::interval_type{56, 91});
    tree.insert(types::interval_type{37, 79});

    EXPECT_EQ(tree.root()->max(), 100);
}
