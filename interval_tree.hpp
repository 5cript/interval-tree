#pragma once

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

    template <typename node_type, typename owner_type>
    class basic_interval_tree_iterator;

    template <typename node_type>
    class const_interval_tree_iterator;

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
        bool operator==(interval const& other) const
        {
            return low_ == other.low_ && high_ == other.high_;
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
        friend basic_interval_tree_iterator <node <numerical_type, interval_type>,
                                             interval_tree <numerical_type, typename interval_type::interval_kind> const*>;
        friend basic_interval_tree_iterator <node <numerical_type, interval_type>,
                                             interval_tree <numerical_type, typename interval_type::interval_kind>*>;

        template <typename node_type, typename interval_type__>
        friend void insert_node(node_type* parent, node_type*& root, interval_type__ ival)
        {
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

        ~node()
        {
            int A = 0;
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
        void set_interval(interval_type const& ival)
        {
            interval_ = ival;
        }

    private:
        interval_type interval_;
        value_type max_;
        node* parent_;
        node* left_;
        node* right_;
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

    protected:
        basic_interval_tree_iterator(node_ptr_t node, owner_type owner)
            : node_{node}
            , owner_{owner}
        {
        }

    protected:
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
            insert_node(static_cast <node_type*> (nullptr), root_, ival);
        }

        iterator erase(iterator iter)
        {
            if (!iter.node_)
                throw std::out_of_range("cannot erase end iterator");

            //node_type* x = nullptr;
            node_type* y = iter.node_;

            if (!iter->left_ && !iter->right_)
            {
                auto* parent = iter.node_->parent_;
                if (parent)
                {
                    if (parent->left_ == iter.node_)
                    {
                        delete parent->left_;
                        parent->left_ = nullptr;
                    }
                    else
                    {
                        delete parent->right_;
                        parent->right_ = nullptr;
                    }
                }
                else
                {
                    // is root
                    delete iter.node_;
                    root_ = nullptr;
                }
            }
            else if (iter->right_ && iter->left_)
            {
                y = minimum(iter->right_);
                if (y->parent_ != iter.node_)
                {
                    transplant(y, y->right_);
                    y->right_ = iter.node_;
                    y->right_->parent_ = y;
                }
                transplant(iter.node_, y);
                y->left_ = iter.node_->left_;
                y->left_->parent_ = y;
                //...
            }
            else if (iter->right_)
            {
                //x = &iter->right_;
                transplant(iter.node_, iter->right_);
            }
            else if (iter->left_)
            {
                //x = &iter->left_;
                transplant(iter.node_, iter->left_);
            }
            //iter.node_->release();

            // if (y orig color == black)
            // RB-DELETE-FIXUP(x)

            return begin();
        }

        /**
         *  Finds the first interval that overlaps with ival.
         *
         *  @param ival The interval to find an overlap for within the tree.
         */
        iterator overlap_find(interval_type const& ival)
        {
            auto* ptr = root_;
            while (*ptr && !ival.overlaps(ptr->interval()))
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
            interval_tree temp = *this;

            for (auto i = begin(), e = end(); i != e; ++i)
            {
                auto f = overlap_find_i(i);
                if (f != end())
                {
                    auto merged = f->interval().join(*i);
                    temp.insert(merged);
                    i->set_interval(merged);
                }
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
            while (*ptr && (!node->interval().overlaps(ptr->interval()) || ptr->get() == node.operator->()))
            {
                if (ptr->left_ && ptr->left_->max() >= node->interval().low())
                    ptr = ptr->left_;
                else
                    ptr = ptr->right_;
            }
            if (!ptr->get())
                return {nullptr, this};
            return {ptr, this};
        }

        bool is_descendant(iterator par, iterator desc)
        {
            auto p = desc->parent_;
            for (; p && p != par.node_; p = p->parent_) {}
            return p != nullptr;
        }

        void left_rotate(node_type* x)
        {
            /*
            auto* y = &(*x)->right_;
            std::swap((*x)->right_, y->left_);
            if (y->left_)
                y->left_->parent_ = x->node_;
            y->parent_ = (*x)->parent_;
            if ((*x)->parent_ == nullptr)
                std::swap(root_, *y);
            else if (x->node_ == (*x)->parent_->left_.get())
                std::swap((*x)->parent_->left_, *y);
            else
                std::swap((*x)->parent_->right, *y);
            std::swap(y->left_, *x);
            x->parent_ = y->get();
            */
        }

        // set v inplace of u.
        void transplant(node_type* u, node_type* v)
        {
            if (!u->parent_)
                root_ = v;
            else if (u == u->parent_->left_)
                u->parent_->left_ = v;
            else
                u->parent_->right_ = v;
            if (v)
                v->parent_ = u->parent_;
        }

        node_type* minimum(node_type* x) const
        {
            while (x->left_)
                x = x->left_;
            return x;
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

    private:
        node_type* root_;
    };
//############################################################################################################
}
