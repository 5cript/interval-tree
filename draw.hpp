#pragma once

#include "interval_tree.hpp"

#include "cairo-wrap/cairo_wrap.hpp"

#include <boost/optional.hpp>

#include <vector>

namespace lib_interval_tree
{
//#####################################################################################################################
    using defnode = node <int, interval <int, right_open>>;
    using deftree = interval_tree <>;
//#####################################################################################################################
    struct TreeGridNode
    {
        defnode const* node;
        std::pair <int, int> parentCoords;
    };
    struct TreeGrid
    {
        // (row-major)
        std::vector < // rows
            std::vector < // columns
                boost::optional <TreeGridNode>
            >
        > grid;
        int xMax = 0;
        int xMin = 0;
        int yMax = 0;
    };
//#####################################################################################################################
    void drawNode(Cairo::DrawContext* ctx, defnode const* node, double x, double y);
//---------------------------------------------------------------------------------------------------------------------
    template <typename NodeT>
    void drawNode(Cairo::DrawContext* ctx, interval_tree_iterator <NodeT> const& iter, double x, double y)
    {
        drawNode(ctx, iter.node_, x, y);
    }
//---------------------------------------------------------------------------------------------------------------------
    TreeGrid createGrid(deftree const& tree);
//---------------------------------------------------------------------------------------------------------------------
    void drawGrid(Cairo::DrawContext* ctx, TreeGrid const& grid, bool drawEmpty = false);
//---------------------------------------------------------------------------------------------------------------------
    Cairo::Surface createSurface(TreeGrid const& grid);
//#####################################################################################################################
}
