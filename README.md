# interval-tree

A C++ header only interval tree implementation, which takes a red black tree as its base to inhibit degeneration to linked lists.

## How an interval tree looks like:
![ExampleTree](https://cloud.githubusercontent.com/assets/6238896/24608762/36422d7c-1878-11e7-9c5c-a45bdcd6e187.png)

## Example
```C++
#define INTERVAL_TREE_SAFE_INTERVALS // makes sure that upper_bound > lower_bound (by swapping if neccessary), but is slower. Will become an assert if left out.
// #include "draw.hpp" // to draw tree. this is not header only anymore.
#include "interval_tree.hpp"

int main()
{
  using namespace lib_interval_tree;

  // interval_tree <interval <int>>;
  interval_tree_t <int> tree;

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

  tree.deoverlap();
  
  for (auto const& i : tree)
  {
    std::cout << "[" << i.low() << ", " << i.high() << "]\n";
  }
}
```

## Compile & Run Testing
Having googletest (find here on github) installed / built is a requirement to run the tests.
Navigate into the tests folder and build the source using the CMakeLists. You might have to adapt the linker line for gtest, if you built it yourself and didn't install it into your system.

## Free Functions
### interval<NumericT, Kind> make_safe_interval(NumericT border1, NumericT border2)
Creates an interval where the borders are sorted so the lower border is the first one.

## Members of IntervalTree<Interval>

  - [Members of IntervalTree<Interval>](#members-of-intervaltreeinterval)
    - [iterator insert(interval_type const& ival)](#iterator-insertinterval_type-const-ival)
      - [Parameters](#parameters)
    - [iterator insert_overlap(interval_type const& ival)](#iterator-insert_overlapinterval_type-const-ival)
      - [Parameters](#parameters-1)
    - [iterator erase(iterator iter)](#iterator-eraseiterator-iter)
      - [Parameters](#parameters-2)
    - [size_type size() const](#size_type-size-const)
    - [(const)iterator find(interval_type const& ival)](#constiterator-findinterval_type-const-ival)
      - [Parameters](#parameters-3)
    - [(const)iterator find(interval_type const& ival, CompareFunctionT const& compare)](#constiterator-findinterval_type-const-ival-comparefunctiont-const-compare)
      - [Parameters](#parameters-4)
    - [(const)iterator find_all(interval_type const& ival, OnFindFunctionT const& on_find)](#constiterator-find_allinterval_type-const-ival-onfindfunctiont-const-on_find)
      - [Parameters](#parameters-5)
      - [Example](#example-1)
    - [(const)iterator find_all(interval_type const& ival, OnFindFunctionT const& on_find, CompareFunctionT const& compare)](#constiterator-find_allinterval_type-const-ival-onfindfunctiont-const-on_find-comparefunctiont-const-compare)
      - [Parameters](#parameters-6)
    - [(const)iterator find_next_in_subtree(iterator from, interval_type const& ival)](#constiterator-find_next_in_subtreeiterator-from-interval_type-const-ival)
      - [Parameters](#parameters-7)
    - [(const)iterator find_next(iterator from, interval_type const& ival, CompareFunctionT const& compare)](#constiterator-find_nextiterator-from-interval_type-const-ival-comparefunctiont-const-compare)
      - [Parameters](#parameters-8)
    - [(const)iterator overlap_find(interval_type const& ival, bool exclusive)](#constiterator-overlap_findinterval_type-const-ival-bool-exclusive)
      - [Parameters](#parameters-9)
    - [(const)iterator overlap_find_all(interval_type const& ival, OnFindFunctionT const& on_find, bool exclusive)](#constiterator-overlap_find_allinterval_type-const-ival-onfindfunctiont-const-on_find-bool-exclusive)
      - [Parameters](#parameters-10)
      - [Example](#example-2)
    - [(const)iterator overlap_find_next_in_subtree(interval_type const& ival, bool exclusive)](#constiterator-overlap_find_next_in_subtreeinterval_type-const-ival-bool-exclusive)
      - [Parameters](#parameters-11)
    - [interval_tree& deoverlap()](#interval_tree-deoverlap)
    - [After deoverlap](#after-deoverlap)
    - [interval_tree& deoverlap_copy()](#interval_tree-deoverlap_copy)
    - [interval_tree punch(interval_type const& ival)](#interval_tree-punchinterval_type-const-ival)
    - [After punching (with [0, 50])](#after-punching-with-0-50)
    - [interval_tree punch()](#interval_tree-punch)
    - [bool empty() const noexcept](#bool-empty-const-noexcept)
    - [iterator begin()](#iterator-begin)
    - [iterator end()](#iterator-end)

### iterator insert(interval_type const& ival)
Adds an interval into the tree. 
#### Parameters
* `ival` An interval

**Returns**: An iterator to the inserted element.

---
### iterator insert_overlap(interval_type const& ival)
Inserts an interval into the tree if no other interval overlaps it.
Otherwise merge the interval with the one being overlapped.
#### Parameters
* `ival` An interval
* `exclusive` Exclude borders from overlap check. Defaults to false.

**Returns**: An iterator to the inserted element.

---
### iterator erase(iterator iter)
Removes the interval given by iterator from the tree.
(does not invalidate iterators).
#### Parameters
* `iter` A valid non-end iterator

**Returns**: An iterator to the next element.

---
### size_type size() const
Returns the amount of nodes in the tree.

**Returns**: The amount of tree nodes.

---
### (const)iterator find(interval_type const& ival)
Finds the first interval in the interval tree that has an exact match.
**WARNING**: There is no special handling for floats.
#### Parameters
* `ival` The interval to find.

**Returns**: An iterator to the found element, or std::end(tree).

---
### (const)iterator find(interval_type const& ival, CompareFunctionT const& compare)
Finds the first interval in the interval tree that has the following statement evaluate to true: compare(ival, interval_in_tree);
Allows for propper float comparisons.
#### Parameters
* `ival` The interval to find.
* `compare` The compare function to compare intervals with.

**Returns**: An iterator to the found element, or std::end(tree).

---
### (const)iterator find_all(interval_type const& ival, OnFindFunctionT const& on_find)
Find all intervals in the tree matching ival.
#### Parameters
* `ival` The interval to find.
* `on_find` A function of type bool(iterator) that is called when an interval was found.
Return true to continue, false to preemptively abort search.
#### Example
```c++
tree.insert({3, 7});
tree.insert({3, 7});
tree.insert({8, 9});
tree.find_all({3, 7}, [](auto iter) /* iter will be const_iterator if tree is const */ {
  // will find all intervals that are exactly {3,7} here.
});
```

**Returns**: An iterator to the found element, or std::end(tree).

---
### (const)iterator find_all(interval_type const& ival, OnFindFunctionT const& on_find, CompareFunctionT const& compare)
Find all intervals in the tree that the compare function returns true for.
#### Parameters
* `ival` The interval to find.
* `compare` The compare function to compare intervals with.
* `on_find` A function of type bool(iterator) that is called when an interval was found.
Return true to continue, false to preemptively abort search.

**Returns**: An iterator to the found element, or std::end(tree).

---
### (const)iterator find_next_in_subtree(iterator from, interval_type const& ival)
Finds the next exact match EXCLUDING from in the subtree originating from "from".
You cannot find all matches this way, use find_all for that.
#### Parameters
* `from` The iterator to start from. (including this iterator!)
* `ival` The interval to find.

**Returns**: An iterator to the found element, or std::end(tree).

---
### (const)iterator find_next(iterator from, interval_type const& ival, CompareFunctionT const& compare)
Finds the next exact match EXCLUDING from in the subtree originating from "from".
You cannot find all matches this way, use find_all for that.
#### Parameters
* `from` The iterator to start from (including this iterator!)
* `ival` The interval to find.
* `compare` The compare function to compare intervals with.

**Returns**: An iterator to the found element, or std::end(tree).

---
### (const)iterator overlap_find(interval_type const& ival, bool exclusive)
Finds the first interval in the interval tree that overlaps the given interval.
#### Parameters
* `ival` The interval to find an overlap for.
* `exclusive` Exclude borders from overlap check. Defaults to false.

**Returns**: An iterator to the found element, or std::end(tree).

---
### (const)iterator overlap_find_all(interval_type const& ival, OnFindFunctionT const& on_find, bool exclusive)
Finds the first interval in the interval tree that overlaps the given interval.
#### Parameters
* `ival` The interval to find an overlap for.
* `on_find` A function of type bool(iterator) that is called when an interval was found.
Return true to continue, false to preemptively abort search.
* `exclusive` Exclude borders from overlap check. Defaults to false.
#### Example
```c++
tree.insert({0, 5});
tree.insert({5, 10});
tree.insert({10, 15});
tree.overlap_find_all({5, 5}, [](auto iter) /* iter will be const_iterator if tree is const */ {
  // called with {0, 5} and {5, 10} in unspecified order.
});
```

**Returns**: An iterator to the found element, or std::end(tree).

---
### (const)iterator overlap_find_next_in_subtree(interval_type const& ival, bool exclusive)
Finds the next interval in the subtree originating in ival that overlaps the given interval.
You cannot find all matches this way, use overlap_find_all for that.
#### Parameters
* `ival` The interval to find an overlap for.
* `exclusive` Exclude borders from overlap check. Defaults to false.

**Returns**: An iterator to the found element, or std::end(tree).

---
### interval_tree& deoverlap()
Merges all overlapping intervals within the tree. After calling deoverlap, the tree will only contain disjoint intervals.

**Returns**: *this
### After deoverlap
![AfterDeoverlap](https://user-images.githubusercontent.com/6238896/55505612-c5a96c80-5653-11e9-81f8-28a8ae35a077.png)

### interval_tree& deoverlap_copy()
Same as deoverlap, but not inplace

---
### interval_tree punch(interval_type const& ival)
Removes all intervals from `ival` and produces a tree that contains the remaining intervals.
**The tree must be deoverlapped, or the result is undefined.**
`ival` is expected to encompass the entire interval range.

**Returns**: A new interval_tree containing the gaps.
### After punching (with [0, 50])
![AfterPunch](https://cloud.githubusercontent.com/assets/6238896/24613645/2dbf72e8-1889-11e7-813f-6d16fe0ad327.png)

---
### interval_tree punch()
Same as punch(interval_type const& ival), but with ival = [lowest_lower_bound, highest_upper_bound], resulting in only
the gaps between existing intervals.

---
### bool empty() const noexcept
Returns whether or not the tree is empty.

**Returns**: Is this tree empty?

---
### iterator begin()
Returns the iterator of the interval with the lowest lower_bound.

**Returns**: begin iterator.

---
### iterator end()
Returns a past the end iterator.

**Returns**: past the end iterator.

---

## Members of Interval

  - [Members of Interval](#members-of-interval)
    - [using value_type](#using-value_type)
    - [using interval_kind](#using-interval_kind)
    - [friend bool operator==(interval const& lhs, interval const& other)](#friend-bool-operatorinterval-const-lhs-interval-const-other)
    - [friend bool operator!=(interval const& lhs, interval const& other)](#friend-bool-operatorinterval-const-lhs-interval-const-other-1)
    - [value_type low() const](#value_type-low-const)
    - [value_type high() const](#value_type-high-const)
    - [bool overlaps(value_type l, value_type h) const](#bool-overlapsvalue_type-l-value_type-h-const)
    - [bool overlaps_exclusive(value_type l, value_type h) const](#bool-overlaps_exclusivevalue_type-l-value_type-h-const)
    - [bool overlaps(interval const& other) const](#bool-overlapsinterval-const-other-const)
    - [bool overlaps_exclusive(interval const& other) const](#bool-overlaps_exclusiveinterval-const-other-const)
    - [bool within(value_type value) const](#bool-withinvalue_type-value-const)
    - [bool within(interval const& other) const](#bool-withininterval-const-other-const)
    - [value_type operator-(interval const& other) const](#value_type-operator-interval-const-other-const)
    - [value_type size() const](#value_type-size-const)
    - [interval join(interval const& other) const](#interval-joininterval-const-other-const)

You can implement your own interval if you provide all the same functions.
### using value_type
The underlying interval numerical type
### using interval_kind
The interval kind. You dont need to provides this typedef in your interval class.
### friend bool operator==(interval const& lhs, interval const& other)
Comparison operator.
### friend bool operator!=(interval const& lhs, interval const& other)
Comparison operator.
### value_type low() const
Lower bound.
### value_type high() const
Upper bound.
### bool overlaps(value_type l, value_type h) const
Overlap these bounds with this interval (closed)?
### bool overlaps_exclusive(value_type l, value_type h) const
Overlap these bounds with this interval excluding borders?
### bool overlaps(interval const& other) const
Like overlaps with lower and upper bound.
### bool overlaps_exclusive(interval const& other) const
Like overlaps with lower and upper bound.
### bool within(value_type value) const
Is the value within the interval (closed)?
### bool within(interval const& other) const
Is the interval within the interval?
### value_type operator-(interval const& other) const
Calculates the distance between the two intervals.
Overlapping intervals have 0 distance.
### value_type size() const
Returns high - low.
### interval join(interval const& other) const
Joins 2 intervals and whatever is inbetween.