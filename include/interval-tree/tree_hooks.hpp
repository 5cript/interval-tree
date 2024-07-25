#pragma once

namespace lib_interval_tree::hooks
{
    struct no_hook_state
    {};

    struct regular
    {
        using node_type = void;
        using size_type = void;
        using hook_state = no_hook_state;

        template <typename tree_type>
        static inline void on_destroy(tree_type& tree) noexcept
        {}

        template <typename tree_type>
        static inline void on_before_insert_fixup(tree_type& tree, typename tree_type::node_type* node) noexcept
        {}

        template <typename tree_type>
        static inline void on_after_insert_fixup(tree_type& tree, typename tree_type::node_type* node) noexcept
        {}

        template <typename tree_type>
        static inline void on_before_erase_fixup(
            tree_type& tree,
            typename tree_type::node_type* node,
            typename tree_type::node_type* node_parent,
            bool other_is_left_child
        ) noexcept
        {}

        template <typename tree_type>
        static inline void on_after_erase_fixup(
            tree_type& tree,
            typename tree_type::node_type* node,
            typename tree_type::node_type* node_parent,
            bool other_is_left_child
        ) noexcept
        {}

        template <typename tree_type>
        static inline void on_before_recalculate_max(tree_type& tree, typename tree_type::node_type* node) noexcept
        {}

        template <typename tree_type>
        static inline void on_after_recalculate_max(tree_type& tree, typename tree_type::node_type* node) noexcept
        {}

        template <typename tree_type, typename compare_function_type>
        static inline void on_find(
            tree_type const& tree,
            typename tree_type::node_type* node,
            typename tree_type::interval_type const& ival,
            compare_function_type compare
        ) noexcept
        {}

        template <typename tree_type, typename compare_function_type>
        static inline void on_find_all(
            tree_type const& tree,
            typename tree_type::node_type* node,
            typename tree_type::interval_type const& ival,
            compare_function_type compare
        ) noexcept
        {}

        template <typename tree_type>
        static inline void on_overlap_find(
            tree_type const& tree,
            typename tree_type::node_type* node,
            typename tree_type::interval_type const& ival
        ) noexcept
        {}

        template <typename tree_type>
        static inline void on_overlap_find_all(
            tree_type const& tree,
            typename tree_type::node_type* node,
            typename tree_type::interval_type const& ival
        ) noexcept
        {}
    };
}