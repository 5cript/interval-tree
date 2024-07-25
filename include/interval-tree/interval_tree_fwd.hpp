#pragma once

namespace lib_interval_tree
{
    template <typename numerical_type, typename interval_kind_, typename tree_hooks>
    struct interval;

    template <typename IntervalT, typename tree_hooks>
    class interval_tree;

    template <typename numerical_type, typename interval_type, typename tree_hooks>
    class node;

    template <typename node_type, typename owner_type>
    class basic_interval_tree_iterator;

    template <typename node_type, bool reverse>
    class const_interval_tree_iterator;

    template <typename node_type, bool reverse>
    class interval_tree_iterator;
}
