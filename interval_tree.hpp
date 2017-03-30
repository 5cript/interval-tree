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
        friend void insert_node(std::unique_ptr <node_type>* parent, std::unique_ptr <node_type>& root, interval_type__ ival)
        {
            if (!root)
            {
                root = std::make_unique <node>(parent, ival);
                return;
            }

            auto root_begin = root->interval().low();
            if (ival.low() < root_begin)
                insert_node(&root, root->left_, ival);
            else
                insert_node(&root, root->right_, ival);

            if (root->max_ < ival.high())
                root->max_ = ival.high();
        }

        template <typename numerical_type_, typename interval_type__>
        friend std::unique_ptr <node <numerical_type_, interval_type__> >& assign(
            std::unique_ptr <node <numerical_type_, interval_type__> >& lhs,
            std::unique_ptr <node <numerical_type_, interval_type__> >& rhs
        ) noexcept
        {
            if (!lhs || &lhs == &rhs || lhs.get() == rhs.get())
                return lhs;

            if (!rhs)
            {
                lhs.reset();
                return lhs;
            }

            bool wasLeft = false;
            bool wasRight = false;

            // avoid destruction of left & right
            auto* left = lhs->left_.release();
            if (rhs.get() == nullptr)
                wasLeft = true;

            auto* right = lhs->right_.release();
            if (rhs.get() == nullptr && !wasLeft)
                wasRight = true;

            // release right and set left.
            if (!wasLeft && !wasRight)
                lhs.reset(rhs.release());
            if (wasLeft)
                lhs.reset(left);
            if (wasRight)
                lhs.reset(right);

            // reinstate left and right
            if (lhs)
            {
                if (!wasLeft)
                    lhs->left_.reset(left);
                if (!wasRight)
                    lhs->right_.reset(right);
            }
            return lhs;
        }

    public:
        node(std::unique_ptr <node>* parent, interval_type interval)
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
        std::unique_ptr <node>* parent_;
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
            if (!node_ || !*node_)
            {
                node_ = &owner_->root_;

                if (!node_ || !*node_)
                    return *this;

                while((*node_)->left_)
                    node_ = &(*node_)->left_;
            }

            if ((*node_)->right_)
            {
                node_ = &(*node_)->right_;

                while ((*node_)->left_)
                    node_ = &(*node_)->left_;
            }
            else
            {
                auto* parent = (*node_)->parent_;
                while (parent != nullptr && node_ == &(*parent)->right_)
                {
                    node_ = parent;
                    parent = (*parent)->parent_;
                }
                node_ = parent;
            }
            if (node_ && !*node_)
                node_ = nullptr;

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
            if (node_ && *node_)
                return (*node_)->interval();
            else
                throw std::out_of_range("interval_tree_iterator out of bounds");
        }

        value_type* operator->()
        {
            if (!node_)
                return nullptr;
            return (*node_).get();
        }

    private:
        interval_tree_iterator(std::unique_ptr <node_type>* node, tree_type* owner)
            : node_{node}
            , owner_{owner}
        {
        }

    private:
        std::unique_ptr <node_type>* node_;
        tree_type* owner_;
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
            insert_node(static_cast <std::unique_ptr <node_type>*> (nullptr), root_, ival);
        }

        iterator erase(iterator iter)
        {
            if (!iter.node_)
                throw std::out_of_range("cannot erase end iterator");

            std::unique_ptr <node_type>* x = nullptr;
            std::unique_ptr <node_type>* y = iter.node_;

            if (!iter->left_ && !iter->right_)
            {
                auto* parent = (*iter.node_)->parent_;
                if (parent)
                {
                    if (&(*parent)->left_ == iter.node_)
                        (*parent)->left_.reset();
                    else
                        (*parent)->right_.reset();
                }
                else
                    iter.node_->reset();
            }
            else if (iter->right_)
            {
                //x = &iter->right_;
                transplant(iter.node_, &iter->right_);
            }
            else if (iter->left_)
            {
                //x = &iter->left_;
                transplant(iter.node_, &iter->left_);
            }
            else
            {
                y = minimum(&iter->right_);
                //x = &(*y)->right_;
                /*
                if ((*y)->parent_ == iter.node_)
                    (*x)->parent_ = y;
                else
                {
                    transplant(y, &(*y)->right_);
                    (*y)->right_.reset(iter->right_.release());
                    (*y)->right_->parent_ = y;
                }
                transplant(iter.node_, y);
                (*y)->left_.reset(iter->left_.release());
                (*y)->left_->parent_ = y;
                */
                if ((*y)->parent_ != iter.node_)
                {
                    transplant(y, &(*y)->right_);
                    assign((*y)->right_, iter->right_);
                    (*y)->right_->parent_ = y;
                }
                transplant(iter.node_, y);
                assign((*y)->left_, iter->left_);
                (*y)->left_->parent_ = y;
                // (*y)->color = iter->color_;
            }
            //iter.node_->release();

            // if (y orig color == black)
            // RB-DELETE-FIXUP(x)

            return begin();

            /*
            auto former_node_max = iter.node_->max_;

            auto* parent = iter.node_->parent_;

            if (parent->right_ && iter.node_ == parent->right_.get())
                parent->right_.reset();
            if (parent->left_ && iter.node_ == parent->left_.get())
                parent->left_.reset();

            if (parent->max_ == former_node_max)
                recalculate_max(parent);

            return {parent, this};
            */
        }

        /**
         *  Finds the first interval that overlaps with ival.
         *
         *  @param ival The interval to find an overlap for within the tree.
         */
        iterator overlap_find(interval_type const& ival)
        {
            auto* ptr = &root_;
            while (*ptr && !ival.overlaps((*ptr)->interval()))
            {
                if ((*ptr)->left_ && (*ptr)->left_->max() >= ival.low())
                    ptr = &(*ptr)->left_;
                else
                    ptr = &(*ptr)->right_;
            }
            return iterator{ptr, this};
        }

        /**
         *  Merges all overlapping intervals by erasing overlapping intervals and reinserting the merged interval.
         */
        void deoverlap()
        {
            interval_tree temp;

            for (auto ival = begin(); ival != end();)
            {
                auto fres = overlap_find_i(ival);
                if (fres != end())
                {
                    auto merged = fres->interval().join(*ival);
                    ival = erase(fres);
                    temp.insert(merged);
                }
                else
                    ++ival;
            }

            *this = std::move(temp);
        }

        iterator begin()
        {
            if (!root_)
                return {nullptr, this};

            auto* iter = &root_;

            while ((*iter)->left_)
                iter = &(*iter)->left_;

            return{iter, this};
        }
        iterator end()
        {
            return {nullptr, this};
        }
    private:
        iterator overlap_find_i(iterator node)
        {
            auto* ptr = &root_;
            while (ptr->get() && (!node->interval().overlaps((*ptr)->interval()) || ptr->get() == node.operator->()))
            {
                if ((*ptr)->left_ && (*ptr)->left_->max() >= node->interval().low())
                    ptr = &(*ptr)->left_;
                else
                    ptr = &(*ptr)->right_;
            }
            if (!ptr->get())
                return {nullptr, this};
            return {ptr, this};
        }

        bool is_descendant(iterator par, iterator desc)
        {
            auto p = desc->parent_;
            for (; p && p != par.node_; p = (*p)->parent_) {}
            return p != nullptr;
        }

        void left_rotate(std::unique_ptr <node_type>* x)
        {
            /*
            auto* y = &(*x)->right_;
            std::swap((*x)->right_, (*y)->left_);
            if ((*y)->left_)
                (*y)->left_->parent_ = x->node_;
            (*y)->parent_ = (*x)->parent_;
            if ((*x)->parent_ == nullptr)
                std::swap(root_, *y);
            else if (x->node_ == (*x)->parent_->left_.get())
                std::swap((*x)->parent_->left_, *y);
            else
                std::swap((*x)->parent_->right, *y);
            std::swap((*y)->left_, *x);
            x->parent_ = y->get();
            */
        }

        void transplant(std::unique_ptr <node_type>* u, std::unique_ptr <node_type>* v)
        {
            auto* up = (*u)->parent_;
            if (up == nullptr) // u is root
            {
                assign(root_, *v);
                root_->parent_ = nullptr;
            }
            else if (u == &(*up)->left_) // u is left branch
            {
                //(*up)->left_.reset(released);
                assign((*up)->left_, *v);
                (*up)->left_->parent_ = up;
            }
            else // u is right branch
            {
                assign((*up)->right_, *v);
                (*up)->right_->parent_ = up;
            }
        }

        std::unique_ptr <node_type>* minimum(std::unique_ptr <node_type>* x) const
        {
            while ((*x)->left_)
                x = &(*x)->left_;
            return x;
        }

        iterator minimum(node_type* x) const
        {
            return minimum({x, this});
        }

        void recalculate_max(std::unique_ptr <node_type>* reacalculation_root)
        {
            auto* p = reacalculation_root;
            while ((*p) && (*p)->max_ <= (*reacalculation_root)->max_)
            {
                if ((*p)->left_ && (*p)->left_->max_ < (*p)->max_)
                    (*p)->max_ = (*p)->left_->max_;
                else if (p->right_ && p->right_->max_ < (*p)->max_)
                    (*p)->max_ = (*p)->right_->max_;
                p = (*p)->parent_;
            }
        }

    private:
        std::unique_ptr <node_type> root_;
    };
//############################################################################################################
}
