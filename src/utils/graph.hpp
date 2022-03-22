#pragma once

#include "base.hpp"

typedef std::pair<float, int> NodeEdge;
typedef std::vector<NodeEdge> Node;
typedef std::vector<Node> Graph;

struct Edge {
  int a, b;
  float w;

  inline Edge() {}
  inline Edge(int a, int b, float w) : a(a), b(b), w(w) {}

  bool operator<(const Edge &other) const { return w < other.w; }
  bool operator==(const Edge &other) const { return w == other.w; }

  static bool compareNodes(const Edge &a, const Edge &b) {
    return a.a == b.a ? (a.b < b.b) : (a.a < b.a);
  }

  static bool sameNodes(const Edge &a, const Edge &b) {
    return a.a == b.a && a.b == b.b;
  }
};

typedef std::vector<Edge> Edges;
typedef std::vector<Edge>::iterator EdgeIt;

struct HalfEdge {
  int b;
  float w;  // b = other node, w = edge weight

  HalfEdge() {}
  HalfEdge(int b, float w) : b(b), w(w) {}
};

static Graph edgesToGraph(const std::vector<Edge> &edges) {
  int n = 0;
  for (const Edge &e : edges) {
    n = std::max(n, std::max(e.a, e.b) + 1);
  }

  Graph g(n);
  for (const Edge &e : edges) {
    g[e.a].emplace_back(std::make_pair(e.w, e.b));
    g[e.b].emplace_back(std::make_pair(e.w, e.a));
  }
  return g;
}

static void edgesToGraph(const Edges &edges, int N, Graph &g) {
  std::vector<int> count(N);
  for (const Edge &e : edges) {
    count[e.a]++;
    count[e.b]++;
  }

  for (int i = 0; i < N; i++) {
    g[i].resize(count[i]);
  }

  for (const Edge &e : edges) {
    g[e.a][--count[e.a]] = {e.w, e.b};
    g[e.b][--count[e.b]] = {e.w, e.a};
  }
}
