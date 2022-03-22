#pragma once

#include <algorithm>

#include "unionfind.hpp"
#include "utils/graph.hpp"

// adds the edge e to the MST if it is possible
static inline bool addEdgeToMst(DisjointSet &set, const Edge &e, int &card,
                                float &cost) {
  bool canAddEdge = set.checkMerge(e.a, e.b);
  if (canAddEdge) {
    cost += e.w;
    ++card;
  }
  return canAddEdge;
}

static inline float kruskal(DisjointSet &set, EdgeIt first, EdgeIt last, int N,
                            int &card, bool doSort) {
  if (doSort) std::sort(first, last);

  float cost = 0;
  for (EdgeIt it = first; it < last; it++) {
    Edge &e = *it;
    if (addEdgeToMst(set, e, card, cost) && (card == N - 1)) {
      break;
    }
  }
  return cost;
}

static inline float kruskal(std::vector<Edge> &edges, int N) {
  DisjointSet set(N);
  int card = 0;
  return kruskal(set, edges.begin(), edges.end(), N, card, true);
}