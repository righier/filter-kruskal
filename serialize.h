#pragma once

#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

#include "utils.h"
#include "graph.h"

static void edgesToFile(const std::vector<Edge> &edges,
                        const std::string &path) {
  std::ofstream fout(path);
  fout << edges.size() << '\n';
  for (const Edge &e : edges) {
    fout << e.a << " " << e.b << " " << e.w << '\n';
  }
}

static void graphToFile(const Graph &g, const std::string &path) {
  std::ofstream fout(path);
  fout << g.size() << '\n';
  for (const Node &node : g) {
    fout << node.size() << '\n';
    for (const NodeEdge &edge : node) {
      fout << edge.second << " " << edge.first << '\n';
    }
  }
}

static std::vector<Edge> parseEdges(const std::string &path) {
  std::ifstream fin(path);

  i64 m;
  fin >> m;

  std::vector<Edge> edges(m);
  for (int i = 0; i < m; i++) {
    int a, b;
    float w;
    fin >> a >> b >> w;
    edges[i] = Edge(a, b, w);
  }
  return edges;
}

static Graph parseGraph(const std::string &path) {
  std::ifstream fin(path);

  i64 n;
  fin >> n;

  Graph g(n);
  for (Node &node : g) {
    int m;
    fin >> m;
    node.reserve(m);
    for (int i = 0; i < m; i++) {
      int other;
      float weight;
      fin >> other >> weight;
      node.emplace_back(std::make_pair(weight, other));
    }
  }
  return g;
}