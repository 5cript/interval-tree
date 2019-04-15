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
    std::uniform_int_distribution <int> distSmall{-500, 500};
    std::uniform_int_distribution <int> distLarge{-50000, 50000};
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
        tree.insert(lib_interval_tree::make_safe_interval(distSmall(gen), distSmall(gen)));

    auto maxHeight{0};
    for (auto i = std::begin(tree); i != std::end(tree); ++i)
        maxHeight = std::max(maxHeight, i->height());

    EXPECT_LE(maxHeight, 2 * std::log2(amount + 1));
}

TEST_F(InsertTests, MaxValueTest1)
{
    constexpr int amount = 100'000;

    for (int i = 0; i != amount; ++i)
        tree.insert(lib_interval_tree::make_safe_interval(distSmall(gen), distSmall(gen)));

    for (auto i = std::begin(tree); i != std::end(tree); ++i)
    {
        if (i->left())
        {
            EXPECT_LE(i->left()->max(), i->max());
        }
        if (i->right())
        {
            EXPECT_LE(i->right()->max(), i->max());
        }
    }
}
