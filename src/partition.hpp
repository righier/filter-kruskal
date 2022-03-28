#pragma once

#include <algorithm>

#include "utils/graph.hpp"

static inline EdgeIt partition(EdgeIt first, EdgeIt last, float pivotVal) {
  return std::partition(first, last,
                        [pivotVal](const Edge &e) { return e.w < pivotVal; });
}

// partitions the range [first, last) and filters out the pivot from the list
static inline int partitionSkipPivot(Edges &edges, int first, int &last,
                                     Edge pivot) {
  int right = last - 1;
  while (first < right) {
    if (Edge::sameNodes(edges[first], pivot)) {
      std::swap(edges[first], edges[--last]);
    } else if (edges[first] < pivot) {
      first++;
    } else {
      std::swap(edges[first], edges[--right]);
    }
  }
  return first;
}