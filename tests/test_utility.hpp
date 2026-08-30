#pragma once

#include <interval-tree/interval_tree.hpp>

#include <gtest/gtest.h>

#include <functional>
#include <cmath>

/**
 *  This function has linear complexity.
 */
template <typename TreeT>
void testRedBlackPropertyViolation(TreeT const& tree)
{
    using namespace lib_interval_tree;

    // root is always black.
    EXPECT_EQ(tree.root().color(), rb_color::black);

    std::function<int(typename TreeT::const_iterator)> verify = [&](typename TreeT::const_iterator node) -> int {
        if (node == std::cend(tree))
            return 1;

        // check that all nodes have red or black coloring. (seems obvious, but is not on bug)
        EXPECT_EQ(true, node.color() == rb_color::black || node.color() == rb_color::red);

        // check for (red children = black) property:
        if (node.color() == rb_color::red)
        {
            if (node.left() != std::end(tree))
            {
                EXPECT_EQ(node.left().color(), rb_color::black);
            }
            if (node.right() != std::end(tree))
            {
                EXPECT_EQ(node.right().color(), rb_color::black);
            }
        }

        // Test that all paths from a node down to its null descendants contain the same number of black nodes.
        const auto leftHeight = verify(node.left());
        const auto rightHeight = verify(node.right());
        EXPECT_EQ(leftHeight, rightHeight);
        return leftHeight + (node.color() == rb_color::black ? 1 : 0);
    };

    verify(tree.root());
}

template <typename TreeT>
void testMaxProperty(TreeT const& tree)
{
    for (auto i = std::begin(tree); i != std::end(tree); ++i)
    {
        if (i.node()->left())
        {
            EXPECT_LE(i.node()->left()->max(), i.node()->max());
        }
        if (i.node()->right())
        {
            EXPECT_LE(i.node()->right()->max(), i.node()->max());
        }
        EXPECT_GE(i.node()->max(), i.interval().high());
    }
}

template <typename TreeT>
void testTreeHeightHealth(TreeT const& tree)
{
    const auto treeSize = tree.size();

    auto maxHeight{0};
    for (auto i = std::begin(tree); i != std::end(tree); ++i)
        maxHeight = std::max(maxHeight, i.node()->height());

    const auto calc = 2 * std::log2(static_cast<int>(treeSize) + 1);
    EXPECT_LE(maxHeight, calc);
}
