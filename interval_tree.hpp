#pragma once

#include <memory>
#include <cassert>
#include <cstdio>
#include <stdexcept>
#include <iterator>

namespace lib_interval_tree
{
//############################################################################################################
    // (]
    struct left_open
    {
        template <typename numerical_type>
        static inline bool within(numerical_type b, numerical_type e, numerical_type p)
        {
            return b < p <= e;
        }
    };
    // [)
    struct right_open
    {
        template <typename numerical_type>
        static inline bool within(numerical_type b, numerical_type e, numerical_type p)
        {
            return b <= p < e;
        }
    };
    // []
    struct closed
    {
        template <typename numerical_type>
        static inline bool within(numerical_type b, numerical_type e, numerical_type p)
        {
            return b <= p <= e;
        }
    };
    // ()
    struct open
    {
        template <typename numerical_type>
        static inline bool within(numerical_type b, numerical_type e, numerical_type p)
        {
            return b < p < e;
        }
    };
//############################################################################################################
    using default_interval_value_type = int;

    template <typename numerical_type, typename interval_kind_>
    struct interval;

    template <typename numerical_type, typename interval_kind>
    class interval_tree;

    template <typename numerical_type, typename interval_type>
    class node;

    template <typename node_type>
    class interval_tree_iterator;
//############################################################################################################
    template <typename numerical_type, typename interval_kind_>
    struct interval
    {
        friend node <numerical_type, interval <numerical_type, interval_kind_>>;

    public:
        using value_type = numerical_type;
        using interval_kind = interval_kind_;

        /**
         *  Constructs an interval. low MUST be smaller than high.
         */
#ifndef SAVE_INTERVALS
        interval(value_type low, value_type high)
            : low_{low}
            , high_{high}
        {
            assert(low < high);
        }
#else
        interval(value_type low, value_type high)
            : low_{std::min(low, high)}
            , high_{std::max(low, high)}
        {
        }
#endif

        /**
         *  Returns if both intervals equal.
         */
        bool operator==(interval const& other)
        {
            return low_ == other.low_ && high_ == other.high;
        }

        /**
         *  Returns the lower bound of the interval
         */
        value_type low() const
        {
            return low_;
        }

        /**
         *  Returns the upper bound of the interval
         */
        value_type high() const
        {
            return high_;
        }

        /**
         *  Returns whether the intervals overlap
         */
        bool overlaps(value_type l, value_type h) const
        {
            return low_ < h && l < high_;
        }

        /**
         *  Returns whether the intervals overlap
         */
        bool overlaps(interval const& other) const
        {
            return overlaps(other.low_, other.high_);
        }

        /**
         *  Returns whether the given value is in this.
         */
        bool within(value_type value) const
        {
            return interval_kind::within(value);
        }

        /**
         *  Returns whether the given interval is in this.
         */
        bool within(interval const& other) const
        {
            return low_ <= other.low_ && high_ >= other.high_;
        }

        /**
         *  Calculates the distance between the two intervals.
         *  Overlapping intervals have 0 distance.
         */
        value_type operator-(interval const& other) const
        {
            if (overlaps(other))
                return 0;
            if (high_ < other.low)
                return other.low_ - high_;
            else
                return low_ - other.high_;
        }

        /**
         *  Returns the size of the interval.
         */
        value_type size() const
        {
            return high_ - low_;
        }

        /**
         *  Creates a new interval from this and other, that contains both intervals and whatever
         *  is between.
         */
        interval join(interval const& other) const
        {
            return {std::min(low_, other.low_), std::max(high_, other.high_)};
        }

    private:
        value_type low_;
        value_type high_;
    };
//############################################################################################################
    template <typename numerical_type = default_interval_value_type, typename interval_type_ = interval <numerical_type, right_open>>
    class node
    {
    public:
        using interval_type = interval_type_;
        using value_type = numerical_type;

    public:
        friend interval_tree <numerical_type, typename interval_type::interval_kind>;
        friend interval_tree_iterator <node <value_type, interval_type>>;

        template <typename node_type, typename interval_type__>
        friend void insert_node(node_type* parent, std::unique_ptr <node_type>& root, interval_type__ ival)
        {
            if (!root)
            {
                root = std::make_unique <node>(parent, ival);
                return;
            }

            auto root_begin = root->interval().low();
            if (ival.low() < root_begin)
                insert_node(root.get(), root->left_, ival);
            else
                insert_node(root.get(), root->right_, ival);

            if (root->max_ < ival.high())
                root->max_ = ival.high();
        }

    public:
        node(node* parent, interval_type interval)
            : interval_{std::move(interval)}
            , max_{interval.high()}
            , parent_{parent}
            , left_{}
            , right_{}
        {
        }

