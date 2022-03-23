#include <chrono>

#include "graphgen/randomgraphs.hpp"
#define DOCTEST_CONFIG_DISABLE
#include <doctest.h>
#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

#include <iostream>

#include "filterkruskal.hpp"
#include "partialmst.hpp"
#include "unionfind.hpp"

int main() {
  std::cout << "hello" << std::endl;

  int N = 1000;
  int M = 1000000;

  Random rnd(31);

  ankerl::nanobench::Bench bench;

  for (int M = 10; M < 1000000; M *= 1.5) {
    int N = std::max((int)std::sqrt(M), 2);
    std::cout << N << " " << M << std::endl;

    Edges edges;
    randomGraph(rnd, N, M, 1.0, edges);

    bench.complexityN(M).minEpochIterations(100).run("FilterKruskal", [&] {
      Edges edgesCopy = edges;
      float cost = filterKruskal(edgesCopy, N);
      ankerl::nanobench::doNotOptimizeAway(cost);
    });

    bench.complexityN(M).minEpochIterations(100).run("Kruskal", [&] {
      Edges edgesCopy = edges;
      float cost = kruskal(edgesCopy, N);
      ankerl::nanobench::doNotOptimizeAway(cost);
    });

    bench.complexityN(M)
        .minEpochIterations(1)
        .maxEpochTime(std::chrono::nanoseconds(1))
        .run("ImprovedKruskal", [&] {
          Edges edgesCopy = edges;
          float cost = improvedKruskal(edgesCopy, N);
          ankerl::nanobench::doNotOptimizeAway(cost);
        });
  }

  return 0;
}