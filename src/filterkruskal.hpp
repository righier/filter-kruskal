#pragma once

#include <algorithm>

#include "kruskal.hpp"
#include "partition.hpp"
#include "pivot.hpp"
#include "unionfind.hpp"
#include "utils/graph.hpp"

static inline bool filter(DisjointSet &set, int a, int b) {
  return set.compare(a, b);
}

static inline EdgeIt filterAll(DisjointSet &set, EdgeIt first, EdgeIt last) {
  while (first < last) {
    const Edge &e = *first;
    if (filter(set, e.a, e.b)) {
      *first = *(--last);
    } else {
      ++first;
    }
  }
  return last;
}

static inline void filterKruskal(DisjointSet &set, EdgeIt first, EdgeIt last,
                                 int N, Edges &mst) {
  u64 M = last - first;
  if (M == 0) return;
  // if (M < 1000) return kruskal(set, first, last, N, true, mst);

  EdgeIt pivotPos = pickRandomPivot(first, last);
  EdgeIt mid = partition(first, last, pivotPos->w);

  filterKruskal(set, first, mid, N, mst);

  if (mst.size() < N - 1) addEdgeToMst(set, *pivotPos, mst);
  if (mst.size() < N - 1) {
    last = filterAll(set, mid, last);
    filterKruskal(set, mid, last, N, mst);
  }
}

static inline Edges filterKruskal(Edges &edges, int N) {
  DisjointSet set(N);
  Edges mst;
  filterKruskal(set, edges.begin(), edges.end(), N, mst);
  return mst;
}
