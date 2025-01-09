// #include <interval-tree/draw.hpp> // to draw tree. this is not header only anymore.
#include <interval-tree/interval_tree.hpp>

#include <iostream>

int main()
{
    using namespace lib_interval_tree;

    // interval_tree<interval<int>>; // closed by default
    // interval_tree<interval<int, open>>;
    // interval_tree<interval<int, closed>>;
    // interval_tree<interval<int, left_open>>;
    // interval_tree<interval<int, right_open>>;
    // interval_tree<interval<int, closed_adjacent>>; // counts adjacent intervals as overlapping
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

    tree.deoverlap();

    for (auto const& i : tree)
    {
        std::cout << "[" << i.low() << ", " << i.high() << "]\n";
    }

    using lib_interval_tree::open;
    // dynamic has some logic overhead.
    interval_tree<interval<int, dynamic>> dynamicIntervals;
    dynamicIntervals.insert({0, 1, interval_border::closed, interval_border::open});
    dynamicIntervals.insert({7, 5, interval_border::open, interval_border::closed_adjacent});
}