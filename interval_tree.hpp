#pragma once

#include "interval_tree_fwd.hpp"
#include "interval_types.hpp"

#include "draw.hpp"

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
        bool operator==(interval const& other) const
        {
            return low_ == other.low_ && high_ == other.high_;
        }

        /**
         *  Returns if both intervals are different.
         */
        bool operator!=(interval const& other) const
        {
            return low_ != other.low_ || high_ != other.high_;
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

#ifdef PUBLICIZE
    public:
#else
    private:
#endif // PUBLICIZE
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
        friend basic_interval_tree_iterator <node <numerical_type, interval_type>,
                                             interval_tree <numerical_type, typename interval_type::interval_kind> const*>;
        friend basic_interval_tree_iterator <node <numerical_type, interval_type>,
                                             interval_tree <numerical_type, typename interval_type::interval_kind>*>;

        template <typename node_type, typename interval_type__>
        friend void insert_node(node_type* parent, node_type*& root, interval_type__ ival)
        //template <typename tree_type, typename node_type>
        //friend void insert_node(tree_type* tree, node_type* z)
        {
            /*
            if (!root)
            {
                root = new node(parent, ival);
                return;
            }

            auto root_begin = root->interval().low();
            if (ival.low() < root_begin)
                insert_node(root, root->left_, ival);
            else
                insert_node(root, root->right_, ival);

            if (root->max_ < ival.high())
                root->max_ = ival.high();
            */
        }

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
        constexpr basic_interval_tree_iterator& operator=(basic_interval_tree_iterator const&) = default;

        basic_interval_tree_iterator& operator++()
        {
            if (!node_ )
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


        basic_interval_tree_iterator operator++(int)
        {
            basic_interval_tree_iterator cpy = *this;
            operator++();
            return cpy;
        }

        bool operator!=(basic_interval_tree_iterator const& other) const
        {
            return node_ != other.node_;
        }

        bool operator==(basic_interval_tree_iterator const& other) const
        {
            return node_ == other.node_;
        }

        ~basic_interval_tree_iterator() = default;

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

        friend tree_type;

    public:
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

        friend tree_type;

    public:
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
    template <typename numerical_type = default_interval_value_type, typename interval_kind = right_open>
    class interval_tree
    {
    public:
        friend basic_interval_tree_iterator <node <numerical_type, interval <numerical_type, interval_kind>>,
                                             interval_tree <numerical_type, interval_kind> const*>;
        friend basic_interval_tree_iterator <node <numerical_type, interval <numerical_type, interval_kind>>,
                                             interval_tree <numerical_type, interval_kind>*>;

    public:
        using value_type = numerical_type;
        using interval_type = interval <value_type, interval_kind>;
        using node_type = node <value_type, interval_type>;
        using iterator = interval_tree_iterator <node_type>;
        using const_iterator = const_interval_tree_iterator <node_type>;

        interval_tree()
            : root_{}
        {
        }

        ~interval_tree()
        {
            //for (auto i = std::begin(*this); i != std::end(*this);)
            //    i = erase(i);
        }

        interval_tree(interval_tree const& other)
            : root_{}
        {
            operator=(other);
        }

        interval_tree& operator=(interval_tree const& other)
        {
            for (auto i = other.begin(), e = other.end(); i != e; ++i)
                insert(i->interval());
            return *this;
        }


        /**
         *  Inserts an interval into the tree.
         */
        void insert(interval_type const& ival)
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
        }

        /**
         *  Erases the element pointed to be iter.
         */
        iterator erase(iterator iter)
        {
            if (!iter.node_)
                throw std::out_of_range("cannot erase end iterator");

            auto next = iter; next++;

            node_type* y = iter.node_;
            node_type* x = nullptr;

            auto y_orig_color = y->color_;

            if (!iter->left_ && !iter->right_)
            {
                if (!y->is_root())
                    y->kill();
                else
                {
                    delete iter.node_;
                    root_ = nullptr;
                    return {nullptr, this};
                }
            }
            else if (iter->right_ && iter->left_)
            {
                y = minimum(iter->right_);
                x = y->right_;
                y_orig_color = y->color_;
                if (y->parent_ == iter.node_)
                    x->parent_ = y;
                else
                {
                    transplant(y, y->right_);
                    y->right_ = iter.node_->right_;
                    y->right_->parent_ = y;
                }
                transplant(iter.node_, y);
                y->left_ = iter.node_->left_;
                y->left_->parent_ = y;
                y->color_ = iter.node_->color_;
                delete iter.node_;
            }
            else if (iter->right_)
            {
                x = iter->right_;
                transplant(iter.node_, iter->right_);
                delete iter.node_;
            }
            else if (iter->left_)
            {
                x = iter->left_;
                transplant(iter.node_, iter->left_);
                delete iter.node_;
            }
            if (y_orig_color == rb_color::black)
                erase_fixup(x);

            return next;
        }

        /**
         *  Finds the first interval that overlaps with ival.
         *
         *  @param ival The interval to find an overlap for within the tree.
         */
        iterator overlap_find(interval_type const& ival)
        {
            auto* ptr = root_;
            while (ptr && !ival.overlaps(ptr->interval()))
            {
                if (ptr->left_ && ptr->left_->max() >= ival.low())
                    ptr = ptr->left_;
                else
                    ptr = ptr->right_;
            }
            return iterator{ptr, this};
        }

        /**
         *  Finds the first exact match.
         *
         *  @param ival The interval to find an exact match for within the tree.
         */
        iterator find(interval_type const& ival)
        {
            auto* ptr = root_;
            while (ptr && ival != ptr->interval())
            {
                if (ptr->left_ && ptr->left_->max() >= ival.low())
                    ptr = ptr->left_;
                else
                    ptr = ptr->right_;
            }
            return iterator{ptr, this};
        }

        /**
         *  Merges all overlapping intervals by erasing overlapping intervals and reinserting the merged interval.
         */
        void deoverlap()
        {
            int counter = 0;
            for (auto i = begin(), e = end(); i != e;)
            {
                drawTree(std::to_string(counter++) + ".png", this);
                auto f = overlap_find_i(i);
                if (f != end())
                {
                    auto merged = f->interval().join(*i);
                    i->set_interval(merged);
                    erase(f);
                    i = std::begin(*this);
                }
                else
                    ++i;
            }
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

        const_iterator begin() const
        {
            if (!root_)
                return {nullptr, this};

            auto* iter = root_;

            while (iter->left_)
                iter = iter->left_;

            return const_iterator{iter, this};
        }
        const_iterator end() const
        {
            return const_iterator{nullptr, this};
        }

    private:
        iterator overlap_find_i(iterator node)
        {
            auto* ptr = root_;
            while (ptr && (!node->interval().overlaps(ptr->interval()) || ptr == node.node_))
            {
                if (ptr->left_ && ptr->left_->max() >= node->interval().low())
                    ptr = ptr->left_;
                else
                    ptr = ptr->right_;
            }
            if (!ptr)
                return {nullptr, this};
            return {ptr, this};
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
        }

        void recalculate_max(node_type* reacalculation_root)
        {
            auto* p = reacalculation_root;
            while (p && p->max_ <= reacalculation_root->max_)
            {
                if (p->left_ && p->left_->max_ < p->max_)
                    p->max_ = p->left_->max_;
                else if (p->right_ && p->right_->max_ < p->max_)
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
                    if (y->color_ == rb_color::red)
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
                    if (y->color_ == rb_color::red)
                    {
                        z->parent_->color_ = rb_color::black;
                        y->color_ = rb_color::black;
                        z->parent_->parent_->color_ = rb_color::red;
                        z = z->parent_->parent_;
                    }
                    else
                    {
                        if (z == z->parent_->left_)
                        {
                            z = z->parent_;
                            left_rotate(z);
                        }
                        z->parent_->color_ = rb_color::black;
                        z->parent_->parent_->color_ = rb_color::red;
                        right_rotate(z->parent_->parent_);
                    }
                }
            }
            root_->color_ = rb_color::black;
        }

        void erase_fixup(node_type* x)
        {
            while (x != root_ && x->color_ == rb_color::black)
            {
                if (x == x->parent_->left_)
                {
                    node_type* w = x->parent_->right_;
                    if (w->color_ == rb_color::red)
                    {
                        w->color_ = rb_color::black;
                        x->parent_->color_ = rb_color::red;
                        left_rotate(x->parent_);
                        w = x->parent_->right_;
                    }
                    if (!w->left_ || !w->right_)
                        return;
                    if (w->left_->color_ == rb_color::black && w->right_->color_ == rb_color::black)
                    {
                        w->color_ = rb_color::red;
                        x = x->parent_;
                    }
                    else
                    {
                        if (w->right_->color_ == rb_color::black)
                        {
                            w->left_->color_ = rb_color::black;
                            w->color_ = rb_color::red;
                            right_rotate(w);
                            w = x->parent_->right_;
                        }
                        w->color_ = x->parent_->color_;
                        x->parent_->color_ = rb_color::black;
                        w->right_->color_ = rb_color::black;
                        left_rotate(x->parent_);
                        x = root_;
                    }
                }
                else
                {
                    node_type* w = x->parent_->left_;
                    if (w->color_ == rb_color::red)
                    {
                        w->color_ = rb_color::black;
                        x->parent_->color_ = rb_color::red;
                        left_rotate(x->parent_);
                        w = x->parent_->left_;
                    }
                    if (!w->right_ || !w->left_)
                        return;
                    if (w->right_->color_ == rb_color::black && w->left_->color_ == rb_color::black)
                    {
                        w->color_ = rb_color::red;
                        x = x->parent_;
                    }
                    else
                    {
                        if (w->left_->color_ == rb_color::black)
                        {
                            w->right_->color_ = rb_color::black;
                            w->color_ = rb_color::red;
                            right_rotate(w);
                            w = x->parent_->left_;
                        }
                        w->color_ = x->parent_->color_;
                        x->parent_->color_ = rb_color::black;
                        w->left_->color_ = rb_color::black;
                        left_rotate(x->parent_);
                        x = root_;
                    }
                }
            }
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
