#include "graphgen/randomgraphs.hpp"
#define DOCTEST_CONFIG_DISABLE
#include <doctest.h>
#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

#include <iostream>

#include "filterkruskal.hpp"
#include "unionfind.hpp"

int main() {
  std::cout << "hello" << std::endl;

  int N = 1000;
  int M = 1000000;

  Random rnd(31);
  Edges edges;
  float cost1, cost2;

  ankerl::nanobench::Bench().minEpochIterations(100).run(
      "Graph generation", [&] { randomGraph(rnd, N, M, 1.0, edges); });

  std::cout << edges.size() << std::endl;

  ankerl::nanobench::Bench().minEpochIterations(5).run("Kruskal", [&] {
    Edges edgesCopy = edges;
    cost1 = kruskal(edgesCopy, N);
  });

  ankerl::nanobench::Bench().minEpochIterations(100).run("FilterKruskal", [&] {
    Edges edgesCopy = edges;
    cost2 = filterKruskal(edgesCopy, N);
  });

  std::cout << "Result: " << cost1 << " " << cost2 << std::endl;
  return 0;
}