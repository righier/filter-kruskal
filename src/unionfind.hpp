#pragma once

#include <doctest.h>

#include <memory>

#include "utils/base.hpp"

struct DisjointSet {
  u32 N;
  std::unique_ptr<u32[]> p;  // parent ids
  std::unique_ptr<u32[]> r;  // ranks

  DisjointSet(u32 N) : N(N), p(new u32[N]), r(new u32[N]) {
    // every node starts out in its own set
    for (std::size_t i = 0; i < N; i++) p[i] = i;
  }

  // finds the parent of x
  // iterative path compression
  inline u32 find(u32 x) {
    assert(x < N);
    u32 root = x;
    while (root != p[root]) root = p[root];
    while (x != root) {
      u32 temp = p[x];
      p[x] = root;
      x = temp;
    }
    return root;
  }

  // checks if a and b have the same parent
  bool compare(u32 a, u32 b) {
    assert(a < N);
    assert(b < N);

    u32 pa = p[a];
    u32 pb = p[b];
    if (pa == pb) return true;

    p[a] = pa = find(pa);
    p[b] = pb = find(pb);
    return pa == pb;
  }

  // checks if a and b have the same parent
  // if not merge the two sets
  // returns:
  //   true if find(a) != find(b)
  //   false otherwise
  // union by rank
  bool checkMerge(u32 a, u32 b) {
    assert(a < N);
    assert(b < N);

    u32 pa = p[a];
    u32 pb = p[b];
    if (pa == pb) return false;

    p[a] = pa = find(pa);
    p[b] = pb = find(pb);
    if (pa == pb) return false;

    if (r[pa] < r[pb])
      std::swap(pa, pb);
    else if (r[pa] == r[pb])
      r[pa]++;
    p[pb] = pa;
    return true;
  }

  // alternative find implementations:

  // naive
  u32 find0(u32 x) {
    if (p[x] == x) return x;
    return find0(p[x]);
  }

  // recursive path compression
  u32 find1(u32 x) {
    if (p[x] == x) return x;
    return (p[x] = find1(p[x]));
  }

  // path splitting
  u32 find3(u32 x) {
    while (p[x] != x) {
      u32 parent = p[x];
      p[x] = p[p[x]];
      x = parent;
    }
    return x;
  }

  // path halving
  u32 find4(u32 x) {
    while (p[x] != x) {
      p[x] = p[p[x]];
      x = p[x];
    }
    return x;
  }
};

TEST_CASE("DisjointSet") {
  int N = 10;
  DisjointSet s(N);
  for (int i = 0; i < N; i++) {
    CHECK(s.find(i) == i);
  }

  CHECK(s.checkMerge(0, 1));
  CHECK(s.checkMerge(0, 2));
  CHECK(s.checkMerge(1, 2) == false);

  CHECK(s.checkMerge(3, 4));
  CHECK(s.checkMerge(4, 5));
  CHECK(s.checkMerge(0, 5));
  CHECK(s.checkMerge(2, 3) == false);
}