        interval_type interval() const
        {
            return interval_;
        }

        value_type max() const
        {
            return max_;
        }

    private:
        interval_type interval_;
        value_type max_;
        node* parent_;
        std::unique_ptr <node> left_;
        std::unique_ptr <node> right_;
    };
//############################################################################################################
    template <typename node_type>
    class interval_tree_iterator : public std::forward_iterator_tag
    {
    public:
        friend interval_tree <typename node_type::interval_type::value_type, typename node_type::interval_type::interval_kind>;

        using tree_type = interval_tree <typename node_type::interval_type::value_type, typename node_type::interval_type::interval_kind>;
        using value_type = node_type;

    public:
        constexpr interval_tree_iterator(interval_tree_iterator const&) = default;
        constexpr interval_tree_iterator& operator=(interval_tree_iterator const&) = default;

        interval_tree_iterator& operator++()
        {
            if (!node_)
            {
                node_ = owner_->root_.get();

                if (!node_)
                    return *this;

                while(node_->left_)
                    node_ = node_->left_.get();
            }

            if (node_->right_)
            {
                node_ = node_->right_.get();

                while (node_->left_)
                    node_ = node_->left_.get();
            }
            else
            {
                auto* parent = node_->parent_;
                while (parent != nullptr && node_ == parent->right_.get())
                {
                    node_ = parent;
                    parent = parent->parent_;
                }
                node_ = parent;
            }
            return *this;
        }

        interval_tree_iterator operator++(int)
        {
            interval_tree_iterator cpy = *this;
            operator++();
            return cpy;
        }

        bool operator!=(interval_tree_iterator const& other)
        {
            return node_ != other.node_;
        }

        bool operator==(interval_tree_iterator const& other)
        {
            return node_ == other.node_;
        }

        typename value_type::interval_type operator*() const
        {
            if (node_)
                return node_->interval();
            else
                throw std::out_of_range("interval_tree_iterator out of bounds");
        }

        value_type const* operator->() const
        {
            return node_;
        }

    private:
        interval_tree_iterator(node_type* node, tree_type const* owner)
            : node_{node}
            , owner_{owner}
        {
        }

    private:
        node_type* node_;
        tree_type const* owner_;
    };
//############################################################################################################
    template <typename numerical_type = default_interval_value_type, typename interval_kind = right_open>
    class interval_tree
    {
    public:
        friend interval_tree_iterator <node <numerical_type, interval <numerical_type, interval_kind>>>;

    public:
        using value_type = numerical_type;
        using interval_type = interval <value_type, interval_kind>;
        using node_type = node <value_type, interval_type>;
        using iterator = interval_tree_iterator <node_type>;

        interval_tree()
            : root_{}
        {
        }

        /**
         *  Inserts an interval into the tree.
         */
        void insert(interval_type const& ival)
        {
            insert_node((node_type*)nullptr, root_, ival);
        }

        iterator erase(iterator const& iter)
        {
            if (!iter.node_)
                throw std::out_of_range("cannot erase end iterator");

            if (iter.node_ == root_.get())
            {
                root_.reset();
                return end();
            }

            auto former_node_max = iter.node_->max_;

            auto* parent = iter.node_->parent_;
            if (parent->right_ && iter.node_ == parent->right_.get())
                parent->right_.reset();
            if (parent->left_ && iter.node_ == parent->left_.get())
                parent->left_.reset();

            if (parent->max_ == former_node_max)
                recalculate_max(parent);

            return {parent, this};
        }

        iterator overlap_find(interval_type const& ival) const
        {
            auto* ptr = root_.get();
            while (ptr && !ival.overlaps(ptr->interval()))
            {
                if (ptr->left_ && ptr->left_->max() >= ival.low())
                    ptr = ptr->left_.get();
                else
                    ptr = ptr->right_.get();
            }
            return iterator{ptr, this};
        }

        iterator begin()
        {
            node_type* iter = root_.get();

            if (root_)
                while (iter->left_)
                    iter = iter->left_.get();

            return{iter, this};
        }
        iterator end()
        {
            return {nullptr, this};
        }
    private:
        void recalculate_max(node_type* reacalculation_root)
        {
            auto p = reacalculation_root;
            while (p && p->max_ <= reacalculation_root->max_)
            {
                if (p->left_ && p->left_->max_ < p->max_)
                    p->max_ = p->left_->max_;
                else if (p->right_ && p->right_->max_ < p->max_)
                    p->max_ = p->right_->max_;
                p = p->parent_;
            }
        }

    private:
        std::unique_ptr <node_type> root_;
    };
//############################################################################################################
}
