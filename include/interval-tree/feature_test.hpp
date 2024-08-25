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