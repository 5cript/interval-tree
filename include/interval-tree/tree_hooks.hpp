#pragma once

namespace lib_interval_tree
{
    namespace hooks
    {
        struct no_hook_state
        {};

        struct regular
        {
            using node_type = void;
            using size_type = void;
            using hook_state = no_hook_state;

            template <typename tree_type>
            static inline void on_destroy(tree_type&) noexcept
            {}

            template <typename tree_type>
            static inline void on_after_insert(tree_type&, typename tree_type::node_type*) noexcept
            {}

            template <typename tree_type>
            static inline void on_before_insert_fixup(tree_type&, typename tree_type::node_type*) noexcept
            {}

            template <typename tree_type>
            static inline void on_after_insert_fixup(tree_type&, typename tree_type::node_type*) noexcept
            {}

            template <typename tree_type>
            static inline void on_before_erase_fixup(
                tree_type&,
                typename tree_type::node_type*,
                typename tree_type::node_type*,
                bool /*other_is_left_child*/
            ) noexcept
            {}

            template <typename tree_type>
            static inline void on_after_erase_fixup(
                tree_type&,
                typename tree_type::node_type*,
                typename tree_type::node_type*,
                bool /*other_is_left_child*/
            ) noexcept
            {}

            template <typename tree_type>
            static inline void on_before_recalculate_max(tree_type&, typename tree_type::node_type*) noexcept
            {}

            template <typename tree_type>
            static inline void on_after_recalculate_max(tree_type&, typename tree_type::node_type*) noexcept
            {}

            template <typename tree_type, typename compare_function_type>
            static inline void on_find(
                tree_type const&,
                typename tree_type::node_type*,
                typename tree_type::interval_type const&,
                compare_function_type
            ) noexcept
            {}

            template <typename tree_type, typename compare_function_type>
            static inline void on_find_all(
                tree_type const&,
                typename tree_type::node_type*,
                typename tree_type::interval_type const&,
                compare_function_type
            ) noexcept
            {}

            template <typename tree_type>
            static inline void
            on_overlap_find(tree_type const&, typename tree_type::node_type*, typename tree_type::interval_type const&) noexcept
            {}

            template <typename tree_type>
            static inline void
            on_overlap_find_all(tree_type const&, typename tree_type::node_type*, typename tree_type::interval_type const&) noexcept
            {}
        };
    }
}