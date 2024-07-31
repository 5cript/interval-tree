#pragma once

#include <interval-tree/interval_tree.hpp>

#include <iostream>

namespace lib_interval_tree
{
    template <typename... IntervalArgs>
    std::ostream&
    operator<<(std::ostream& os, lib_interval_tree::interval<IntervalArgs...> const& interval)
    {
        os << '[' << interval.low() << ", " << interval.high() << ']';
        return os;
    }
}