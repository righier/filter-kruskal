#pragma once

#include "utils.h"

struct DisjointSet {
  vector<u32> p;
  vector<u32> r;

  DisjointSet(u32 N) : p(N, 0), r(N, 0) {
    for (u32 i = 0; i < N; i++) p[i] = i;
  }

  // naive
  u32 find0(u32 x) {
    if (p[x] == x) return x;
    return find(p[x]);
  }

  // recursive path compression
  u32 find1(u32 x) {
    if (p[x] == x) return x;
    return (p[x] = find(p[x]));
  }

  // iterative path compression
  inline u32 find(u32 x) {
    u32 root = x;
    while (root != p[root]) root = p[root];
    while (x != root) {
      u32 temp = p[x];
      p[x] = root;
      x = temp;
    }
    return root;
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

  bool compare(u32 a, u32 b) {
    u32 pa = p[a];
    u32 pb = p[b];
    if (pa == pb) return true;
    p[a] = pa = find(pa);
    p[b] = pb = find(pb);
    return pa == pb;
  }

  // union by rank
  bool checkMerge(u32 a, u32 b) {
    assert(a < p.size());
    assert(b < p.size());
    u32 pa = p[a];
    u32 pb = p[b];
    if (pa == pb) return false;
    p[a] = pa = find(pa);
    p[b] = pb = find(pb);
    if (pa == pb) return false;

    if (r[pa] < r[pb])
      swap(pa, pb);
    else if (r[pa] == r[pb])
      r[pa]++;
    p[pb] = pa;
    return true;
  }

};