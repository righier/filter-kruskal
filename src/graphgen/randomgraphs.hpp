#pragma once

#include <algorithm>
#include <cmath>
#include <set>
#include <unordered_set>
#include <vector>

#include "../utils/base.hpp"
#include "../utils/graph.hpp"
#include "../utils/random.hpp"
#include "kdtree.hpp"

static Pos randomPos(Random &rnd, float s) {
  return Pos(rnd.getFloat() * s, rnd.getFloat() * s);
}

static std::vector<Pos> randomNodes(Random &rnd, int n, float maxcoord) {
  std::vector<Pos> nodes;
  nodes.reserve(n);
  for (int i = 0; i < n; i++) nodes.emplace_back(randomPos(rnd, maxcoord));
  return nodes;
}

static void printDot(const std::vector<Pos> &nodes,
                     const std::vector<Edge> &edges) {
  std::cout << "graph name {" << std::endl;
  for (size_t i = 0; i < nodes.size(); i++) {
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

static void randomGeometricGraphFull(Random &rnd, int n, float maxcoord,
                                     Edges &edges) {
  i64 maxm = i64(n) * (i64(n) - 1) / 2;
  edges.resize(maxm);
  auto nodes = randomNodes(rnd, n, maxcoord);
  int it = 0;
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      float d = sqrt(dist2(nodes[i], nodes[j]));
      edges[it++] = Edge(i, j, d);
    }
  }
}

static void randomGeometricGraphSeq(Random &rnd, int n, i64 m, float maxcoord,
                                    Edges &edges, bool printDotGraph = false,
                                    bool printDotTree = false) {
  i64 maxm = i64(n) * (i64(n) - 1) / 2;
  if (m >= maxm) return randomGeometricGraphFull(rnd, n, maxcoord, edges);
  // if (m >= maxm * 0.5) return randomGeometricGraphDense(rnd, n, m, maxcoord,
  // edges, printDotGraph);
  int k = (int)std::ceil(double(m) * 1.82 / n);

  auto nodes = randomNodes(rnd, n, maxcoord);
  kdTree tree(nodes);

  if (printDotTree) {
    tree.printDot();
  }

  edges.reserve(n * k);
  std::vector<int> nearest;
  for (int i = 0; i < n; i++) {
    nearest.clear();
    tree.closestK(nodes[i], i, k, nearest);
    for (size_t j = 0; j < nearest.size(); j++) {
      int other = nearest[j];
      Edge e(i, other, sqrt(dist2(nodes[i], nodes[other])));
      if (e.a == e.b) continue;

      if (e.a > e.b) std::swap(e.a, e.b);
      edges.push_back(e);
    }
  }

  sort(edges.begin(), edges.end(), Edge::compareNodes);
  edges.erase(unique(edges.begin(), edges.end(), Edge::sameNodes), edges.end());
  random_shuffle(edges.begin(), edges.end());

  if (printDotGraph) {
    printDot(nodes, edges);
  }
}

static void randomGraphFull(Random &rnd, int n, float maxw, Edges &edges) {
  i64 m = (i64)n * (n - 1) / 2;
  edges.resize(m);
  i64 i = 0;
  for (int a = 0; a < n; a++) {
    for (int b = a + 1; b < n; b++) {
      float weight = rnd.getFloat() * maxw;
      edges[i++] = Edge(a, b, weight);
    }
  }
}

static void randomGraphDense(Random &rnd, int n, i64 m, float maxw,
                             Edges &edges) {
  if (m <= 0) return;

  i64 maxm = i64(n) * (i64(n) - 1) / 2;
  if (m == maxm) return randomGraphFull(rnd, n, maxw, edges);

  double p = (double)m / (double)maxm;
  edges.clear();
  edges.reserve(m * 1.001);

  for (int a = 0; a < n; a++) {
    for (int b = a + 1; b < n; b++) {
      if (rnd.getDouble() < p) {
        float weight = rnd.getFloat() * maxw;
        edges.emplace_back(Edge(a, b, weight));
      }
    }
  }
}

static void randomGraph(Random &rnd, int n, i64 m, float maxw, Edges &edges) {
  i64 maxm = i64(n) * (i64(n) - 1) / 2;
  if (m >= maxm) return randomGraphFull(rnd, n, maxw, edges);
  // if (m > maxm * 0.63) return randomGraphDense(rnd, n, m, maxw, edges);
  if (m <= 0) return;

  if (m > maxm) m = maxm;

  // inverse logarithm of the probability of not picking an edge
  double ilogp = 1.0 / std::log(1.0 - double(m) / double(maxm));
  int a = 0, b = 0;

  edges.clear();
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
}

static void randomGraphOneLongDense(Random &rnd, int n, i64 m, float maxw,
                                    Edges &edges) {
  randomGraph(rnd, n, m, maxw / 2.f, edges);
  for (Edge &e : edges) {
    if (e.a != 0) break;
    e.w = maxw;
  }
}

static void randomGraphOneLong(Random &rnd, int n, i64 m, float maxw,
                               Edges &edges) {
  i64 maxm = (i64)n * ((i64)n - 1) / 2;
  // if (m > maxm * 0.63) return randomGraphOneLongDense(rnd, n, m, maxw,
  // edges);

  randomGraph(rnd, n - 1, m - 1, maxw / 2.f, edges);
  edges.push_back(Edge(0, n - 1, maxw));
}
