#pragma once

#include "interval_types.hpp"
#include "interval_tree.hpp"

#include <cairo-wrap/cairo_wrap.hpp>

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

#include <boost/optional.hpp>

namespace lib_interval_tree
{
    namespace
    {
        using int_tree_for_draw = interval_tree <interval<int, closed>>;
        using int_tree_iterator = int_tree_for_draw::iterator;
        using const_int_tree_iterator = int_tree_for_draw::const_iterator;

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

        std::string iterCaption(const_int_tree_iterator iter)
        {
            auto ival = iter->interval();
            std::stringstream sstr;
            sstr << '[' << ival.low() << ',' << ival.high() << ']';
            return sstr.str();
        }

        std::string pointerString(void const* ptr)
        {
            std::stringstream sstr;
            sstr << "0x" << std::hex << reinterpret_cast <NumericalPointerEquivalent<sizeof(void*)>::type> (ptr);
            return sstr.str();
        }

		constexpr double margin = 5.;
		constexpr double gridMargin = 5.;
		constexpr double yPadding = 0.;
		constexpr double xPadding = 30.;
		constexpr double leftPadding = 10.;
		constexpr double rightPadding = 10.;
		constexpr double topPadding = 10.;
		constexpr double bottomPadding = 10.;
		constexpr Cairo::Pen blackPen = {3., Cairo::Colors::Black};
		constexpr Cairo::Pen iterCaptionPen =  {3., Cairo::Colors::Black};
		constexpr Cairo::Pen ptrPen =  {3., Cairo::Colors::Red};
		constexpr Cairo::Pen edgePen =  {8., Cairo::Colors::Black};
		constexpr auto whitePen = Cairo::Colors::White;

		auto getiterBounds()
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

            auto bounds = captionBoundProvider.calculateBounds(iterCaptionPen);
            bounds.setWidth(bounds.getWidth() + 30.);
            bounds.setHeight(bounds.getWidth() + ptrBounds.getHeight() + 5.);

            return bounds;
        }

        auto getiterRadius()
        {
            return getiterBounds().getWidth() / 2. + margin * 2.;
        }
    }
//#####################################################################################################################
    struct TreeGriditer
    {
        const_int_tree_iterator iter;
        std::pair <int, int> parentCoords;
    };
    struct TreeGrid
    {
        // (row-major)
        std::vector < // rows
            std::vector < // columns
                boost::optional <TreeGriditer>
            >
        > grid;
        int xMax = 0;
        int xMin = 0;
        int yMax = 0;
    };
//#####################################################################################################################
    void drawIter(Cairo::DrawContext* ctx, const_int_tree_iterator iter, double x, double y, bool drawPointers)
    {
        auto caption = Cairo::Text(
            ctx,
            0,
            0,
            iterCaption(iter),
            {"Arial", 18, CAIRO_FONT_WEIGHT_BOLD}
        );

        auto max = Cairo::Text(
            ctx,
            0,
            0,
            //pointerString(iter),
            std::to_string(iter.max()),
            {"Arial", 12}
        );

        /*
        auto ptr = Cairo::Text(
            ctx,
            0,
            0,
            pointerString(iter->),
            {"Arial", 10}
        );
        */

        auto actualCaptionBounds = caption.calculateBounds(blackPen);
        auto iterCaptionBounds = getiterBounds();
        auto bounds = iterCaptionBounds;
        auto maxBounds = max.calculateBounds(ptrPen);
        //auto ptrBounds = ptr.calculateBounds(ptrPen);

        iterCaptionBounds.setWidth(bounds.getWidth() - 30.);
        iterCaptionBounds.setHeight(bounds.getWidth() - maxBounds.getHeight() - 5.);

        double circleX = x;
        double circleY = y;
        double circleRadius = getiterRadius();

        while (maxBounds.getWidth() > circleRadius * 2 && caption.getFont().size > 6)
        {
            caption.getFont().size--;
            maxBounds = max.calculateBounds(ptrPen);
        }

        Cairo::Arc circle {
            ctx,
            circleX,
            circleY,
            circleRadius
        };
        switch (iter->color())
        {
        case (rb_color::red):
            circle.draw(blackPen, Cairo::Colors::Red);
            break;
        case (rb_color::black):
            circle.draw(blackPen, Cairo::Colors::Black);
            break;
        case (rb_color::fail):
            circle.draw(blackPen, Cairo::Colors::White);
            break;
        case (rb_color::double_black):
            circle.draw(blackPen, Cairo::Colors::Gray);
            break;
        }

        caption.move(circleX - actualCaptionBounds.getWidth() / 2., circleY - actualCaptionBounds.getHeight() / 2. - maxBounds.getHeight());

        if (iter->color() != rb_color::black)
            caption.draw(iterCaptionPen);
        else
            caption.draw(Cairo::Colors::White);

        max.move(circleX - maxBounds.getWidth() / 2., circleY - maxBounds.getHeight() / 2. + 10.);
        //ptr.move(circleX - ptrBounds.getWidth() / 2., circleY - ptrBounds.getHeight() / 2. + 10. + maxBounds.getHeight() + margin);

        if (iter->color() != rb_color::red)
            max.draw(ptrPen);
        else
            max.draw(Cairo::Colors::Yellow);

        /*
        if (drawPointers)
        {
            if (iter->color() != rb_color::red)
                ptr.draw(Cairo::RGB{0xFF, 0x55, 0x55});
            else
                ptr.draw(Cairo::Colors::Yellow);
        }
        */
    }
