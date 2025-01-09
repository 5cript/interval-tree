#pragma once

#include <interval-tree/interval_tree.hpp>
#include <interval-tree/dot_graph.hpp>

class DotDrawTests : public ::testing::Test
{
  public:

  protected:
};

TEST_F(DotDrawTests, WillDrawEmptyTree)
{
    lib_interval_tree::interval_tree_t<int> tree;
    std::stringstream ss;
    lib_interval_tree::draw_dot_graph(ss, tree);
    EXPECT_EQ(ss.str(), "digraph G {\n}");
}

TEST_F(DotDrawTests, DrawsSingleNode)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});
    std::stringstream ss;
    lib_interval_tree::draw_dot_graph(ss, tree);
    EXPECT_EQ(ss.str(), "digraph G {\n\ta [label=\"[0,10]\"];\n}");
}

TEST_F(DotDrawTests, DrawsNodesWithEdge)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});
    tree.insert({10, 20});
    std::stringstream ss;
    lib_interval_tree::draw_dot_graph(ss, tree);
    EXPECT_EQ(ss.str(), "digraph G {\n\ta [label=\"[0,10]\"];\n\tb [label=\"[10,20]\"];\n\ta -> b;\n}");
}

TEST_F(DotDrawTests, SpaceAfterCommaIsSetIfEnabled)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});
    std::stringstream ss;
    lib_interval_tree::dot_graph_draw_settings settings;
    settings.space_after_comma = true;
    lib_interval_tree::draw_dot_graph(ss, tree, settings);
    EXPECT_EQ(ss.str(), "digraph G {\n\ta [label=\"[0, 10]\"];\n}");
}

TEST_F(DotDrawTests, GraphIsUndirectedIfSpecifiedAsSuch)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});
    tree.insert({10, 20});
    std::stringstream ss;
    lib_interval_tree::dot_graph_draw_settings settings;
    settings.digraph = false;
    lib_interval_tree::draw_dot_graph(ss, tree, settings);
    EXPECT_EQ(ss.str(), "graph G {\n\ta [label=\"[0,10]\"];\n\tb [label=\"[10,20]\"];\n\ta -- b;\n}");
}

TEST_F(DotDrawTests, CanSpecifyExtraNodeAttributes)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});
    std::stringstream ss;
    lib_interval_tree::dot_graph_draw_settings settings;
    settings.extra_node_attributes = {"color=red"};
    lib_interval_tree::draw_dot_graph(ss, tree, settings);
    EXPECT_EQ(ss.str(), "digraph G {\n\ta [label=\"[0,10]\", color=red];\n}");
}

TEST_F(DotDrawTests, GraphNameIsSet)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});
    std::stringstream ss;
    lib_interval_tree::dot_graph_draw_settings settings;
    settings.name = "MyGraph";
    lib_interval_tree::draw_dot_graph(ss, tree, settings);
    EXPECT_EQ(ss.str(), "digraph MyGraph {\n\ta [label=\"[0,10]\"];\n}");
}

TEST_F(DotDrawTests, CanOverrideLeftBrace)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});
    std::stringstream ss;
    lib_interval_tree::dot_graph_draw_settings settings;
    settings.left_brace = '(';
    lib_interval_tree::draw_dot_graph(ss, tree, settings);
    EXPECT_EQ(ss.str(), "digraph G {\n\ta [label=\"(0,10]\"];\n}");
}

TEST_F(DotDrawTests, CanOverrideRightBrace)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});
    std::stringstream ss;
    lib_interval_tree::dot_graph_draw_settings settings;
    settings.right_brace = ')';
    lib_interval_tree::draw_dot_graph(ss, tree, settings);
    EXPECT_EQ(ss.str(), "digraph G {\n\ta [label=\"[0,10)\"];\n}");
}

TEST_F(DotDrawTests, CanSpecifyExtraStatements)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});
    std::stringstream ss;
    lib_interval_tree::dot_graph_draw_settings settings;
    settings.extra_statements = {"rankdir=LR"};
    lib_interval_tree::draw_dot_graph(ss, tree, settings);
    EXPECT_EQ(ss.str(), "digraph G {\n\trankdir=LR;\n\ta [label=\"[0,10]\"];\n}");
}

TEST_F(DotDrawTests, CanSpecifyExtraEdgeAttributes)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});
    tree.insert({10, 20});
    std::stringstream ss;
    lib_interval_tree::dot_graph_draw_settings settings;
    settings.edge_attributes = {"color=blue"};
    lib_interval_tree::draw_dot_graph(ss, tree, settings);
    EXPECT_EQ(ss.str(), "digraph G {\n\ta [label=\"[0,10]\"];\n\tb [label=\"[10,20]\"];\n\ta -> b [color=blue];\n}");
}

