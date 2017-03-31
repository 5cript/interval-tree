#include "draw.hpp"

#include "cairo-wrap/cairo_wrap.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>

namespace lib_interval_tree
{
//#####################################################################################################################
    template <unsigned Size>
    struct NumericalPointerEquivalent
    {};

    template <>
    struct NumericalPointerEquivalent <sizeof(unsigned long)>
    {
        using type = unsigned long;
    };

    template <>
    struct NumericalPointerEquivalent <sizeof(unsigned long long)>
    {
        using type = unsigned long long;
    };
//#####################################################################################################################
    constexpr double margin = 5.;
    constexpr double gridMargin = 5.;
    constexpr double yPadding = 0.;
    constexpr double xPadding = 30.;
    constexpr double leftPadding = 10.;
    constexpr double rightPadding = 10.;
    constexpr double topPadding = 10.;
    constexpr double bottomPadding = 10.;
    constexpr Cairo::Pen blackPen = {3., Cairo::Colors::Black};
    constexpr Cairo::Pen nodeCaptionPen =  {3., Cairo::Colors::Black};
    constexpr Cairo::Pen ptrPen =  {3., Cairo::Colors::Red};
    constexpr Cairo::Pen edgePen =  {8., Cairo::Colors::Black};
    constexpr auto whitePen = Cairo::Colors::White;
//---------------------------------------------------------------------------------------------------------------------
    std::string pointerString(void const* ptr)
    {
        std::stringstream sstr;
        sstr << "0x" << reinterpret_cast <NumericalPointerEquivalent<sizeof(void*)>::type> (ptr);
        return sstr.str();
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string nodeCaption(defnode const* node)
    {
        auto ival = node->interval();
        std::stringstream sstr;
        sstr << '[' << ival.low() << ',' << ival.high() << ']';
        return sstr.str();
    }
//---------------------------------------------------------------------------------------------------------------------
    auto getNodeBounds()
    {
        Cairo::Surface dummySurface(0, 0);
        Cairo::DrawContext dummyContext(&dummySurface);

        auto const captionBoundProvider = Cairo::Text(
            &dummyContext,
            0,
            0,
            "[00,00]",
            {"Arial", 18, CAIRO_FONT_WEIGHT_BOLD}
        );

        auto ptr = Cairo::Text(
            &dummyContext,
            0,
            0,
            pointerString(nullptr),
            {"Arial", 10}
        );
        auto ptrBounds = ptr.calculateBounds(ptrPen);

        auto bounds = captionBoundProvider.calculateBounds(nodeCaptionPen);
        bounds.setWidth(bounds.getWidth() + 30.);
        bounds.setHeight(bounds.getWidth() + ptrBounds.getHeight() + 5.);

        return bounds;
    }
//---------------------------------------------------------------------------------------------------------------------
    auto getNodeRadius()
    {
        return getNodeBounds().getWidth() / 2. + margin * 2.;
    }
//#####################################################################################################################
    void drawNode(Cairo::DrawContext* ctx, defnode const* node, double x, double y)
    {
        auto caption = Cairo::Text(
            ctx,
            0,
            0,
            nodeCaption(node),
            {"Arial", 18, CAIRO_FONT_WEIGHT_BOLD}
        );

        auto ptr = Cairo::Text(
            ctx,
            0,
            0,
            pointerString(node),
            {"Arial", 10}
        );

        auto actualCaptionBounds = caption.calculateBounds(blackPen);
        auto nodeCaptionBounds = getNodeBounds();
        auto bounds = nodeCaptionBounds;
        auto ptrBounds = ptr.calculateBounds(ptrPen);

        nodeCaptionBounds.setWidth(bounds.getWidth() - 30.);
        nodeCaptionBounds.setHeight(bounds.getWidth() - ptrBounds.getHeight() - 5.);

        double circleX = x;
        double circleY = y;
        double circleRadius = getNodeRadius();

        while (ptrBounds.getWidth() > circleRadius * 2 && caption.getFont().size > 6)
        {
            caption.getFont().size--;
            ptrBounds = ptr.calculateBounds(ptrPen);
        }

        Cairo::Arc circle {
            ctx,
            circleX,
            circleY,
            circleRadius
        };
        circle.draw(blackPen, whitePen);

        caption.move(circleX - actualCaptionBounds.getWidth() / 2., circleY - actualCaptionBounds.getHeight() / 2. - ptrBounds.getHeight());
        caption.draw(nodeCaptionPen);

        ptr.move(circleX - ptrBounds.getWidth() / 2., circleY - ptrBounds.getHeight() / 2. + 10.);
        ptr.draw(ptrPen);
    }
//---------------------------------------------------------------------------------------------------------------------
    TreeGrid createGrid(deftree const& tree)
    {
        auto* root = tree.root_;
        if (root == nullptr)
            return {};

        TreeGrid grid;

        struct _GridPoint
        {
            defnode const* node;
            defnode const* parent;
            int x;
            int y;
        };

        std::vector <_GridPoint> gridPoints;

        std::function <int(defnode const* node)> subtreeSize;
        subtreeSize = [&](defnode const* node) {
            if (!node)
                return 0;
            if (!node->left_ && !node->right_)
                return 1;
            if (node->right_ && node->left_)
                return subtreeSize(node->right_) + subtreeSize(node->left_) + 2;
            if (node->right_)
                return subtreeSize(node->right_) + 1;
            if (node->left_)
                return subtreeSize(node->left_) + 1;
            return 0;
        };

        std::function <void(defnode const*, int pX, int pY)> deduceCoordinates;
        deduceCoordinates = [&](defnode const* node, int pX, int pY)
        {
            int y = pY;
            int x = pX;
            if (!node->is_root())
            {
                ++y;
                if (node->is_right())
                    x = x + subtreeSize(node->left_) + 1;
                else // is_left
                    x = x - subtreeSize(node->right_) - 1;
            }
            gridPoints.push_back({node, node->parent_, x, y});

            if (node->left_)
                deduceCoordinates(node->left_, x, y);
            if (node->right_)
                deduceCoordinates(node->right_, x, y);
        };

        deduceCoordinates(root, 0, 0);

        for (auto& i : gridPoints)
        {
            grid.xMin = std::min(grid.xMin, i.x);
            grid.xMax = std::max(grid.xMax, i.x);
            grid.yMax = std::max(grid.yMax, i.y);
        }

        auto width = -grid.xMin + grid.xMax + 1;
        auto height = grid.yMax + 1;

        grid.grid.resize(height);

        for (auto& i : grid.grid)
        {
            i.resize(width);
        }

        for (auto& i : gridPoints)
        {
            std::cout << "[" << i.y << "][" << i.x + -grid.xMin << "]\n";
            std::pair <int, int> parentCoords = {-1, -1};
            for (auto const& j : gridPoints)
            {
                if (j.node == i.parent)
                {
                    parentCoords = {j.x - grid.xMin, j.y};
                    break;
                }
            }

            grid.grid[i.y][i.x + -grid.xMin] = {TreeGridNode{i.node, parentCoords}};
        }

        return grid;
    }
//---------------------------------------------------------------------------------------------------------------------
    void drawGrid(Cairo::DrawContext* ctx, TreeGrid const& grid, bool drawEmpty)
    {
        auto nodeRadius = getNodeRadius();
        auto cellSize = nodeRadius * 2. + gridMargin;

        auto nodeX = [&](int x_) {return leftPadding + nodeRadius + x_ * cellSize + x_ * xPadding;};
        auto nodeY = [&](int y_) {return topPadding + nodeRadius + y_ * cellSize + y_ * yPadding;};

        // Draw Lines
        int y = 0;
        for (auto const& row : grid.grid)
        {
            int x = 0;
            for (auto const& cell : row)
            {
                if (cell && cell.get().parentCoords.first != -1)
                {
                    auto line = Cairo::Line {
                        ctx,
                        nodeX(x),
                        nodeY(y),
                        nodeX(cell.get().parentCoords.first),
                        nodeY(cell.get().parentCoords.second)
                    };
                    line.draw(edgePen);
                }
                ++x;
            }
            ++y;
        }
        y = 0;

        // Draw Nodes
        for (auto const& row : grid.grid)
        {
            int x = 0;
            for (auto const& cell : row)
            {
                if (cell)
                {
                    auto* node = cell.get().node;
                    drawNode(ctx, node, nodeX(x), nodeY(y));
                }
                else if (drawEmpty)
                {
                    Cairo::Arc circle {
                        ctx,
                        nodeX(x),
                        nodeY(y),
                        nodeRadius
                    };
                    circle.draw(blackPen, ptrPen);

                }
                ++x;
            }
            ++y;
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    Cairo::Surface createSurface(TreeGrid const& grid)
    {
        auto nodeRadius = getNodeRadius();
        auto cellSize = nodeRadius * 2. + gridMargin;
        int width = (grid.xMax - grid.xMin) + 1;
        int height = grid.yMax + 1;

        return {
            static_cast <int> (leftPadding + (width) * cellSize + (width-1) * xPadding + rightPadding),
            static_cast <int> (topPadding + (height) * cellSize + (height-1) * yPadding + bottomPadding)
        };
    }
//#####################################################################################################################
}
