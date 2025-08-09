#pragma once

#ifndef INTERVAL_TREE_USE_OPTIONAL_POLYFILL
#    if __cplusplus >= 201703L
#        include <optional>
namespace lib_interval_tree
{
    using nullopt_t = std::nullopt_t;
    template <typename T>
    using optional = std::optional<T>;
    constexpr auto nullopt = std::nullopt;
}
#    elif defined(INTERVAL_TREE_HAVE_BOOST_OPTIONAL)
#        include <boost/optional.hpp>
template <typename T>
namespace lib_interval_tree
{
    template <typename T>
    using optional = boost::optional<T>;
    constexpr auto nullopt = boost::none;
    using nullopt_t = decltype(boost::none);
}
#    else
#        define INTERVAL_TREE_USE_OPTIONAL_POLYFILL
#    endif
#endif

/**
 * Note that the interval tree optional is not a complete replacement for std::optional or boost::optional.
 * It is missing some features, such as comparison operators. It does not follow the standard.
 */
#ifdef INTERVAL_TREE_USE_OPTIONAL_POLYFILL
#    include <stdexcept>
#    include <type_traits>
#    include <memory>
namespace lib_interval_tree
{
    class bad_optional_access : public std::logic_error
    {
      public:
        bad_optional_access()
            : logic_error("bad optional access")
        {}
        virtual ~bad_optional_access() noexcept = default;
    };

    struct nullopt_t
    {
        nullopt_t() = delete;
        enum class construct
        {
            token
        };
        explicit constexpr nullopt_t(construct) noexcept
        {}
    };
    constexpr nullopt_t nullopt{nullopt_t::construct::token};

    struct in_place_t
    {
        in_place_t() = delete;
        enum class construct
        {
            token
        };
        explicit constexpr in_place_t(construct) noexcept
        {}
    };
    constexpr in_place_t in_place{in_place_t::construct::token};

#    define INTERVAL_TREE_OPTIONAL_INTESTINES \
\
      public: \
        using stored_type = typename std::remove_const<T>::type; \
\
      public: \
        constexpr optional_base(nullopt_t) noexcept \
            : optional_base{} \
        {} \
\
        template <typename... Args> \
        constexpr explicit optional_base(in_place_t, Args&&... args) \
            : value_(std::forward<Args>(args)...) \
            , engaged_{true} \
        {} \
\
        template < \
            typename U, \
            typename... Args, \
            std::enable_if_t<std::is_constructible<T, std::initializer_list<U>&, Args&&...>::value, int> = 0> \
        constexpr explicit optional_base(in_place_t, std::initializer_list<U> il, Args&&... args) \
            : value_(il, std::forward<Args>(args)...) \
            , engaged_{true} \
        {} \
\
        constexpr optional_base() noexcept \
            : empty_value_{} \
        {} \
\
        optional_base(optional_base const& other) \
        { \
            if (other.engaged_) \
                construct(other.value_); \
        } \
\
        optional_base(optional_base&& other) noexcept(std::is_nothrow_move_constructible<T>::value) \
        { \
            if (other.engaged_) \
                construct(std::move(other.value_)); \
        } \
\
        optional_base& operator=(optional_base const& other) \
        { \
            if (engaged_ && other.engaged_) \
                value_ = other.value_; \
            else \
            { \
                if (other.engaged_) \
                    construct(other.value_); \
                else \
                    reset(); \
            } \
\
            return *this; \
        } \
\
        optional_base& operator=(optional_base&& other) noexcept( \
            std::is_nothrow_move_constructible<T>::value && std::is_nothrow_move_assignable<T>::value \
        ) \
        { \
            if (engaged_ && other.engaged_) \
                value_ = std::move(other.value_); \
            else \
            { \
                if (other.engaged_) \
                    construct(std::move(other.value_)); \
                else \
                    reset(); \
            } \
            return *this; \
        } \
\
        void reset() \
        { \
            if (engaged_) \
            { \
                engaged_ = false; \
                value_.~stored_type(); \
            } \
        } \
\
      protected: \
        template <typename... Args> \
        void construct(Args&&... args) \
        { \
            ::new (std::addressof(value_)) stored_type(std::forward<Args>(args)...); \
            engaged_ = true; \
        } \
\
        struct empty_byte \
        {}; \
        union \
        { \
            empty_byte empty_value_; \
            stored_type value_; \
        }; \
        bool engaged_ = false;

    template <typename T, bool should_provide_destructor = !std::is_trivially_destructible<T>::value>
    class optional_base
    {
        INTERVAL_TREE_OPTIONAL_INTESTINES

      public:
        ~optional_base()
        {
            if (engaged_)
                value_.~stored_type();
        }
    };

    template <typename T>
    class optional_base<T, false>
    {
        INTERVAL_TREE_OPTIONAL_INTESTINES
    };

    template <typename T>
    class optional : private optional_base<T>
    {
      public:
        using value_type = T;

        using optional_base<T>::optional_base;

        constexpr optional() = default;

        template <
            typename U = T,
            std::enable_if_t<
                !std::is_same<optional<T>, std::decay_t<U>>::value && std::is_constructible<T, U&&>::value &&
                    std::is_convertible<U&&, T>::value,
                bool> = true>
        constexpr optional(U&& value)
            : optional_base<T>{in_place, std::forward<U>(value)}
        {}

