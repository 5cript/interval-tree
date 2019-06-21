#pragma once

template <typename ContainedT>
struct IntervalTypes
{
    using value_type = ContainedT;
    using interval_type = lib_interval_tree::interval <ContainedT>;
    using tree_type = lib_interval_tree::interval_tree <interval_type>;
    using iterator_type = typename tree_type::iterator;
};