TEST_F(DotDrawTests, IndentIsSet)
{
    lib_interval_tree::interval_tree_t<int> tree;
    tree.insert({0, 10});
    std::stringstream ss;
    lib_interval_tree::dot_graph_draw_settings settings;
    settings.indent = "XXX";
    lib_interval_tree::draw_dot_graph(ss, tree, settings);
    EXPECT_EQ(ss.str(), "digraph G {\nXXXa [label=\"[0,10]\"];\n}");
}

TEST_F(DotDrawTests, CanDrawTreeWithOpenIntervalType)
{
    lib_interval_tree::interval_tree<lib_interval_tree::interval<int, lib_interval_tree::open>> tree;
    tree.insert({0, 10});
    tree.insert({10, 20});
    std::stringstream ss;
    lib_interval_tree::draw_dot_graph(ss, tree);
    EXPECT_EQ(ss.str(), "digraph G {\n\ta [label=\"(0,10)\"];\n\tb [label=\"(10,20)\"];\n\ta -> b;\n}");
}

TEST_F(DotDrawTests, CanDrawTreeWithLeftOpenIntervalType)
{
    lib_interval_tree::interval_tree<lib_interval_tree::interval<int, lib_interval_tree::left_open>> tree;
    tree.insert({0, 10});
    tree.insert({10, 20});
    std::stringstream ss;
    lib_interval_tree::draw_dot_graph(ss, tree);
    EXPECT_EQ(ss.str(), "digraph G {\n\ta [label=\"(0,10]\"];\n\tb [label=\"(10,20]\"];\n\ta -> b;\n}");
}

TEST_F(DotDrawTests, CanDrawTreeWithRightOpenIntervalType)
{
    lib_interval_tree::interval_tree<lib_interval_tree::interval<int, lib_interval_tree::right_open>> tree;
    tree.insert({0, 10});
    tree.insert({10, 20});
    std::stringstream ss;
    lib_interval_tree::draw_dot_graph(ss, tree);
    EXPECT_EQ(ss.str(), "digraph G {\n\ta [label=\"[0,10)\"];\n\tb [label=\"[10,20)\"];\n\ta -> b;\n}");
}

TEST_F(DotDrawTests, CanDrawTreeWithClosedAdjacentIntervalType)
{
    lib_interval_tree::interval_tree<lib_interval_tree::interval<int, lib_interval_tree::closed_adjacent>> tree;
    tree.insert({0, 10});
    tree.insert({10, 20});
    std::stringstream ss;
    lib_interval_tree::draw_dot_graph(ss, tree);
    EXPECT_EQ(ss.str(), "digraph G {\n\ta [label=\"[0,10]\"];\n\tb [label=\"[10,20]\"];\n\ta -> b;\n}");
}

#if __cplusplus >= 201703L
TEST_F(DotDrawTests, CanDrawTreeWithDynamicIntervalType)
{
    lib_interval_tree::interval_tree<lib_interval_tree::interval<int, lib_interval_tree::dynamic>> tree;
    tree.insert({0, 10, lib_interval_tree::interval_border::closed, lib_interval_tree::interval_border::open});
    tree.insert({10, 20, lib_interval_tree::interval_border::open, lib_interval_tree::interval_border::closed_adjacent}
    );
    std::stringstream ss;
    lib_interval_tree::draw_dot_graph(ss, tree);
    EXPECT_EQ(ss.str(), "digraph G {\n\ta [label=\"[0,10)\"];\n\tb [label=\"(10,20]\"];\n\ta -> b;\n}");
}
#endif

TEST_F(DotDrawTests, CanDrawLargerTree)
{
    using namespace lib_interval_tree;
    interval_tree_t<int> tree;

    tree.insert(make_safe_interval<int>(21, 16)); // make_safe_interval swaps low and high if not in right order.
    tree.insert({8, 9});
    tree.insert({25, 30});
    tree.insert({5, 8});
    tree.insert({15, 23});
    tree.insert({17, 19});
    tree.insert({26, 26});
    tree.insert({0, 3});
    tree.insert({6, 10});
    tree.insert({19, 20});

    std::stringstream ss;

    lib_interval_tree::dot_graph_draw_settings settings;
    settings.indent = "";

    draw_dot_graph(ss, tree, settings);
    EXPECT_EQ(
        ss.str(),
        R"(digraph G {
a [label="[16,21]"];
b [label="[8,9]"];
c [label="[5,8]"];
d [label="[0,3]"];
e [label="[6,10]"];
f [label="[15,23]"];
g [label="[25,30]"];
h [label="[17,19]"];
i [label="[19,20]"];
j [label="[26,26]"];
a -> b;
b -> c;
c -> d;
c -> e;
b -> f;
a -> g;
g -> h;
h -> i;
g -> j;
})"
    );
}