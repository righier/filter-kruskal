#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <numeric>
#include <tuple>
#include <utility>

#include "filterkruskal.hpp"
#include "kruskal.hpp"
#include "partition.hpp"
#include "unionfind.hpp"
#include "utils/graph.hpp"

struct BestPivotFinder {
  Edges edges;
  int N;
  Edges mst;
  std::vector<int> filteredBy, lastEdge;
  std::vector<std::vector<int>> count;

  struct Result {
    u64 cost;
    int pivot;
  };

  typedef std::pair<int, int> Pii;
  typedef std::map<Pii, Result> Mpp;
  Mpp dp;

  BestPivotFinder(const Edges &edges, int N) : edges(edges), N(N) {}

  std::vector<Edge> getBestPivots() {
    customKruskal();
    calcCount();
    std::vector<Edge> pivots;
    getPivotsRec(0, mst.size(), pivots);
    return pivots;
  }

  void customKruskal() {
    std::sort(edges.begin(), edges.end());
    DisjointSet set(N);
    mst.clear();
    lastEdge = std::vector<int>(edges.size());
    filteredBy = std::vector<int>(edges.size());
    for (int i = 0; i < edges.size(); i++) {
      if (addEdgeToMst(set, edges[i], mst)) {
        int mstEdgeId = mst.size() - 1;
        filteredBy[i] = mstEdgeId;

        for (int j = i; j < edges.size(); j++) {
          const Edge &e = edges[j];
          lastEdge[j] = mstEdgeId + 1;
          if (filteredBy[j] == 0 && set.compare(e.a, e.b)) {
            filteredBy[j] = mstEdgeId;
          }
        }
      }
    }
  }

  void calcCount() {
    count = std::vector<std::vector<int>>(mst.size() + 1);
    for (int i = 0; i < count.size(); i++) {
      auto &cc = count[i] = std::vector<int>(mst.size() + 1, 0);
    }

    // count edges inside each bucket
    for (int j = 0; j < filteredBy.size(); j++) {
      count[filteredBy[j]][lastEdge[j]] += 1;
    }

    for (int i = count.size() - 2; i >= 0; i--) {
      // horizontal prefix sum
      for (int j = 1; j < count[0].size(); j++) {
        count[i][j] += count[i][j - 1];
      }

      // vertical postfix sum
      for (int j = 1; j < count[0].size(); j++) {
        count[i][j] += count[i + 1][j];
      }
    }

    // for (int i = 0; i < count.size(); i++) {
    //   std::cout << i << ": " << count[i] << std::endl;
    // }
  }

  static u64 findCost() { return 1; }
  static u64 compareCost() { return findCost() * 2 + 1; }
  static u64 mergeCost() { return compareCost(); }

  u64 calcCost(int l, int r, int pivot, int depth = 0) {
    assert(l >= 0);
    assert(r <= mst.size());
    assert(l <= r);
    assert(l <= pivot);
    assert(pivot < r);

    float cost = count[l][r] - count[l][l] - 1;  // partition cost
    cost += solve(l, pivot, depth + 1).cost;     // left solve
    cost += mergeCost();                         // merge pivot cost
    if (pivot < mst.size() - 1) {                // is last pivot?
      cost +=
          compareCost() * (count[l][r] - count[l][pivot] - 1);  // right filter
      cost += solve(pivot + 1, r, depth + 1).cost;              // right solve
    }
    return cost;
  }

  Result solve(int l, int r, int depth = 0) {
    assert(l >= 0);
    assert(r <= mst.size());
    assert(l <= r);

    // base case
    if (r - l == 0) {
      return {0, -1};
    }

    // recursive case
    if (dp.count({l, r}) == 0) {
      Result best = {0, -1};
      for (int pivot = l; pivot < r; pivot++) {
        u64 cost = calcCost(l, r, pivot, depth);
        if (best.cost == 0 || cost < best.cost) {
          best = {cost, pivot};
        }
      }
      assert(best.cost > 0);  // impossible
      dp[{l, r}] = best;
    }
    // std::cout << std::string(depth, ' ') << dp[{l, r}].first << std::endl;
    return dp[{l, r}];
  }

  void getPivotsRec(int l, int r, Edges &pivots) {
    assert(l >= 0);
    assert(r <= mst.size());
    assert(l <= r);
    int p = solve(l, r).pivot;
    if (p == -1) return;
    assert(p >= l);
    assert(p < r);

    pivots.push_back(mst[p]);
    getPivotsRec(l, p, pivots);
    getPivotsRec(p + 1, r, pivots);
  }
};

static inline std::vector<Edge> findBestPivots(const Edges &edges, int N) {
  BestPivotFinder finder(edges, N);
  auto pivots = finder.getBestPivots();
  return pivots;
}

static inline void filterKruskalSeeded(DisjointSet &set, Edges &edges,
                                       int first, int last, int N, Edges &mst,
                                       EdgeIt &nextPivot) {
  u64 M = last - first;
  if (M == 0) return;
  if (M < 100)
    return kruskal(set, edges.begin() + first, edges.begin() + last, N, true,
                   mst);

  Edge pivot = *nextPivot++;
  int mid = partitionSkipPivot(edges, first, last, pivot);

  filterKruskalSeeded(set, edges, first, mid, N, mst, nextPivot);

  if (mst.size() < N - 1) addEdgeToMst(set, pivot, mst);
  if (mst.size() < N - 1) {
    last = filterAll(set, edges.begin() + mid, edges.begin() + last) -
           edges.begin();
    filterKruskalSeeded(set, edges, mid, last, N, mst, nextPivot);
  }
}

static inline Edges filterKruskalSeeded(Edges &edges, int N, Edges &pivots) {
  // ciao
  DisjointSet set(N);
  EdgeIt nextPivot = pivots.begin();
  Edges mst;
  filterKruskalSeeded(set, edges, 0, edges.size(), N, mst, nextPivot);
  return mst;
}
