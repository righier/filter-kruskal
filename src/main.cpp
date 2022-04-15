#include <chrono>
#include <cmath>
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

  for (int N = 10; N < 1000000; N *= 1.5) {
    int M = N * log(N);

    double minPos = 1.0;
    double maxPos = 0.0;
    double avgPos = 0.0;

    for (int i = 0; i < 100; i++) {
      Edges edges;
      randomGraph(rnd, N, M, 1.0, edges);
      int realM = edges.size();

      Edges mst = kruskal(edges, N);

      // std::cout << pivots << std::endl;
      // std::cout << mst << std::endl;

      // Edges pivots = findBestPivots(edges, N);
      int lastPos;
      for (int i = 0; i < edges.size(); i++) {
        if (Edge::sameNodes(edges[i], mst.back())) {
          lastPos = i;
          break;
        }
      }

      double relPos = lastPos / (double)M;
      minPos = std::min(minPos, relPos);
      maxPos = std::max(maxPos, relPos);
      avgPos += relPos;

      // bool pivotIsLast = Edge::sameNodes(pivots[0], mst.back());
      // std::cout << N << " " << (mst.size() == N - 1) << " " << (pivotIsLast)
      //           << " " << edges.size() << " " << pivotPos << " "
      //           << relPos << std::endl;
      // if (!pivotIsLast) {
      //   std::cout << edges << std::endl;
      //   std::cout << mst << std::endl;
      //   std::cout << pivots << std::endl;
      // }
    }
    avgPos /= 100.0;

    std::cout << N << " " << minPos << " " << avgPos << " " << maxPos
              << std::endl;

    // Edges edges;
    // randomGraph(rnd, N, M, 1.0, edges);

    // bench.complexityN(M).minEpochIterations(100).run("FilterKruskal", [&] {
    //   Edges edgesCopy = edges;
    //   Edges mst = filterKruskal(edgesCopy, N);
    //   ankerl::nanobench::doNotOptimizeAway(mst);
    // });

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
  // std::cout << bench.complexityBigO() << std::endl;
  std::cout << log(exp(1)) << std::endl;

  return 0;
}