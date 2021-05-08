#pragma once

#include "utils.h"

struct DisjointSet {
  vector<int> p;
  vector<int> r;

  DisjointSet(int N) : p(N, 0), r(N) {
    for (int i = 0; i < N; i++) p[i] = i;
  }

  // naive
  int find0(int x) {
    if (p[x] == x) return x;
    return find(p[x]);
  }

  // recursive path compression
  int find1(int x) {
    if (p[x] == x) return x;
    return (p[x] = find(p[x]));
  }

  // iterative path compression
  int find(int x) {
    int root = x;
    while (root != p[root]) root = p[root];
    while (x != root) {
      int temp = p[x];
      p[x] = root;
      x = temp;
    }
    return root;
  }

  // path splitting
  int find3(int x) {
    while (p[x] != x) {
      int parent = p[x];
      p[x] = p[p[x]];
      x = parent;
    }
    return x;
  }

  // path halving
  int find4(int x) {
    while (p[x] != x) {
      p[x] = p[p[x]];
      x = p[x];
    }
    return x;
  }

  bool compare(int a, int b) {
    int pa = p[a];
    int pb = p[b];
    if (pa == pb) return true;
    p[a] = pa = find(pa);
    p[b] = pb = find(pb);
    return pa == pb;
  }

  // union by rank
  void merge(int a, int b) {
    int pa = find(a);
    int pb = find(b);
    if (r[pa] < r[pb])
      swap(pa, pb);
    else if (r[pa] == r[pb])
      r[pa]++;
    p[pb] = pa;
  }

  // union by rank
  bool checkMerge(int a, int b) {
    assert((u64)a < p.size());
    assert((u64)b < p.size());
    int pa = p[a];
    int pb = p[b];
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