class InsertTests : public ::testing::Test
{
protected:
    lib_interval_tree::interval_tree <int> tree;
};

TEST_F(InsertTests, InsertIntoEmpty)
{
    tree.insert({0, 16});

    EXPECT_EQ(tree.size(), 1);
}
