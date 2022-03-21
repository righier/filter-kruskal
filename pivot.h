#pragma once

#include <algorithm>

#include "kruskal.h"
#include "random.h"
#include "utils.h"

// pick the first element as pivot
static inline float pickPivot(vector<Edge> &edges, ISize begin, ISize end) {
  UNUSED(edges);
  UNUSED(end);
  return edges[begin].w;
}

// pick a random pivot
static inline float pickPivotRandom(vector<Edge> &edges, ISize begin,
                                    ISize end) {
  static Random rnd(31);
  // ISize i = rnd.getInt(begin, end);
  // swap(edges[begin], edges[end]);
  return edges[rnd.getULong(begin, end)].w;
}

static inline ISize pickPivotRandomPos(vector<Edge> &edges, ISize begin,
                                       ISize end) {
  UNUSED(edges);
  static Random rnd(31);

  return rnd.getULong(begin, end);
}

static inline EdgeIt pickPivotRandomPos(EdgeIt begin, EdgeIt end) {
  static Random rnd(31);
  return begin + rnd.getULong(end - begin);
}

static float pickPivotRandom(EdgeIt begin, EdgeIt end) {
  static Random rnd(31);
  auto val = rnd.getULong(end - begin);
  return begin[val].w;
}

// sample only 3 elements: first, middle, last
static inline EdgeIt pickPivotSample3(EdgeIt begin, EdgeIt end) {
  EdgeIt a = begin;
  EdgeIt b = end - 1;
  EdgeIt c = begin + (end - begin) / 2;

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
  if (b < c)
    return b;
  else
    return c;
}

static inline vector<float> pickSamplesRootK(EdgeIt begin, EdgeIt end) {
  static Random rnd(31);
  int n_samples = max(int(sqrtf(end - begin)), 1);

  vector<float> v;
  if (n_samples > (end - begin)) return v;
  v.reserve(n_samples);
  ISize max = end - begin;
  for (int i = 0; i < n_samples; i++) {
    EdgeIt j = begin + rnd.getInt(max--);
    v.push_back(j->w);
    std::iter_swap(j, begin + max);
  }

  sort(v.begin(), v.end());
  return v;
}

// sample sqrt(k) random elements
static inline float pickPivotSampleRootK(EdgeIt begin, EdgeIt end,
                                         float pos = 0.5f) {
  static Random rnd(31);
  static vector<float> v;
  int samples = max(int(sqrtf(end - begin)), 1);

  v.clear();
  ISize max = end - begin;
  for (int i = 0; i < samples; i++) {
    ISize j = rnd.getInt(max--);
    std::iter_swap(begin + j, begin + max);
    v.push_back(begin[max].w);
  }

  sort(v.begin(), v.end());
  // nth_element(v.begin(), v.begin() + index, v.end());

  int index = std::round(samples * pos);
  float pivot = v[index];
  return pivot;
}

// sample sqrt(k) random elements
static inline float pickPivotSampleRootKOld(EdgeIt begin, EdgeIt end,
                                            float pos = 0.5f) {
  static Random rnd(31);
  static vector<float> v;
  int samples = max(int(sqrtf(end - begin)), 1);

  v.clear();
  for (int i = 0; i < samples; i++) {
    ISize j = rnd.getInt(end - begin);
    v.push_back(begin[j].w);
  }

  int index = std::round(samples * pos);
  sort(v.begin(), v.end());
  // nth_element(v.begin(), v.begin() + index, v.end());

  float pivot = v[index];
  return pivot;
}