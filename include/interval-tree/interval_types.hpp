#pragma once

#include "feature_test.hpp"

#include <cmath>
#include <algorithm>
#include <utility>
#include <type_traits>

namespace lib_interval_tree
{
    // (]
    struct left_open
    {
        template <typename numerical_type>
        static inline bool within(numerical_type low, numerical_type high, numerical_type p)
        {
            return (low < p) && (p <= high);
        }

        template <typename numerical_type>
        static inline bool overlaps(numerical_type l1, numerical_type h1, numerical_type l2, numerical_type h2)
        {
            return (l1 < h2) && (l2 <= h1);
        }

        template <typename numerical_type>
        static inline numerical_type size(numerical_type low, numerical_type high)
        {
            return high - low;
        }
    };
    // [)
    struct right_open
    {
        template <typename numerical_type>
        static inline bool within(numerical_type low, numerical_type high, numerical_type p)
        {
            return (low <= p) && (p < high);
        }

        template <typename numerical_type>
        static inline bool overlaps(numerical_type l1, numerical_type h1, numerical_type l2, numerical_type h2)
        {
            return (l1 <= h2) && (l2 < h1);
        }

        template <typename numerical_type>
        static inline numerical_type size(numerical_type low, numerical_type high)
        {
            return high - low;
        }
    };
    // []
    struct closed
    {
        template <typename numerical_type>
        static inline bool within(numerical_type low, numerical_type high, numerical_type p)
        {
            return (low <= p) && (p <= high);
        }

        template <typename numerical_type>
        static inline bool overlaps(numerical_type l1, numerical_type h1, numerical_type l2, numerical_type h2)
        {
            return (l1 <= h2) && (l2 <= h1);
        }

        template <typename numerical_type>
        static inline typename std::enable_if<!std::is_floating_point<numerical_type>::value, numerical_type>::type
        size(numerical_type low, numerical_type high)
        {
            return high - low + 1;
        }

        template <typename numerical_type>
#ifdef LIB_INTERVAL_TREE_CONCEPTS
        requires std::is_floating_point_v<numerical_type>
        static inline numerical_type
#else
        static inline typename std::enable_if<std::is_floating_point<numerical_type>::value, numerical_type>::type
#endif
        size(numerical_type low, numerical_type high)
        {
            return high - low;
        }
    };
    // ()
    struct open
    {
        template <typename numerical_type>
        static inline bool within(numerical_type low, numerical_type high, numerical_type p)
        {
            return (low < p) && (p < high);
        }

        template <typename numerical_type>
        static inline bool overlaps(numerical_type l1, numerical_type h1, numerical_type l2, numerical_type h2)
        {
            return (l1 < h2) && (l2 < h1);
        }

        template <typename numerical_type>
        static inline typename std::enable_if<!std::is_floating_point<numerical_type>::value, numerical_type>::type
        size(numerical_type low, numerical_type high)
        {
            return high - low - 1;
        }

        template <typename numerical_type>
#ifdef LIB_INTERVAL_TREE_CONCEPTS
        requires std::is_floating_point_v<numerical_type>
        static inline numerical_type
#else
        static inline typename std::enable_if<std::is_floating_point<numerical_type>::value, numerical_type>::type
#endif
        size(numerical_type low, numerical_type high)
        {
            return high - low;
        }
    };
    /// [] and adjacent counts as overlapping
    struct closed_adjacent
    {
        template <typename numerical_type>
#ifdef LIB_INTERVAL_TREE_CONCEPTS
        requires std::is_integral_v<numerical_type>
#endif
        static inline bool within(numerical_type low, numerical_type high, numerical_type p)
        {
            return (low <= p) && (p <= high);
        }

        template <typename numerical_type>
#ifdef LIB_INTERVAL_TREE_CONCEPTS
        requires std::is_integral_v<numerical_type>
#endif
        static inline bool overlaps(numerical_type l1, numerical_type h1, numerical_type l2, numerical_type h2)
        {
            return (l1 <= (h2 + 1)) && ((l2 - 1) <= h1);
        }

        template <typename numerical_type>
#ifdef LIB_INTERVAL_TREE_CONCEPTS
        requires std::is_integral_v<numerical_type>
#endif
        static inline numerical_type size(numerical_type low, numerical_type high)
        {
            return high - low + 1;
        }
    };

