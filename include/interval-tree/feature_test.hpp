#pragma once

// define LIB_INTERVAL_TREE_CONCEPTS to override the concepts feature test
#ifndef LIB_INTERVAL_TREE_CONCEPTS
#    if defined(__cpp_concepts) && __cpp_concepts >= 202002L
#        define LIB_INTERVAL_TREE_CONCEPTS
#    endif
#endif

// define LIB_INTERVAL_TREE_DEPRECATED if __attribute__((deprecated)) is not supported
#ifndef LIB_INTERVAL_TREE_DEPRECATED
#    if __has_cpp_attribute(deprecated)
#        define LIB_INTERVAL_TREE_DEPRECATED [[deprecated]]
#    else
#        define LIB_INTERVAL_TREE_DEPRECATED __attribute__((deprecated))
#    endif
#endif

#ifndef LIB_INTERVAL_TREE_FALLTHROUGH
#    if __has_cpp_attribute(fallthrough)
#        define LIB_INTERVAL_TREE_FALLTHROUGH [[fallthrough]]
#    else
#        define LIB_INTERVAL_TREE_FALLTHROUGH __attribute__((fallthrough))
#    endif
#endif

// if constexpr is supported, use it, otherwise use plain if and pray the compiler optimizes it.
#if __cplusplus >= 201703L
#    define INTERVAL_TREE_CONSTEXPR_IF if constexpr
#else
#    define INTERVAL_TREE_CONSTEXPR_IF if
#endif

// enum { value = value_ } or static constexpr
#if __cplusplus >= 201703L
#    define INTERVAL_TREE_META_VALUE(type, name, the_value) static constexpr type name = the_value
#else
#    define INTERVAL_TREE_META_VALUE(type, name, the_value) \
        enum : type \
        { \
            name = the_value \
        }
#endif