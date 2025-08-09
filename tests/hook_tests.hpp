#pragma once

struct TestHook : public lib_interval_tree::hooks::regular
{
    using hook_state = struct hook_state
    {
        std::function<void()> on_destroy;
        std::function<void(void*, void*)> on_before_insert_fixup;
        std::function<void(void*, void*)> on_after_insert_fixup;
        std::function<void(void*, void*, void*, bool)> on_before_erase_fixup;
        std::function<void(void*, void*, void*, bool)> on_after_erase_fixup;
        std::function<void(void*, void*)> on_before_recalculate_max;
        std::function<void(void*, void*)> on_after_recalculate_max;
    };

    template <typename tree_type>
    static inline void on_destroy(tree_type& tree) noexcept
    {
        if (tree.on_destroy)
            tree.on_destroy();
    }

    template <typename tree_type>
    static inline void on_before_insert_fixup(tree_type& tree, typename tree_type::node_type* node) noexcept
    {
        if (tree.on_before_insert_fixup)
            tree.on_before_insert_fixup(&tree, node);
    }

    template <typename tree_type>
    static inline void on_after_insert_fixup(tree_type& tree, typename tree_type::node_type* node) noexcept
    {
        if (tree.on_after_insert_fixup)
            tree.on_after_insert_fixup(&tree, node);
    }

    template <typename tree_type>
    static inline void on_before_erase_fixup(
        tree_type& tree,
        typename tree_type::node_type* node,
        typename tree_type::node_type* node_parent,
        bool other_is_left_child
    ) noexcept
    {
        if (tree.on_before_erase_fixup)
            tree.on_before_erase_fixup(&tree, node, node_parent, other_is_left_child);
    }

    template <typename tree_type>
    static inline void on_after_erase_fixup(
        tree_type& tree,
        typename tree_type::node_type* node,
        typename tree_type::node_type* node_parent,
        bool other_is_left_child
    ) noexcept
    {
        if (tree.on_after_erase_fixup)
            tree.on_after_erase_fixup(&tree, node, node_parent, other_is_left_child);
    }

    template <typename tree_type>
    static inline void on_before_recalculate_max(tree_type& tree, typename tree_type::node_type* node) noexcept
    {
        if (tree.on_before_recalculate_max)
            tree.on_before_recalculate_max(&tree, node);
    }

    template <typename tree_type>
    static inline void on_after_recalculate_max(tree_type& tree, typename tree_type::node_type* node) noexcept
    {
        if (tree.on_after_recalculate_max)
            tree.on_after_recalculate_max(&tree, node);
    }
};

class HookTests : public ::testing::Test
{
  public:
    using interval_type = lib_interval_tree::interval<int>;

    template <typename HookT>
    using tree_type = lib_interval_tree::interval_tree<interval_type, HookT>;
};

TEST_F(HookTests, OnDestroyIsCalled)
{
    bool called = false;
    {
        tree_type<TestHook> tree;
        tree.on_destroy = [&called]() {
            called = true;
        };
    };

    EXPECT_TRUE(called);
}

TEST_F(HookTests, OnBeforeInsertFixupIsCalled)
{
    bool called = false;
    tree_type<TestHook> tree;
    tree.on_before_insert_fixup = [&called](void*, auto) {
        called = true;
    };

    tree.insert({0, 0});

    EXPECT_TRUE(called);
}

TEST_F(HookTests, OnAfterInsertFixupIsCalled)
{
    bool called = false;
    tree_type<TestHook> tree;
    tree.on_after_insert_fixup = [&called](void*, auto) {
        called = true;
    };

    tree.insert({0, 0});

    EXPECT_TRUE(called);
}

TEST_F(HookTests, OnBeforeEraseFixupIsCalled)
{
    bool called = false;
    tree_type<TestHook> tree;
    tree.on_before_erase_fixup = [&called](void*, auto, auto, bool) {
        called = true;
    };

    tree.insert({16, 21});
    tree.insert({8, 9});
    tree.insert({25, 30});
    tree.insert({5, 8});
    tree.insert({15, 23});
    tree.insert({17, 19});
    tree.insert({26, 26});
    tree.insert({0, 3});
    tree.insert({6, 10});
    tree.insert({19, 20});

    tree.erase(tree.find({17, 19}));

    EXPECT_TRUE(called);
}

TEST_F(HookTests, OnAfterEraseFixupIsCalled)
{
    bool called = false;
    tree_type<TestHook> tree;
    tree.on_after_erase_fixup = [&called](void*, auto, auto, bool) {
        called = true;
    };

    tree.insert({16, 21});
    tree.insert({8, 9});
    tree.insert({25, 30});
    tree.insert({5, 8});
    tree.insert({15, 23});
    tree.insert({17, 19});
    tree.insert({26, 26});
    tree.insert({0, 3});
    tree.insert({6, 10});
    tree.insert({19, 20});

    tree.erase(tree.find({17, 19}));

    EXPECT_TRUE(called);
}

TEST_F(HookTests, OnBeforeRecalculateMaxIsCalled)
{
    bool called = false;
    tree_type<TestHook> tree;
    tree.on_before_recalculate_max = [&called](void*, auto) {
        called = true;
    };

    tree.insert({0, 0});

    EXPECT_TRUE(called);
}

TEST_F(HookTests, OnAfterRecalculateMaxIsCalled)
{
    bool called = false;
    tree_type<TestHook> tree;
    tree.on_after_recalculate_max = [&called](void*, auto) {
        called = true;
    };

    tree.insert({0, 0});

    EXPECT_TRUE(called);
}