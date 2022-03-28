#include "filterkruskal.hpp"
#include "kruskal.hpp"
#include "unionfind.hpp"
#include "utils/graph.hpp"

// fast solution initialization to speed up kruskal and derivates
// TODO: fix for instances where edges with equal weight repeat
static inline void partialMst(DisjointSet &set, EdgeIt first, EdgeIt last,
                              int N, Edges &mst) {
  std::vector<EdgeIt> bestEdge(N, last);

  auto updateBest = [&](int nodeId, EdgeIt newEdge) {
    EdgeIt oldEdge = bestEdge[nodeId];
    if (oldEdge == last || oldEdge->w > newEdge->w) {
      bestEdge[nodeId] = newEdge;
    }
  };

  for (EdgeIt it = first; it < last; it++) {
    updateBest(it->a, it);
    updateBest(it->b, it);
  }

  for (int i = 0; i < N; i++) {
    EdgeIt e = bestEdge[i];
    addEdgeToMst(set, *e, mst);
  }
}

static inline Edges improvedKruskal(Edges &edges, int N) {
  DisjointSet set(N);
  Edges mst;
  partialMst(set, edges.begin(), edges.end(), N, mst);
  EdgeIt newEnd = filterAll(set, edges.begin(), edges.end());
  kruskal(set, edges.begin(), newEnd, N, true, mst);
  return mst;
}