#pragma once

#include <algorithm>

#include "kruskal.hpp"
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

static inline float filterKruskal(DisjointSet &set, EdgeIt first, EdgeIt last,
                                  int N, int &card) {
  u64 M = last - first;
  if (M == 0) return 0;

  EdgeIt pivotPos = pickRandomPivot(first, last);
  float pivotVal = pivotPos->w;
  std::iter_swap(pivotPos, first);
  pivotPos = first++;

  EdgeIt mid = std::partition(
      first, last, [pivotVal](const Edge &e) { return e.w < pivotVal; });

  float cost = filterKruskal(set, first, mid, N, card);

  if (card < N - 1) addEdgeToMst(set, *pivotPos, card, cost);
  if (card < N - 1) {
    last = filterAll(set, mid, last);
    cost += filterKruskal(set, mid, last, N, card);
  }

  return cost;
}

static inline float filterKruskal(Edges &edges, int N) {
  DisjointSet set(N);
  int card = 0;
  return filterKruskal(set, edges.begin(), edges.end(), N, card);
}
