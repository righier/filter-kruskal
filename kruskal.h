#pragma once

#include "unionfind.h"
#include "utils.h"

#include <algorithm>

typedef vector<Edge>::iterator EdgeIt;
typedef vector<Edge>::size_type ISize;

static inline bool addEdgeToMst(DisjointSet &set, const Edge &e, int &card, float &cost) {
  if (set.checkMerge(e.a, e.b)) {
    cost += e.w;
    ++card;
    return true;
  } else {
    return false;
  }
}

static inline float kruskal(DisjointSet &set, EdgeIt begin, EdgeIt end, int N, int &card, bool doSort = true) {
  if (doSort) sort(begin, end);
  float cost = 0;
  for (EdgeIt it = begin; it < end; it++) {
    if (addEdgeToMst(set, *it, card, cost) && card == N - 1) {
      break;
    }
  }
  return cost;
}

static inline float kruskal(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end,
              int N, int &card, bool doSort = true) {
  if (doSort) sort(edges.begin() + begin, edges.begin() + end);

  float cost = 0;
  for (auto it = begin; it != end; ++it) {
    if (addEdgeToMst(set, edges[it], card, cost) && card == N - 1) {
      break;
    }
  }

  return cost;
}

static inline float kruskal(vector<Edge> &edges, int N) {
  DisjointSet set(N);
  int card = 0;
  return kruskal(set, edges, 0, edges.size(), N, card);
}