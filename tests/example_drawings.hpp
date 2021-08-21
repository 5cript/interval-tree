#pragma once
#ifdef INTERVAL_TREE_DO_DRAWINGS

#include "../draw.hpp"

#include <string>

void drawDocExample()
{
    using namespace lib_interval_tree;

    interval_tree_t<int> tree;

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

    drawTree("drawings/doc_example.png", tree, false, false);
}

void drawFromTests1()
{
    using namespace lib_interval_tree;
    using namespace std::string_literals;

    interval_tree_t<int> tree;

    std::vector <interval_tree_t<int>::interval_type> intervalCollection;

    intervalCollection.push_back({-51, 11});
    intervalCollection.push_back({26, 68});
    intervalCollection.push_back({11, 100});
    intervalCollection.push_back({-97, 65});
    intervalCollection.push_back({-85, 18});
    intervalCollection.push_back({-31, -20});
    intervalCollection.push_back({-91, -6});
    intervalCollection.push_back({-17, 71});
    intervalCollection.push_back({-58, 37});
    intervalCollection.push_back({-50, -1});
    intervalCollection.push_back({11, 61});
    intervalCollection.push_back({6, 74});
    intervalCollection.push_back({13, 78});
    intervalCollection.push_back({-83, -62});
    intervalCollection.push_back({-80, 93});
    intervalCollection.push_back({-2, 84});
    intervalCollection.push_back({-62, -18});
    intervalCollection.push_back({-96, -53});
    intervalCollection.push_back({56, 91});
    intervalCollection.push_back({37, 79});

    int counter{0};
    for (auto const& i : intervalCollection)
    {
        tree.insert(i);
        drawTree("drawings/from_tests_1_"s + std::to_string(counter) + ".png", tree, false, false);
        ++counter;
    }

    tree.deoverlap();

    drawTree("drawings/from_tests_1_deoverlapped.png", tree, false, false);
}

void drawAll()
{
    drawDocExample();
    drawFromTests1();
}

#endif // INTERVAL_TREE_DO_DRAWINGS