    enum class interval_border
    {
        closed,
        open,
        closed_adjacent
    };

    /**
     * @brief Do not use for floating point types
     */
    class dynamic
    {
      public:
        template <typename interval_type>
#ifdef LIB_INTERVAL_TREE_CONCEPTS
        requires std::is_integral_v<typename interval_type::value_type>
#endif
        static inline bool within(interval_type const& ival, typename interval_type::value_type p)
        {
            switch (ival.left_border())
            {
                case interval_border::open:
                {
                    if (ival.low() >= p)
                    {
                        return false;
                    }
                    break;
                }
                case interval_border::closed_adjacent:
                    LIB_INTERVAL_TREE_FALLTHROUGH;
                case interval_border::closed:
                {
                    if (ival.low() > p)
                    {
                        return false;
                    }
                    break;
                }
            }
            switch (ival.right_border())
            {
                case interval_border::open:
                {
                    if (p >= ival.high())
                    {
                        return false;
                    }
                    break;
                }
                case interval_border::closed_adjacent:
                    LIB_INTERVAL_TREE_FALLTHROUGH;
                case interval_border::closed:
                {
                    if (p > ival.high())
                    {
                        return false;
                    }
                    break;
                }
            }
            return true;
        }

        template <typename interval_type>
#ifdef LIB_INTERVAL_TREE_CONCEPTS
        requires std::is_integral_v<typename interval_type::value_type>
#endif
        static inline bool overlaps(interval_type const& ival1, interval_type const& ival2)
        {
            const auto lowToClosed = [](auto const& ival) {
                if (ival.left_border() == interval_border::open)
                    return ival.low() + 1;
                return ival.low();
            };

            const auto highToClosed = [](auto const& ival) {
                if (ival.right_border() == interval_border::open)
                    return ival.high() - 1;
                return ival.high();
            };

            const interval_type closedEquiv1{
                lowToClosed(ival1), highToClosed(ival1), interval_border::closed, interval_border::closed
            };
            const interval_type closedEquiv2{
                lowToClosed(ival2), highToClosed(ival2), interval_border::closed, interval_border::closed
            };

            auto closedOverlap =
                closed::overlaps(closedEquiv1.low(), closedEquiv1.high(), closedEquiv2.low(), closedEquiv2.high());
            if (!closedOverlap)
            {
                if (closedEquiv1.high() + 1 == closedEquiv2.low() &&
                    (ival1.right_border() == interval_border::closed_adjacent ||
                     ival2.left_border() == interval_border::closed_adjacent))
                {
                    return true;
                }
                if (closedEquiv2.high() + 1 == closedEquiv1.low() &&
                    (ival2.right_border() == interval_border::closed_adjacent ||
                     ival1.left_border() == interval_border::closed_adjacent))
                {
                    return true;
                }
                return false;
            }
            return true;
        }

        template <typename interval_type>
#ifdef LIB_INTERVAL_TREE_CONCEPTS
        requires std::is_integral_v<typename interval_type::value_type>
#endif
        static typename interval_type::value_type distance(interval_type const& ival1, interval_type const& ival2)
        {
            using value_type = typename interval_type::value_type;

            if (overlaps(ival1, ival2))
                return 0;

            value_type adjusted_low = ival1.left_border() == interval_border::open ? ival1.low() + 1 : ival1.low();
            value_type adjusted_high = ival1.right_border() == interval_border::open ? ival1.high() - 1 : ival1.high();
            value_type other_adjusted_low =
                ival2.left_border() == interval_border::open ? ival2.low() + 1 : ival2.low();
            value_type other_adjusted_high =
                ival2.right_border() == interval_border::open ? ival2.high() - 1 : ival2.high();

            if (adjusted_high < other_adjusted_low)
                return other_adjusted_low - adjusted_high;
            return adjusted_low - other_adjusted_high;
        }

        static interval_border border_promote(interval_border border1, interval_border border2)
        {
            if (border1 == interval_border::closed_adjacent || border2 == interval_border::closed_adjacent)
                return interval_border::closed_adjacent;
            if (border1 == interval_border::closed || border2 == interval_border::closed)
                return interval_border::closed;
            return interval_border::open;
        }

