#pragma once

#include <algorithm>

#include "unionfind.hpp"
#include "utils/graph.hpp"

// adds the edge e to the MST if it is possible
static inline bool addEdgeToMst(DisjointSet &set, const Edge &e, Edges &mst) {
  bool canAddEdge = set.checkMerge(e.a, e.b);
  if (canAddEdge) {
    mst.push_back(e);
  }
  return canAddEdge;
}

static inline void kruskal(DisjointSet &set, EdgeIt first, EdgeIt last, int N,
                           bool doSort, Edges &mst) {
  if (doSort) std::sort(first, last);

  float cost = 0;
  for (EdgeIt it = first; it < last; it++) {
    Edge &e = *it;
    if (addEdgeToMst(set, e, mst) && (mst.size() == N - 1)) {
      break;
    }
  }
}

static inline Edges kruskal(Edges &edges, int N) {
  DisjointSet set(N);
  Edges mst;
  kruskal(set, edges.begin(), edges.end(), N, true, mst);
  return mst;
}