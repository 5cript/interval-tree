class InsertTests
    : public ::testing::Test
{
public:
    using types = IntervalTypes <int>;

protected:
    lib_interval_tree::interval_tree <int> tree;
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
    tree.insert({0, 16});
    tree.insert({5, 13});

    EXPECT_EQ(tree.size(), 2);
}