//---------------------------------------------------------------------------------------------------------------------
    template <typename iterT>
    void drawiter(Cairo::DrawContext* ctx, interval_tree_iterator <iterT> const& iter, double x, double y, bool drawPointers)
    {
        drawiter(ctx, iter, x, y, drawPointers);
    }
//---------------------------------------------------------------------------------------------------------------------
    TreeGrid createGrid(int_tree_for_draw const& tree)
    {
        auto root = tree.root();
        if (root == std::end(tree))
            return {};

        TreeGrid grid;

        struct _GridPoint
        {
            const_int_tree_iterator iter;
            const_int_tree_iterator parent;
            int x;
            int y;
        };

        std::vector <_GridPoint> gridPoints;

        std::function <int(const_int_tree_iterator iter)> subtreeSize;
        subtreeSize = [&](const_int_tree_iterator iter) {
            if (iter == std::end(tree))
                return 0;
            if (iter.left() == std::end(tree) && iter.right() == std::end(tree))
                return 1;
            if (iter.right() != std::end(tree) && iter.left() != std::end(tree))
                return subtreeSize(iter.right()) + subtreeSize(iter.left()) + 2;
            if (iter.right() != std::end(tree))
                return subtreeSize(iter.right()) + 1;
            if (iter.left() != std::end(tree))
                return subtreeSize(iter.left()) + 1;
            return 0;
        };

        std::function <void(const_int_tree_iterator, int pX, int pY)> deduceCoordinates;
        deduceCoordinates = [&](const_int_tree_iterator iter, int pX, int pY)
        {
            int y = pY;
            int x = pX;
            if (!iter->is_root())
            {
                ++y;
                if (iter->is_right())
                    x = x + subtreeSize(iter.left()) + 1;
                else // is_left
                    x = x - subtreeSize(iter.right()) - 1;
            }
            gridPoints.push_back({iter, iter.parent(), x, y});

            if (iter.left() == iter || iter.right() == iter)
            {
                gridPoints.push_back({iter, iter.parent(), 10, 10});
                return;
            }

            if (iter.left() != std::end(tree) && iter.left() != iter)
                deduceCoordinates(iter.left(), x, y);
            if (iter.right() != std::end(tree) && iter.right() != iter)
                deduceCoordinates(iter.right(), x, y);
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
            std::pair <int, int> parentCoords = {-1, -1};
            for (auto const& j : gridPoints)
            {
                if (j.iter == i.parent)
                {
                    parentCoords = {j.x - grid.xMin, j.y};
                    break;
                }
            }

            grid.grid[i.y][i.x + -grid.xMin] = {TreeGriditer{i.iter, parentCoords}};
        }

        return grid;
    }
//---------------------------------------------------------------------------------------------------------------------
    void drawGrid(Cairo::DrawContext* ctx, TreeGrid const& grid, bool drawPointers, bool drawEmpty)
    {
        auto iterRadius = getiterRadius();
        auto cellSize = iterRadius * 2. + gridMargin;

        auto iterX = [&](int x_) {return leftPadding + iterRadius + x_ * cellSize + x_ * xPadding;};
        auto iterY = [&](int y_) {return topPadding + iterRadius + y_ * cellSize + y_ * yPadding;};

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
                        iterX(x),
                        iterY(y),
                        iterX(cell.get().parentCoords.first),
                        iterY(cell.get().parentCoords.second)
                    };
                    line.draw(edgePen);
                }
                ++x;
            }
            ++y;
        }
        y = 0;

        // Draw iters
        for (auto const& row : grid.grid)
        {
            int x = 0;
            for (auto const& cell : row)
            {
                if (cell)
                {
                    auto iter = cell.get().iter;
                    drawIter(ctx, iter, iterX(x), iterY(y), drawPointers);
                }
                else if (drawEmpty)
                {
                    Cairo::Arc circle {
                        ctx,
                        iterX(x),
                        iterY(y),
                        iterRadius
                    };
                    circle.draw(blackPen, Cairo::Colors::White);

                }
                ++x;
            }
            ++y;
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    Cairo::Surface createSurface(TreeGrid const& grid)
    {
        auto iterRadius = getiterRadius();
        auto cellSize = iterRadius * 2. + gridMargin;
        int width = (grid.xMax - grid.xMin) + 1;
        int height = grid.yMax + 1;

        return {
            static_cast <int> (leftPadding + (width) * cellSize + (width-1) * xPadding + rightPadding),
            static_cast <int> (topPadding + (height) * cellSize + (height-1) * yPadding + bottomPadding)
        };
    }
//---------------------------------------------------------------------------------------------------------------------
    template <typename... List>
    void drawTree(std::string const& fileName, lib_interval_tree::interval_tree <List...> const* tree, bool drawPointers = false, bool drawEmpty = false)
    {
        auto grid = createGrid(*tree);
        auto surface = createSurface(grid);
        Cairo::DrawContext ctx(&surface);
        drawGrid(&ctx, grid, drawPointers, drawEmpty);
        surface.saveToFile(fileName);
    }
//######################################################################################################
}
