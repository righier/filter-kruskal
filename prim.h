#pragma once

#include <limits.h>

#include "graph.h"
#include "pairingheap.h"
#include "utils.h"

static inline float prim(const Graph &g) {
  int N = g.size();
  const float INF = std::numeric_limits<float>::infinity();
  vector<PairingHeap> cost;
  cost.reserve(N);
  for (int i = 0; i < N; i++) {
    cost.push_back(PairingHeap(INF, i));
  }

  vector<int> parent(N, -1);
  vector<bool> visited(N, false);

  for (int j = 0; j < N; j++) {
    if (cost[j].e.first == INF) {
      cost[j].e.first = 0;
      PairingHeap *h = &cost[j];

      while (h != NULL) {
        int node = top(h).second;
        h = pop(h);
        visited[node] = true;
        for (NodeEdge e : g[node]) {
          float val = e.first;
          int i = e.second;
          if (val < cost[i].e.first and !visited[i]) {
            h = decreaseKey(h, &cost[i], val);
            parent[i] = node;
          }
        }
      }
    }
  }

  float out = 0;
  for (int i = 1; i < N; i++) {
    out += cost[i].e.first;
  }

  return out;
}
