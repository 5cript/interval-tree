#pragma once

#include <interval-tree/interval_types.hpp>
#include <algorithm>
#include <vector>

template <typename numerical_type, typename interval_kind_ = lib_interval_tree::closed>
struct multi_join_interval
{
public:
    using value_type = numerical_type;
    using interval_kind = interval_kind_;

#ifndef INTERVAL_TREE_SAFE_INTERVALS
#if __cplusplus >= 201703L
    constexpr
#endif
    multi_join_interval(value_type low, value_type high)
        : low_{low}
        , high_{high}
    {
        if (low > high)
            throw std::invalid_argument("Low border is not lower or equal to high border.");
    }
#else
#if __cplusplus >= 201703L
    constexpr
#endif
    multi_join_interval(value_type low, value_type high)
        : low_{std::min(low, high)}
        , high_{std::max(low, high)}
    {
    }
#endif
    virtual ~multi_join_interval() = default;
    friend bool operator==(multi_join_interval const& lhs, multi_join_interval const& other)
    {
        return lhs.low_ == other.low_ && lhs.high_ == other.high_;
    }
    friend bool operator!=(multi_join_interval const& lhs, multi_join_interval const& other)
    {
        return lhs.low_ != other.low_ || lhs.high_ != other.high_;
    }
    value_type low() const
    {
        return low_;
    }
    value_type high() const
    {
        return high_;
    }
    bool overlaps(value_type l, value_type h) const
    {
        return low_ <= h && l <= high_;
    }
    bool overlaps_exclusive(value_type l, value_type h) const
    {
        return low_ < h && l < high_;
    }
    bool overlaps(multi_join_interval const& other) const
    {
        return overlaps(other.low_, other.high_);
    }
    bool overlaps_exclusive(multi_join_interval const& other) const
    {
        return overlaps_exclusive(other.low_, other.high_);
    }
    bool within(value_type value) const
    {
        return interval_kind::within(low_, high_, value);
    }
    bool within(multi_join_interval const& other) const
    {
        return low_ <= other.low_ && high_ >= other.high_;
    }
    value_type operator-(multi_join_interval const& other) const
    {
        if (overlaps(other))
            return 0;
        if (high_ < other.low_)
            return other.low_ - high_;
        else
            return low_ - other.high_;
    }
    value_type size() const
    {
        return high_ - low_;
    }
    std::vector<multi_join_interval> join(multi_join_interval const& other) const
    {
        const auto min = std::min(low_, other.low_);
        const auto max = std::max(high_, other.high_);
        const auto avg = (min + max) / 2;
        return {
            {min, avg},
            {avg, max},
        };
    }

protected:
    value_type low_;
    value_type high_;
};