#pragma once

#include "interval_tree_fwd.hpp"
#include "interval_types.hpp"

#include "cairo-wrap/cairo_wrap.hpp"

#include <boost/optional.hpp>

#include <vector>

#define PUBLICIZE

namespace lib_interval_tree
{
//#####################################################################################################################
    using defnode = node <int, interval <int, right_open>>;
    using deftree = interval_tree <int, right_open>;
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
//---------------------------------------------------------------------------------------------------------------------
    template <typename... List>
    void drawTree(std::string const& fileName, lib_interval_tree::interval_tree <List...> const* tree)
    {
        auto grid = createGrid(*tree);
        auto surface = createSurface(grid);
        Cairo::DrawContext ctx(&surface);
        drawGrid(&ctx, grid);
        surface.saveToFile(fileName);
    }
//######################################################################################################
}
