#pragma once

#include "kruskal.h"
#include "utils.h"
#include "random.h"

#include <algorithm>

// pick the first element as pivot
static inline float pickPivot(vector<Edge> &edges, ISize begin, ISize end) {
  UNUSED(edges);
  UNUSED(end);
  return edges[begin].w;
}

// pick a random pivot
static inline float pickPivotRandom(vector<Edge> &edges, ISize begin, ISize end) {
  static Random rnd(31);
  // ISize i = rnd.getInt(begin, end);
  // swap(edges[begin], edges[end]);
  return edges[rnd.getULong(begin, end)].w;
}

static inline ISize pickPivotRandomPos(vector<Edge> &edges, ISize begin, ISize end) {
  UNUSED(edges);
  static Random rnd(31);

  return rnd.getULong(begin, end);
}

static inline EdgeIt pickPivotRandomPos(EdgeIt begin, EdgeIt end) {
  static Random rnd(31);

  return begin + rnd.getULong(end - begin);
}

// sample only 3 elements: first, middle, last
static inline EdgeIt pickPivotSample3(EdgeIt begin, EdgeIt end) {
  EdgeIt a = begin;
  EdgeIt b = end - 1;
  EdgeIt c = begin + (end - begin)/2;

  if (a > b) swap(a, b);
  if (c < a) return a;
  if (b < c)
    return b;
  else
    return c;
}

// sample only 3 random elements
static inline EdgeIt pickPivotSample3Random(EdgeIt begin, EdgeIt end) {
  static Random rnd(31);
  EdgeIt a = (begin + rnd.getInt(end - begin));
  EdgeIt b = (begin + rnd.getInt(end - begin));
  EdgeIt c = (begin + rnd.getInt(end - begin));

  if (a > b) swap(a, b);
  if (c < a) return a;
  if (b < c) return b;
  else return c;
}

// sample sqrt(k) random elements
static inline float pickPivotSampleRootK(vector<Edge> &edges, ISize begin, ISize end) {
  static Random rnd(31);
  static vector<float> v;
  int samples = max(int(sqrtf(end - begin)), 1);

  v.clear();
  for (int i = 0; i < samples; i++) {
    ISize j = rnd.getInt(begin, end);
    v.push_back(edges[j].w);
  }

  // sort(v.begin(), v.end());
  nth_element(v.begin(), v.begin() + samples / 2, v.end());

  float pivot = v[samples / 2];

  return pivot;
}