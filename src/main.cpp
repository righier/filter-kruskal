#include <chrono>
#include <ratio>

#define DOCTEST_CONFIG_DISABLE
#include <doctest.h>

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

#include <iostream>

#include "filterkruskal.hpp"
#include "graphgen/randomgraphs.hpp"
#include "optimalpivot.hpp"
#include "partialmst.hpp"
#include "unionfind.hpp"

int main() {
  Random rnd(23);

  // int N = 10;
  // int M = 20;
  // Edges edges;
  // randomGraphOneLong(rnd, N, M, 1.0, edges);
  // Edges pivots = findBestPivots(edges, N);
  // std::cout << "OK" << std::endl;
  // std::cout << pivots << std::endl;

  // return 0;

  ankerl::nanobench::Bench bench;
  bench.timeUnit(std::chrono::milliseconds(1), "ms");

  for (int M = 10; M < 1000000; M *= 1.5) {
    int N = std::max((int)std::sqrt(M), 2);
    std::cout << N << " " << M << std::endl;

    Edges edges;
    randomGraph(rnd, N, M, 1.0, edges);

    bench.complexityN(M).minEpochIterations(100).run("FilterKruskal", [&] {
      Edges edgesCopy = edges;
      Edges mst = filterKruskal(edgesCopy, N);
      ankerl::nanobench::doNotOptimizeAway(mst);
    });

    // bench.complexityN(M).minEpochIterations(100).run("Kruskal", [&] {
    //   Edges edgesCopy = edges;
    //   Edges mst = kruskal(edgesCopy, N);
    //   ankerl::nanobench::doNotOptimizeAway(mst);
    // });

    // bench.complexityN(M).run("FindPivot", [&] {
    //   Edges pivots = findBestPivots(edges, N);
    //   ankerl::nanobench::doNotOptimizeAway(pivots);
    // });
  }
  std::cout << bench.complexityBigO() << std::endl;

  return 0;
}