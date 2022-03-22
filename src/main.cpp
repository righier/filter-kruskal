#include "graphgen/randomgraphs.hpp"
#define DOCTEST_CONFIG_DISABLE
#include <doctest.h>

#include <iostream>

#include "filterkruskal.hpp"
#include "unionfind.hpp"

int main() {
  std::cout << "hello" << std::endl;

  int N = 1000;
  int M = 1000000;

  Random rnd(31);
  Edges edges;
  randomGraph(rnd, N, M, 1.0, edges);

  float cost = filterKruskal(edges, N);
  std::cout << "Result: " << cost << std::endl;
  return 0;
}