        /**
         * @brief Assumes that ival1 and ival2 overlap and both intervals are dynamic
         *
         * @tparam interval_type
         * @param ival1
         * @param ival2
         * @return interval_type
         */
        template <typename interval_type>
#ifdef LIB_INTERVAL_TREE_CONCEPTS
        requires std::is_integral_v<typename interval_type::value_type>
#endif
        static interval_type join(interval_type const& ival1, interval_type const& ival2)
        {
            typename interval_type::value_type low;
            typename interval_type::value_type high;
            interval_border left;
            interval_border right;

            const bool ival1LeftAnyClosed = ival1.left_border() == interval_border::closed_adjacent ||
                ival1.left_border() == interval_border::closed;
            const bool ival2LeftAnyClosed = ival2.left_border() == interval_border::closed_adjacent ||
                ival2.left_border() == interval_border::closed;
            const bool ival1RightAnyClosed = ival1.right_border() == interval_border::closed_adjacent ||
                ival1.right_border() == interval_border::closed;
            const bool ival2RightAnyClosed = ival2.right_border() == interval_border::closed_adjacent ||
                ival2.right_border() == interval_border::closed;

            // same border means trivial min/max:
            if (ival1.left_border() == ival2.left_border() || (ival1LeftAnyClosed && ival2LeftAnyClosed))
            {
                left = border_promote(ival1.left_border(), ival2.left_border());
                low = std::min(ival1.low(), ival2.low());
            }
            else
            {
                auto* leftOpenInterval = ival1.left_border() == interval_border::open ? &ival1 : &ival2;
                auto* leftClosedInterval = (ival1.left_border() == interval_border::closed ||
                                            ival1.left_border() == interval_border::closed_adjacent)
                    ? &ival1
                    : &ival2;

                const auto openAdjusted = leftOpenInterval->low() + 1;

                if (openAdjusted == leftClosedInterval->low())
                {
                    left = leftClosedInterval->left_border();
                    low = leftClosedInterval->low();
                }
                else if (leftOpenInterval->low() < leftClosedInterval->low())
                {
                    left = leftOpenInterval->left_border();
                    low = leftOpenInterval->low();
                }
                else
                {
                    left = leftClosedInterval->left_border();
                    low = leftClosedInterval->low();
                }
            }

            if (ival1.right_border() == ival2.right_border() || (ival1RightAnyClosed && ival2RightAnyClosed))
            {
                right = border_promote(ival1.right_border(), ival2.right_border());
                high = std::max(ival1.high(), ival2.high());
            }
            else
            {
                auto* rightOpenInterval = ival1.right_border() == interval_border::open ? &ival1 : &ival2;
                auto* rightClosedInterval = (ival1.right_border() == interval_border::closed ||
                                             ival1.right_border() == interval_border::closed_adjacent)
                    ? &ival1
                    : &ival2;

                const auto openAdjusted = rightOpenInterval->high() - 1;

                if (openAdjusted == rightClosedInterval->high())
                {
                    right = rightClosedInterval->right_border();
                    high = rightClosedInterval->high();
                }
                else if (rightOpenInterval->high() > rightClosedInterval->high())
                {
                    right = rightOpenInterval->right_border();
                    high = rightOpenInterval->high();
                }
                else
                {
                    right = rightClosedInterval->right_border();
                    high = rightClosedInterval->high();
                }
            }

            return interval_type{low, high, left, right};
        }

        template <typename interval_type>
#ifdef LIB_INTERVAL_TREE_CONCEPTS
        requires std::is_integral_v<typename interval_type::value_type>
#endif
        typename interval_type::value_type size(interval_type const& ival)
        {
            auto left = ival.left_border();
            if (left == interval_border::closed_adjacent)
                left = interval_border::closed;
            auto right = ival.right_border();
            if (right == interval_border::closed_adjacent)
                right = interval_border::closed;

            if (left == right)
            {
                return left == interval_border::open ? open::size(ival.low(), ival.high())
                                                     : closed::size(ival.low(), ival.high());
            }
            if (left == interval_border::open)
                return left_open::size(ival.low(), ival.high());
            return right_open::size(ival.low(), ival.high());
        }
    };
}
