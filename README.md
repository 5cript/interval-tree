# interval-tree

A C++ header only interval tree implementation, which takes a red black tree as its base to inhibit degeneration to linked lists.
Methods only work (make mathematical sense) with closed intervals.

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

  interval_tree<int> tree;

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

## Members
### iterator insert(interval_type const& ival)
Adds an interval into the tree. 
#### Parameters
* `ival` An interval

**Returns**: An iterator to the inserted element.

---
### iterator erase(iterator iter)
Removes the interval given by iterator from the tree.
(does not invalidate iterators).
#### Parameters
* `iter` A valid non-end iterator

**Returns**: An iterator to the next element.

---
### iterator find(interval_type const& ival)
Finds the first interval in the interval tree that has an exact match.
**WARNING**: There is no special handling for floats.
#### Parameters
* `ival` The interval to find.

**Returns**: An iterator to the found element, or std::end(tree).

---
### iterator find(interval_type const& ival, CompareFunctionT const& compare)
Finds the first interval in the interval tree that has the following statement evaluate to true: compare(ival, interval_in_tree);
Allows for propper float comparisons.
#### Parameters
* `ival` The interval to find.
* `compare` The compare function to compare intervals with.

**Returns**: An iterator to the found element, or std::end(tree).

---
### iterator overlap_find(interval_type const& ival)
Finds the first interval in the interval tree that overlaps the given interval.
#### Parameters
* `ival` The interval to find an overlap for.

**Returns**: An iterator to the found element, or std::end(tree).

---
### interval_tree& deoverlap()
Merges all overlapping intervals within the tree. After calling deoverlap, the tree will only contain disjoint intervals.

**Returns**: *this
### After deoverlap
![AfterDeoverlap](https://user-images.githubusercontent.com/6238896/55505612-c5a96c80-5653-11e9-81f8-28a8ae35a077.png)

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

**Returns**: Is this tree empty?

---
### iterator end()
Returns a past the end iterator.

**Returns**: Is this tree empty?

---
