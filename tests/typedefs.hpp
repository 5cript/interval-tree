#pragma once

template <typename ContainedT>
struct IntervalTypes
{
    using value_type = ContainedT;
    using tree_type = lib_interval_tree::interval_tree <ContainedT>;
    using iterator_type = typename tree_type::iterator;
    using interval_type = typename tree_type::interval_type;
};