        template <
            typename U = T,
            std::enable_if_t<
                !std::is_same<optional<T>, std::decay_t<U>>::value && std::is_constructible<T, U&&>::value &&
                    !std::is_convertible<U&&, T>::value,
                bool> = false>
        constexpr optional(U&& value)
            : optional_base<T>{in_place, std::forward<U>(value)}
        {}

        template <
            typename U,
            std::enable_if_t<
                !std::is_same<T, U>::value && std::is_constructible<T, const U&>::value &&
                    std::is_convertible<const U&, T>::value,
                bool> = true>
        constexpr optional(const optional<U>& other)
        {
            if (other)
                emplace(*other);
        }

        template <
            typename U,
            std::enable_if_t<
                !std::is_same<T, U>::value && std::is_constructible<T, const U&>::value &&
                    !std::is_convertible<const U&, T>::value,
                bool> = false>
        explicit constexpr optional(const optional<U>& other)
        {
            if (other)
                emplace(*other);
        }

        template <
            typename U,
            std::enable_if_t<
                !std::is_same<T, U>::value && std::is_constructible<T, U&&>::value &&
                    std::is_convertible<U&&, T>::value,
                bool> = true>
        constexpr optional(optional<U>&& other)
        {
            if (other)
                emplace(std::move(*other));
        }

        template <
            typename U,
            std::enable_if_t<
                !std::is_same<T, U>::value && std::is_constructible<T, U&&>::value &&
                    !std::is_convertible<U&&, T>::value,
                bool> = false>
        explicit constexpr optional(optional<U>&& other)
        {
            if (other)
                emplace(std::move(*other));
        }

        optional& operator=(nullopt_t) noexcept
        {
            this->reset();
            return *this;
        }

        template <
            typename U = T,
            std::enable_if_t<
                !std::is_same<optional<T>, std::decay_t<U>>::value && std::is_constructible<T, U&&>::value &&
                    !(std::is_scalar<T>::value && std::is_same<T, std::decay_t<U>>::value) &&
                    std::is_assignable<T&, U&&>::value,
                int> = 0>
        optional& operator=(U&& value)
        {
            if (this->engaged_)
                this->value_ = std::forward<U>(value);
            else
                this->construct(std::forward<U>(value));
            return *this;
        }

        template <typename U>
        std::enable_if_t<
            !std::is_same<T, U>::value && std::is_constructible<T, const U&>::value &&
                std::is_assignable<T&, const U&>::value,
            optional&>
        operator=(const optional<U>& other)
        {
            if (other)
            {
                if (this->engaged_)
                    this->value_ = *other;
                else
                    this->construct(*other);
            }
            else
            {
                this->reset();
            }
            return *this;
        }

        template <typename U>
        std::enable_if_t<
            !std::is_same<T, U>::value && std::is_constructible<T, U&&>::value && std::is_assignable<T&, U&&>::value,
            optional&>
        operator=(optional<U>&& other)
        {
            if (other)
            {
                if (this->engaged_)
                    this->value_ = std::move(*other);
                else
                    this->construct(std::move(*other));
            }
            else
            {
                this->reset();
            }
            return *this;
        }

        template <typename... Args>
        std::enable_if_t<std::is_constructible<T, Args&&...>::value, void> emplace(Args&&... args)
        {
            this->reset();
            this->construct(std::forward<Args>(args)...);
        }

        template <typename U, typename... Args>
        std::enable_if_t<std::is_constructible<T, std::initializer_list<U>&, Args&&...>::value, void>
        emplace(std::initializer_list<U> il, Args&&... args)
        {
            this->reset();
            this->construct(il, std::forward<Args>(args)...);
        }

        void swap(optional& other) noexcept(
            std::is_nothrow_move_constructible<T>::value && std::is_nothrow_swappable<T>::value
        )
        {
            using std::swap;

            if (this->engaged_ && other.engaged_)
            {
                swap(this->value_, other.value_);
            }
            else if (this->engaged_)
            {
                other.construct(std::move(this->value_));
                this->reset();
            }
            else if (other.engaged_)
            {
                this->construct(std::move(other.value_));
                other.reset();
            }
        }

        constexpr const T& value() const&
        {
            if (this->engaged_)
                return this->value_;
            throw lib_interval_tree::bad_optional_access();
        }

        constexpr T& value() &
        {
            if (this->engaged_)
                return this->value_;
            throw lib_interval_tree::bad_optional_access();
        }

        constexpr T&& value() &&
        {
            if (this->engaged_)
                return std::move(this->value_);
            throw lib_interval_tree::bad_optional_access();
        }

        constexpr const T&& value() const&&
        {
            if (this->engaged_)
                return std::move(this->value_);
            throw lib_interval_tree::bad_optional_access();
        }

        constexpr const T* operator->() const
        {
            return std::addressof(this->value_);
        }

        T* operator->()
        {
            return std::addressof(this->value_);
        }

        constexpr const T& operator*() const&
        {
            return this->value_;
        }

        constexpr T& operator*() &
        {
            return this->value_;
        }

        constexpr T&& operator*() &&
        {
            return std::move(this->value_);
        }

        constexpr const T&& operator*() const&&
        {
            return std::move(this->value_);
        }

        constexpr explicit operator bool() const noexcept
        {
            return this->engaged_;
        }
    };
}
#endif
