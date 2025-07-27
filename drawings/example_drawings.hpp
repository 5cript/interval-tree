#pragma once

#include <interval-tree/draw.hpp>

#include <random>
#include <string>
#include <iostream>

static void drawDocExample()
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

static void drawFromTests1()
{
    using namespace lib_interval_tree;
    using namespace std::string_literals;

    interval_tree_t<int> tree;

    std::vector<interval_tree_t<int>::interval_type> intervalCollection;

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

static void drawLargeOverlapFree()
{
    using namespace lib_interval_tree;

    interval_tree_t<int> tree;

    for (int i = 0; i < 30; ++i)
    {
        tree.insert({i * 10, i * 10 + 5});
    }

    drawTree("drawings/large_overlap_free.png", tree, false, false);

    std::vector<std::pair<int, int>> intervals;
    for (int i = 0; i < 30; ++i)
    {
        intervals.emplace_back(i * 10, i * 10 + 5);
    }

    // insert shuffled into new tree
    interval_tree_t<int> tree2;
    std::mt19937 rng(std::random_device{}());
    std::shuffle(intervals.begin(), intervals.end(), rng);
    for (const auto& interval : intervals)
    {
        tree2.insert({interval.first, interval.second});
    }
    drawTree("drawings/large_overlap_free_shuffled.png", tree2, false, false);
}

static void drawOpenPunchExample()
{
    constexpr int iterations = 5;
    using namespace lib_interval_tree;

    interval_tree<interval<int, open>> tree;

    // insert shuffled into new tree
    std::vector<std::pair<int, int>> intervals;
    for (int i = 0; i < iterations; ++i)
    {
        intervals.emplace_back(i * 10, i * 10 + 5);
    }

    std::mt19937 rng(std::random_device{}());
    std::shuffle(intervals.begin(), intervals.end(), rng);
    for (const auto& interval : intervals)
    {
        tree.insert({interval.first, interval.second});
    }

    drawTree("drawings/open_punch_source.png", tree, false, false);
    const auto punched = tree.punch({-10, iterations * 10 + 10});
    drawTree("drawings/open_punched.png", punched, false, false);
}

static void drawClosedPunchExample()
{
    constexpr int iterations = 5;
    using namespace lib_interval_tree;

    interval_tree<interval<int, closed>> tree;

    // insert shuffled into new tree
    std::vector<std::pair<int, int>> intervals;
    for (int i = 0; i < iterations; ++i)
    {
        intervals.emplace_back(i * 10, i * 10 + 5);
    }

    std::mt19937 rng(std::random_device{}());
    std::shuffle(intervals.begin(), intervals.end(), rng);
    for (const auto& interval : intervals)
    {
        tree.insert({interval.first, interval.second});
    }

    drawTree("drawings/closed_punch_source.png", tree, false, false);
    const auto punched = tree.punch({-10, iterations * 10 + 10});
    drawTree("drawings/closed_punched.png", punched, false, false);
}

static void drawAdjacentClosedPunchExample()
{
    constexpr int iterations = 5;
    using namespace lib_interval_tree;

    interval_tree<interval<int, closed_adjacent>> tree;

    // insert shuffled into new tree
    std::vector<std::pair<int, int>> intervals;
    for (int i = 0; i < iterations; ++i)
    {
        intervals.emplace_back(i * 10, i * 10 + 5);
    }

    std::mt19937 rng(std::random_device{}());
    std::shuffle(intervals.begin(), intervals.end(), rng);
    for (const auto& interval : intervals)
    {
        tree.insert({interval.first, interval.second});
    }

    drawTree("drawings/closed_adjacent_punch_source.png", tree, false, false);
    const auto punched = tree.punch({-10, iterations * 10 + 10});
    drawTree("drawings/closed_adjacent_punched.png", punched, false, false);
}

static void drawFloatPunchExample()
{
    constexpr int iterations = 5;
    using namespace lib_interval_tree;

    interval_tree<interval<float, closed>> tree;

    // insert shuffled into new tree
    std::vector<std::pair<float, float>> intervals;
    for (int i = 0; i < iterations; ++i)
    {
        intervals.emplace_back(i * 10.0f, i * 10.0f + 5.0f);
    }

    std::mt19937 rng(std::random_device{}());
    std::shuffle(intervals.begin(), intervals.end(), rng);
    for (const auto& interval : intervals)
    {
        tree.insert({interval.first, interval.second});
    }

    drawTree("drawings/float_punch_source.png", tree, false, false);
    const auto punched = tree.punch({-10.0f, iterations * 10.0f + 10.0f});
    drawTree("drawings/float_punched.png", punched, false, false);
}

static void drawAll()
{
    drawDocExample();
    drawFromTests1();
    drawLargeOverlapFree();
    drawOpenPunchExample();
    drawClosedPunchExample();
    drawAdjacentClosedPunchExample();
    drawFloatPunchExample();
}
