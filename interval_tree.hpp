#pragma once

#include "interval_tree_fwd.hpp"
#include "interval_types.hpp"

// #include "draw.hpp"

#include <string>
#include <memory>
#include <cassert>
#include <cstdio>
#include <stdexcept>
#include <iterator>
#include <type_traits>

#include <iostream>

namespace lib_interval_tree
{
//############################################################################################################
    enum class rb_color
    {
        fail,
        red,
        black,
        double_black
    };
//############################################################################################################
    using default_interval_value_type = int;
//############################################################################################################
    template <typename numerical_type, typename interval_kind_>
    struct interval
    {
        friend node <numerical_type, interval <numerical_type, interval_kind_>>;
        friend interval_tree <numerical_type, interval_kind_>;

    public:
        using value_type = numerical_type;
        using interval_kind = interval_kind_;

        /**
         *  Constructs an interval. low MUST be smaller than high.
         */
#ifndef INTERVAL_TREE_SAFE_INTERVALS
        interval(value_type low, value_type high)
            : low_{low}
            , high_{high}
        {
            assert(low <= high);
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
        friend bool operator==(interval const& lhs, interval const& other)
        {
            return lhs.low_ == other.low_ && lhs.high_ == other.high_;
        }

        /**
         *  Returns if both intervals are different.
         */
        friend bool operator!=(interval const& lhs, interval const& other)
        {
            return lhs.low_ != other.low_ || lhs.high_ != other.high_;
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
            return low_ <= h && l <= high_;
        }

        /**
         *  Returns whether the intervals overlap, excluding border.
         */
        bool overlaps_exclusive(value_type l, value_type h) const
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
         *  Returns whether the intervals overlap, excluding border.
         */
        bool overlaps_exclusive(interval const& other) const
        {
            return overlaps_exclusive(other.low_, other.high_);
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

#ifdef PUBLICIZE
    public:
#else
    private:
#endif // PUBLICIZE
        value_type low_;
        value_type high_;
    };
//############################################################################################################
    template <typename numerical_type = default_interval_value_type, typename interval_type_ = interval <numerical_type, closed>>
    class node
    {
    private:
        using node_type = node <numerical_type, interval_type_>;

    public:
        using interval_type = interval_type_;
        using value_type = numerical_type;

    public:
        friend lib_interval_tree::interval_tree <numerical_type, typename interval_type::interval_kind>;
        friend lib_interval_tree::const_interval_tree_iterator <node <numerical_type, interval_type> >;
        friend lib_interval_tree::interval_tree_iterator <node <numerical_type, interval_type> >;

    public:
        node(node* parent, interval_type interval)
            : interval_{std::move(interval)}
            , max_{interval.high()}
            , parent_{parent}
            , left_{}
            , right_{}
            , color_{rb_color::fail}
        {
        }

        ~node()
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

        bool is_left() const noexcept
        {
            return this == parent_->left_;
        }

        bool is_right() const noexcept
        {
            return this == parent_->right_;
        }

        bool is_root() const noexcept
        {
            return !parent_;
        }

private:
        void set_interval(interval_type const& ival)
        {
            interval_ = ival;
        }

        void kill() const noexcept
        {
            auto* parent = parent_;
            if (is_left())
            {
                delete parent_->left_;
                parent->left_ = nullptr;
            }
            else
            {
                delete parent_->right_;
                parent->right_ = nullptr;
            }
        }

#ifdef PUBLICIZE
    public:
#else
    private:
#endif // PUBLICIZE
        interval_type interval_;
        value_type max_;
        node* parent_;
        node* left_;
        node* right_;
        rb_color color_;
    };
//############################################################################################################
    template <typename node_type, typename owner_type>
    class basic_interval_tree_iterator : public std::forward_iterator_tag
    {
    public:
        friend interval_tree <typename node_type::interval_type::value_type, typename node_type::interval_type::interval_kind>;

        using tree_type = interval_tree <typename node_type::interval_type::value_type, typename node_type::interval_type::interval_kind>;
        using value_type = node_type;

        using node_ptr_t = typename std::conditional <
            std::is_const <typename std::remove_pointer <owner_type>::type>::value,
            node_type const*,
            node_type*
        >::type;

    public:
        constexpr basic_interval_tree_iterator(basic_interval_tree_iterator const&) = default;
        basic_interval_tree_iterator& operator=(basic_interval_tree_iterator const&) = default;

        bool operator!=(basic_interval_tree_iterator const& other) const
        {
            return node_ != other.node_;
        }

        bool operator==(basic_interval_tree_iterator const& other) const
        {
            return node_ == other.node_;
        }

        virtual ~basic_interval_tree_iterator() = default;

    protected:
        basic_interval_tree_iterator(node_ptr_t node, owner_type owner)
            : node_{node}
            , owner_{owner}
        {
        }

#ifdef PUBLICIZE
    public:
#else
    protected:
#endif // PUBLICIZE
        node_ptr_t node_;
        owner_type owner_;
    };
//############################################################################################################
    template <typename node_type>
    class const_interval_tree_iterator
        : public basic_interval_tree_iterator <node_type,
                                               interval_tree <typename node_type::interval_type::value_type,
                                                              typename node_type::interval_type::interval_kind> const*>
    {
    public:
        using tree_type = interval_tree <typename node_type::interval_type::value_type,
                                         typename node_type::interval_type::interval_kind>;
        using iterator_base = basic_interval_tree_iterator <node_type, tree_type const*>;
        using value_type = typename iterator_base::value_type;
        using iterator_base::node_;
        using iterator_base::owner_;

        friend tree_type;

    public:
        const_interval_tree_iterator& operator++()
        {
            if (!node_)
            {
                node_ = owner_->root_;

                if (!node_)
                    return *this;

                while(node_->left_)
                    node_ = node_->left_;
            }

            if (node_->right_)
            {
                node_ = node_->right_;

                while (node_->left_)
                    node_ = node_->left_;
            }
            else
            {
                auto* parent = node_->parent_;
                while (parent != nullptr && node_ == parent->right_)
                {
                    node_ = parent;
                    parent = parent->parent_;
                }
                node_ = parent;
            }

            return *this;
        }

        const_interval_tree_iterator operator++(int)
        {
            const_interval_tree_iterator cpy = *this;
            operator++();
            return cpy;
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
        const_interval_tree_iterator(node_type const* node, tree_type const* owner)
            : basic_interval_tree_iterator <node_type, tree_type const*> {node, owner}
        {
        }
    };
//############################################################################################################
    template <typename node_type>
    class interval_tree_iterator
        : public basic_interval_tree_iterator <node_type,
                                               interval_tree <typename node_type::interval_type::value_type,
                                                              typename node_type::interval_type::interval_kind>*>
    {
    public:
        using tree_type = interval_tree <typename node_type::interval_type::value_type,
                                         typename node_type::interval_type::interval_kind>;
        using iterator_base = basic_interval_tree_iterator <node_type, tree_type*>;
        using value_type = typename iterator_base::value_type;
        using iterator_base::node_;
        using iterator_base::owner_;

        friend tree_type;

    public:
        interval_tree_iterator& operator++()
        {
            if (!node_)
            {
                node_ = owner_->root_;

                if (!node_)
                    return *this;

                while(node_->left_)
                    node_ = node_->left_;
            }

            if (node_->right_)
            {
                node_ = node_->right_;

                while (node_->left_)
                    node_ = node_->left_;
            }
            else
            {
                auto* parent = node_->parent_;
                while (parent != nullptr && node_ == parent->right_)
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

        typename value_type::interval_type operator*() const
        {
            if (node_)
                return node_->interval();
            else
                throw std::out_of_range("interval_tree_iterator out of bounds");
        }

        value_type* operator->()
        {
            return node_;
        }

    private:
        interval_tree_iterator(node_type* node, tree_type* owner)
            : basic_interval_tree_iterator <node_type, tree_type*> {node, owner}
        {
        }
    };
//############################################################################################################
    template <typename numerical_type = default_interval_value_type, typename interval_kind = closed>
    class interval_tree
    {
    public:
        using value_type = numerical_type;
        using interval_type = interval <value_type, interval_kind>;
        using node_type = node <value_type, interval_type>;
        using iterator = interval_tree_iterator <node_type>;
        using const_iterator = const_interval_tree_iterator <node_type>;

    public:
        friend const_interval_tree_iterator <node_type>;
        friend interval_tree_iterator <node_type>;

        interval_tree()
            : root_{}
        {
        }

        ~interval_tree()
        {
            clear();
        }

        interval_tree(interval_tree const& other)
            : root_{}
        {
            operator=(other);
        }

    public:
        interval_tree& operator=(interval_tree const& other)
        {
            if (!empty())
                clear();

            if (other.root_ != nullptr)
                root_ = copyTreeImpl(other.root_, nullptr);

            return *this;
        }

        /**
         *  Removes all from this tree.
         */
        void clear()
        {
            for (auto i = std::begin(*this); i != std::end(*this);)
                i = erase(i);
        }

        /**
         *  Inserts an interval into the tree.
         */
        iterator insert(interval_type const& ival)
        {
            node_type* z = new node_type(nullptr, ival);
            node_type* y = nullptr;
            node_type* x = root_;
            while (x)
            {
                y = x;
                if (z->interval_.low_ < x->interval_.low_)
                    x = x->left_;
                else
                    x = x->right_;
            }
            z->parent_ = y;
            if (!y)
                root_ = z;
            else if (z->interval_.low_ < y->interval_.low_)
                y->left_ = z;
            else
                y->right_ = z;
            z->color_ = rb_color::red;

            insert_fixup(z);
            recalculate_max(z);
            return {z, this};
        }

        /**
         *  Inserts an interval into the tree if no other interval overlaps it.
         *  Otherwise merge the interval with the being overlapped.
         *
         *  @param ival The interval
         *  @param exclusive Exclude borders.
         */
        iterator insert_overlap(interval_type const& ival, bool exclusive = false)
        {
            auto iter = overlap_find(ival, exclusive);
            if (iter == end())
                return insert(ival);
            else
            {
                auto merged = iter->interval().join(ival);
                erase(iter);
                return insert(merged);
            }
        }

        /**
         *  Erases the element pointed to be iter.
         */
        iterator erase(iterator iter)
        {
            if (!iter.node_)
                throw std::out_of_range("cannot erase end iterator");

            auto next = iter;
            ++next;

            node_type* y;
            if (!iter.node_->left_ || !iter.node_->right_)
                y = iter.node_;
            else
                y = successor(iter.node_);

            node_type* x;
            if (y->left_)
                x = y->left_;
            else
                x = y->right_;

            if (x)
                x->parent_ = y->parent_;

            auto* x_parent = y->parent_;

            if (!y->parent_)
                root_ = x;
            else if (y->is_left())
                y->parent_->left_ = x;
            else
                y->parent_->right_ = x;

            if (y != iter.node_)
            {
                iter.node_->interval_ = y->interval_;
                iter.node_->max_ = y->max_;
                recalculate_max(iter.node_);
            }

            if (x && x->color_ == rb_color::red)
            {
                if (x_parent)
                    erase_fixup(x, x_parent, y->is_left());
                else
                    x->color_ = rb_color::black;
            }

            delete y;

            return next;
        }

        /**
         *  Finds the next exact match INCLUDING from.
         *
         *  @param from The iterator to search from INCLUSIVE!
         *  @param ival The interval to find an exact match for within the tree.
         *  @param compare A comparison function to use.
         */
        template <typename CompareFunctionT>
        iterator find_next(iterator from, interval_type const& ival, CompareFunctionT const& compare)
        {
            if (root_ == nullptr)
                return end();
            return iterator{find_i(from.node_, ival, compare), this};
        }

        /**
         *  Finds the next exact match INCLUDING from.
         *
         *  @param from The iterator to search from, INCLUSIVE!
         *  @param ival The interval to find an exact match for within the tree.
         *  @param compare A comparison function to use.
         */
        iterator find_next(iterator from, interval_type const& ival)
        {
            return find_next(from, ival, [](auto const& lhs, auto const& rhs){return lhs == rhs;});
        }

        /**
         *  Finds the first interval that overlaps with ival.
         *
         *  @param ival The interval to find an overlap for within the tree.
         *  @param exclusive Exclude edges?
         */
        iterator overlap_find(interval_type const& ival, bool exclusive = false)
        {
            if (root_ == nullptr)
                return end();
            return iterator{overlap_find_i(root_, ival, exclusive), this};
        }

        /**
         *  Finds the next interval that overlaps with ival INCLUDING from.
         *
         *  @param from The iterator to start from, INCLUSIVE!
         *  @param ival The interval to find an overlap for within the tree.
         *  @param exclusive Exclude edges?
         */
        iterator overlap_find_next(iterator from, interval_type const& ival, bool exclusive = false)
        {
            if (root_ == nullptr)
                return end();
            return iterator{overlap_find_i(from.node_, ival, exclusive), this};
        }

        /**
         *  Finds the first exact match.
         *
         *  @param ival The interval to find an exact match for within the tree.
         *  @param compare A comparison function to use.
         */
        template <typename CompareFunctionT>
        iterator find(interval_type const& ival, CompareFunctionT const& compare)
        {
            if (root_ == nullptr)
                return end();
            return iterator{find_i(root_, ival, compare), this};
        }

        /**
         *  Finds the first exact match.
         *
         *  @param ival The interval to find an exact match for within the tree.
         */
        iterator find(interval_type const& ival)
        {
            return find(ival, [](auto const& lhs, auto const& rhs){return lhs == rhs;});
        }

        /**
         *  Deoverlaps the tree but returns it as a copy.
         */
        interval_tree deoverlap_copy()
        {
            interval_tree fresh;
            for (auto i = begin(), e = end(); i != e; ++i)
                fresh.insert_overlap(*i);

            return fresh;
        }

        /**
         *  Merges all overlapping intervals by erasing overlapping intervals and reinserting the merged interval.
         */
        interval_tree& deoverlap()
        {
            *this = deoverlap_copy();
            return *this;
        }

        /**
         *  Only works with deoverlapped trees.
         *  Creates an interval tree that contains all gaps between the intervals as intervals.
         */
        interval_tree punch() const
        {
            if (empty())
                return {};
            auto min = std::begin(*this)->interval().low();
            auto max = root_->max_;
            return punch({min, max});
        }

        /**
         *  Only works with deoverlapped trees.
         *  Removes all intervals from the given interval and produces a tree that contains the remaining intervals.
         *  This is basically the other punch overload with ival = [tree_lowest, tree_highest]
         */
        interval_tree punch(interval_type const& ival) const
        {
            if (empty())
                return {};

            interval_tree result;
            auto i = std::begin(*this);
            if (ival.low() < i->interval().low())
                result.insert({ival.low(), i->interval().low()});

            for (auto e = end(); i != e; ++i)
            {
                auto next = i; ++next;
                if (next != e)
                    result.insert({i->interval().high(), next->interval().low()});
                else
                    break;
            }

            if (i != end() && i->interval().high() < ival.high())
                result.insert({i->interval().high(), ival.high()});

            return result;
        }

        iterator begin()
        {
            if (!root_)
                return {nullptr, this};

            auto* iter = root_;

            while (iter->left_)
                iter = iter->left_;

            return{iter, this};
        }
        iterator end()
        {
            return {nullptr, this};
        }

        const_iterator cbegin() const
        {
            if (!root_)
                return {nullptr, this};

            auto* iter = root_;

            while (iter->left_)
                iter = iter->left_;

            return const_iterator{iter, this};
        }
        const_iterator cend() const
        {
            return const_iterator{nullptr, this};
        }
        const_iterator begin() const
        {
            return cbegin();
        }
        const_iterator end() const
        {
            return cend();
        }

        /**
         *  Returns wether or not the tree is empty
         */
        bool empty() const noexcept
        {
            return root_ == nullptr;
        }

    private:
        node_type* copyTreeImpl(node_type* root, node_type* parent)
        {
            if (root)
            {
                auto* cpy = new node_type(parent, root->interval());
                cpy->color_ = root->color_;
                cpy->max_ = root->max_;
                cpy->left_ = copyTreeImpl(root->left_, cpy);
                cpy->right_ = copyTreeImpl(root->right_, cpy);
                return cpy;
            }
            return nullptr;
        };

        template <typename ComparatorFunctionT>
        node_type* find_i(node_type* ptr, interval_type const& ival, ComparatorFunctionT const& compare)
        {
            if (compare(ptr->interval(), ival))
                return ptr;
            else
            {
                if (ptr->left_ && ival.high() <= ptr->left_->max())
                {
                    // no right? can only continue left
                    if (!ptr->right_)
                        return find_i(ptr->left_, ival, compare);

                    // upper bounds higher than what is contained right? continue left
                    if (ival.high() > ptr->right_->max())
                        return find_i(ptr->left_, ival, compare);

                    auto* res = find_i(ptr->left_, ival, compare);
                    if (res == nullptr)
                        return find_i(ptr->right_, ival, compare);
                    else
                        return res;
                }
                if (ptr->right_ && ival.high() <= ptr->right_->max())
                {
                    if (!ptr->left_)
                        return find_i(ptr->right_, ival, compare);

                    if (ival.high() > ptr->left_->max())
                        return find_i(ptr->right_, ival, compare);

                    auto* res = find_i(ptr->right_, ival, compare);
                    if (res == nullptr)
                        return find_i(ptr->left_, ival, compare);
                    else
                        return res;
                }
                else
                    return nullptr;
            }
        }

        node_type* overlap_find_i(node_type* ptr, interval_type const& ival, bool exclusive)
        {
            if (exclusive)
            {
                if (ptr->interval().overlaps_exclusive(ival))
                    return ptr;
            }
            else
            {
                if (ptr->interval().overlaps(ival))
                    return ptr;
            }

            if (ptr->left_ && ptr->left_->max() >= ival.low())
            {
                // no right? can only continue left
                if (!ptr->right_)
                    return overlap_find_i(ptr->left_, ival, exclusive);

                // upper bounds higher than what is contained right? continue left
                if (ival.high() > ptr->right_->max())
                    return overlap_find_i(ptr->left_, ival, exclusive);

                auto* res = overlap_find_i(ptr->left_, ival, exclusive);
                if (res == nullptr)
                    return overlap_find_i(ptr->right_, ival, exclusive);
                else
                    return res;
            }
            if (ptr->right_ && ptr->right_->max() >= ival.low())
            {
                if (!ptr->left_)
                    return overlap_find_i(ptr->right_, ival, exclusive);

                if (ival.high() > ptr->left_->max())
                    return overlap_find_i(ptr->right_, ival, exclusive);

                auto* res = overlap_find_i(ptr->right_, ival, exclusive);
                if (res == nullptr)
                    return overlap_find_i(ptr->left_, ival, exclusive);
                else
                    return res;
            }
            else
                return nullptr;
        }

        node_type* successor(node_type* node)
        {
            if (node->right_)
                return minimum(node->right_);
            auto* y = node->parent_;
            while (y && node == y->right_)
            {
                node = y;
                y = y->parent_;
            }
            return y;
        }

        bool is_descendant(iterator par, iterator desc)
        {
            auto p = desc->parent_;
            for (; p && p != par.node_; p = p->parent_) {}
            return p != nullptr;
        }

        /**
         *  Set v inplace of u. Does not delete u.
         *  Creates orphaned nodes. A transplant call must be succeeded by delete calls.
         */
        void transplant(node_type* u, node_type* v)
        {
            if (u->is_root())
                root_ = v;
            else if (u->is_left())
                u->parent_->left_ = v;
            else
                u->parent_->right_ = v;
            if (v)
                v->parent_ = u->parent_;
        }

        /**
         *  Get leftest of x.
         */
        node_type* minimum(node_type* x) const
        {
            while (x->left_)
                x = x->left_;
            return x;
        }

        void left_rotate(node_type* x)
        {
            auto* y = x->right_;
            x->right_ = y->left_;
            if (y->left_)
                y->left_->parent_ = x;

            y->parent_ = x->parent_;
            if (!x->parent_)
                root_ = y;
            else if (x->is_left())
                x->parent_->left_ = y;
            else
                x->parent_->right_ = y;

            y->left_ = x;
            x->parent_ = y;

            // max fixup
            if (x->left_ && x->right_)
                x->max_ = std::max(x->max_, std::max(x->left_->max_, x->right_->max_));
            else if (x->left_)
                x->max_ = std::max(x->max_, x->left_->max_);
            else if (x->right_)
                x->max_ = std::max(x->max_, x->right_->max_);
            else
                x->max_ = x->interval_.high_;

            if (y->right_)
                y->max_ = std::max(y->max_, std::max(y->right_->max_, x->max_));
            else
                y->max_ = std::max(y->max_, x->max_);
        }

        void right_rotate(node_type* y)
        {
            auto* x = y->left_;
            y->left_ = x->right_;

            if (x->right_)
                x->right_->parent_ = y;

            x->parent_= y->parent_;
            if (!y->parent_)
                root_ = x;
            else if (y->is_left())
                y->parent_->left_ = x;
            else
                y->parent_->right_ = x;

            x->right_ = y;
            y->parent_ = x;

            // max fixup
            if (y->left_ && y->right_)
                y->max_ = std::max(y->max_, std::max(y->left_->max_, y->right_->max_));
            else if (y->left_)
                y->max_ = std::max(y->max_, y->left_->max_);
            else if (y->right_)
                y->max_ = std::max(y->max_, y->right_->max_);
            else
                y->max_ = y->interval_.high_;

            if (x->left_)
                x->max_ = std::max(x->max_, std::max(x->left_->max_, y->max_));
            else
                x->max_ = std::max(x->max_, y->max_);
        }

        void recalculate_max(node_type* reacalculation_root)
        {
            auto* p = reacalculation_root;
            while (p && p->max_ <= reacalculation_root->max_)
            {
                if (p->left_ && p->left_->max_ > p->max_)
                    p->max_ = p->left_->max_;
                if (p->right_ && p->right_->max_ > p->max_)
                    p->max_ = p->right_->max_;
                p = p->parent_;
            }
        }

        void insert_fixup(node_type* z)
        {
            while (z->parent_ && z->parent_->color_ == rb_color::red)
            {
                if (!z->parent_->parent_)
                    break;
                if (z->parent_ == z->parent_->parent_->left_)
                {
                    node_type* y = z->parent_->parent_->right_;
                    if (y && y->color_ == rb_color::red)
                    {
                        z->parent_->color_ = rb_color::black;
                        y->color_ = rb_color::black;
                        z->parent_->parent_->color_ = rb_color::red;
                        z = z->parent_->parent_;
                    }
                    else
                    {
                        if (z == z->parent_->right_)
                        {
                            z = z->parent_;
                            left_rotate(z);
                        }
                        z->parent_->color_ = rb_color::black;
                        z->parent_->parent_->color_ = rb_color::red;
                        right_rotate(z->parent_->parent_);
                    }
                }
                else
                {
                    node_type* y = z->parent_->parent_->left_;
                    if (y && y->color_ == rb_color::red)
                    {
                        z->parent_->color_ = rb_color::black;
                        y->color_ = rb_color::black;
                        z->parent_->parent_->color_ = rb_color::red;
                        z = z->parent_->parent_;
                    }
                    else
                    {
                        if (z->is_left())
                        {
                            z = z->parent_;
                            right_rotate(z);
                        }
                        z->parent_->color_ = rb_color::black;
                        z->parent_->parent_->color_ = rb_color::red;
                        left_rotate(z->parent_->parent_);
                    }
                }
            }
            root_->color_ = rb_color::black;
        }

        void erase_fixup(node_type* x, node_type* x_parent, bool y_is_left)
        {
            while (x != root_ && x->color_ == rb_color::black)
            {
                node_type* w;
                if (y_is_left)
                {
                    w = x_parent->right_;
                    if (w->color_ == rb_color::red)
                    {
                        w->color_ = rb_color::black;
                        x_parent->color_ = rb_color::red;
                        left_rotate(x_parent);
                        w = x_parent->right_;
                    }

                    if (w->left_->color_ == rb_color::black && w->right_->color_ == rb_color::black)
                    {
                        w->color_ = rb_color::red;
                        x = x_parent;
                        x_parent = x->parent_;
                        y_is_left = (x == x_parent->left_);
                    }
                    else
                    {
                        if (w->right_->color_ == rb_color::black)
                        {
                            w->left_->color_ = rb_color::black;
                            w->color_ = rb_color::red;
                            right_rotate(w);
                            w = x_parent->right_;
                        }

                        w->color_ = x_parent->color_;
                        x_parent->color_ = rb_color::black;
                        if (w->right_)
                            w->right_->color_ = rb_color::black;

                        left_rotate(x_parent);
                        x = root_;
                        x_parent = nullptr;
                    }
                }
                else
                {
                    w = x_parent->left_;
                    if (w->color_ == rb_color::red)
                    {
                        w->color_ = rb_color::black;
                        x_parent->color_ = rb_color::red;
                        right_rotate(x_parent);
                        w = x_parent->left_;
                    }

                    if (w->right_->color_ == rb_color::black && w->left_->color_ == rb_color::black)
                    {
                        w->color_ = rb_color::red;
                        x = x_parent;
                        x_parent = x->parent_;
                        y_is_left = (x == x_parent->left_);
                    }
                    else
                    {
                        if (w->left_->color_ == rb_color::black)
                        {
                            w->right_->color_ = rb_color::black;
                            w->color_ = rb_color::red;
                            left_rotate(w);
                            w = x_parent->left_;
                        }

                        w->color_ = x_parent->color_;
                        x_parent->color_ = rb_color::black;
                        if (w->left_)
                            w->left_->color_ = rb_color::black;

                        right_rotate(x_parent);
                        x = root_;
                        x_parent = nullptr;
                    }
                }
            }

            x->color_ = rb_color::black;
        }

#ifdef PUBLICIZE
    public:
#else
    private:
#endif // PUBLICIZE
        node_type* root_;
    };
//############################################################################################################
}
