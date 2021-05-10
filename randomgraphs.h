#pragma once

#include <algorithm>
#include <cmath>
#include <set>
#include <vector>

#include "kdtree.h"
#include "random.h"
#include "utils.h"

static Pos randomPos(Random &rnd, float s) {
  return Pos(rnd.getFloat() * s, rnd.getFloat() * s);
}

static std::vector<Pos> randomNodes(Random &rnd, int n, float maxcoord) {
  std::vector<Pos> nodes;
  nodes.reserve(n);
  for (int i = 0; i < n; i++) nodes.emplace_back(randomPos(rnd, maxcoord));
  return nodes;
}

static void removeDuplicates(Random &rnd, Edges &edges, int m) {
  auto comp = [](const Edge &a, const Edge &b) {
    return (a.a == b.a && a.b < b.b) || a.a < b.a;
  };
  auto comp2 = [](const Edge &a, const Edge &b) {
    return a.a == b.a && a.b == b.b;
  };
  std::sort(edges.begin(), edges.end(), comp);
  edges.resize(std::unique(edges.begin(), edges.end(), comp2) - edges.begin());

  std::random_shuffle(edges.begin(), edges.end(),
                      [&](u64 n) { return rnd.getUint64() % n; });
  // edges.resize(m);
}

static void printDot(const std::vector<Pos> &nodes,
                     const std::vector<Edge> &edges) {
  std::cout << "graph name {" << std::endl;
  for (int i = 0; i < nodes.size(); i++) {
    Pos pos = nodes[i];
    std::string name = "p" + std::to_string(i);
    std::cout << name << " [pos = \"" << pos.x << "," << pos.y << "!\"];"
              << std::endl;
  }
  for (Edge e : edges) {
    std::string n1 = "p" + std::to_string(e.a);
    std::string n2 = "p" + std::to_string(e.b);
    std::cout << n1 << " -- " << n2 << " [ label=\"" << e.w << "\"];"
              << std::endl;
  }
  std::cout << "}" << std::endl;
}

static Edges randomGeometricGraphFast(Random &rnd, int n, i64 m, float maxcoord,
                                      bool printDotGraph = false,
                                      bool printDotTree = false) {
  i64 maxm = i64(n) * (i64(n) - 1) / 2;
  if (m > maxm) m = maxm;
  int k = (int)std::ceil(double(m) * 2 / n);

  auto nodes = randomNodes(rnd, n, maxcoord);
  kdTree tree(nodes);

  if (printDotTree) {
    tree.printDot();
  }

  Edges edges;
  for (int i = 0; i < n; i++) {
    auto nearest = tree.closestK(nodes[i], i, k);
    for (auto result : nearest) {
      Edge e(i, result.inst->getId(), result.d);
      if (e.a > e.b) std::swap(e.a, e.b);
      edges.emplace_back(e);
    }
  }

  removeDuplicates(rnd, edges, m);

  if (printDotGraph) {
    printDot(nodes, edges);
  }
  return edges;
}

static Edges randomGeometricGraph(Random &rnd, int n, i64 m, float maxcoord,
                                  bool printDotGraph = false) {
  i64 maxm = i64(n) * (i64(n) - 1) / 2;
  if (m > maxm) m = maxm;
  int k = (int)std::ceil(double(m) * 2 / n);

  auto nodes = randomNodes(rnd, n, maxcoord);

  Edges edges;

  for (int i = 0; i < n; i++) {
    std::vector<std::pair<float, int>> cand;
    for (int j = 0; j < n; j++) {
      if (i != j) {
        float dist = dist2(nodes[i], nodes[j]);
        cand.emplace_back(std::make_pair(dist, j));
      }
    }
    std::partial_sort(cand.begin(), cand.begin() + k, cand.end());
    cand.resize(k);
    for (const auto &c : cand) {
      Edge e(i, c.second, c.first);
      if (e.a > e.b) std::swap(e.a, e.b);
      edges.push_back(e);
    }
  }

  removeDuplicates(rnd, edges, m);

  if (printDotGraph) {
    printDot(nodes, edges);
  }
  return edges;
}

static Edges randomGraph(Random &rnd, int n, i64 m, float maxw = 1.0f) {
  i64 maxm = i64(n) * (i64(n) - 1) / 2;
  if (m > maxm) m = maxm;

  // inverse logarithm of the probability of not picking an edge
  double ilogp = 1.0 / std::log(1.0 - double(m) / double(maxm));  
  int a = 0, b = 0;

  Edges edges;
  edges.reserve(m * 1.001);  // if the number of edges is big, we almost never
                             // need to resize the vector
  while (true) {
    double p0 = rnd.getDouble();
    double logpp = log(p0) * ilogp;
    int skip = std::max(int(logpp) + 1, int(1));
    b += skip;

    while (b >= n) {
      b += ++a - n + 1;
    }

    if (a >= n - 1) break;

    float weight = rnd.getFloat() * maxw;
    edges.emplace_back(Edge(a, b, weight));
  }
  return edges;
}

static Edges randomGraphOneLong(Random &rnd, int n, i64 m, float maxw) {
  Edges edges = randomGraph(rnd, n - 1, m - 1, maxw / 2.f);
  edges.push_back(Edge(0, n - 1, maxw));
  return edges;
}