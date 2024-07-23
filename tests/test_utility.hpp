#pragma once

#include <functional>
#include <list>
#include <cmath>

/**
 *  Warning this function is very expensive.
 */
template <typename TreeT>
void testRedBlackPropertyViolation(TreeT const& tree)
{
    using namespace lib_interval_tree;

    // root is always black.
    EXPECT_EQ(tree.root().color(), rb_color::black);

    // check that all nodes have red or black coloring. (seems obvious, but is not on bug)
    for (auto i = std::begin(tree); i != std::end(tree); ++i)
    {
        EXPECT_EQ(true, i.color() == rb_color::black || i.color() == rb_color::red);
    }

    // check for (red children = black) property:
    for (auto i = std::begin(tree); i != std::end(tree); ++i)
    {
        auto nodeColor = i.color();
        if (nodeColor == rb_color::red)
        {
            if (i.left() != std::end(tree))
            {
                EXPECT_EQ(i.left().color(), rb_color::black);
            }
            if (i.right() != std::end(tree))
            {
                EXPECT_EQ(i.right().color(), rb_color::black);
            }
        }
    }

    auto leafCollector = [&](typename TreeT::const_iterator root)
    {
        std::list <typename TreeT::const_iterator> leaves{};
        std::function <void(typename std::list <typename TreeT::const_iterator>::iterator)> recursiveLeafFinder;
        recursiveLeafFinder = [&](typename std::list <typename TreeT::const_iterator>::iterator self)
        {
            if (self->left() != std::end(tree))
            {
                recursiveLeafFinder(leaves.insert(self, self->left()));
            }
            if (self->right() != std::end(tree))
            {
                *self = self->right();
                recursiveLeafFinder(self);
            }
        };
        leaves.push_back(root);
        recursiveLeafFinder(leaves.begin());
        return leaves;
    };

    // Test that for every node, on the path to its leaves, has the same number of black nodes.
    for (auto i = std::cbegin(tree); i != std::cend(tree); ++i)
    {
        auto leaves = leafCollector(i);
        int comparisonCounter{0};
        for (auto const& leaf : leaves)
        {
            auto p = leaf;
            int counter{0};
            do
            {
                if (p.color() == rb_color::black)
                    ++counter;
                p = p.parent();
            } while (p != i && p != std::end(tree));
            if (comparisonCounter == 0)
                comparisonCounter = counter;
            else
            {
                EXPECT_EQ(comparisonCounter, counter);
            }
        }
    }
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
    int treeSize = tree.size();

    auto maxHeight{0};
    for (auto i = std::begin(tree); i != std::end(tree); ++i)
        maxHeight = std::max(maxHeight, i.node()->height());

    EXPECT_LE(maxHeight, 2 * std::log2(treeSize + 1));
}
