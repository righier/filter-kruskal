#pragma once

#include <math.h>

#include <algorithm>

#include "utils/graph.hpp"
#include "utils/random.hpp"

// picks the pivot randomply from the edge list and moves it to the start of the
// list
static inline EdgeIt pickRandomPivot(EdgeIt &first, EdgeIt &last) {
  static Random rnd(31);
  EdgeIt pivotPos = first + rnd.getULong(last - first);
  std::swap(*pivotPos, *first);
  return first++;
}

// picks sqrt(k) samples and returns the start iterator of the samples
// (the prefix is filled with the samples)
static inline EdgeIt pickRandomSampleRootK(EdgeIt &first, EdgeIt &last) {
  static Random rnd(31);
  int nSamples = std::max(int(std::sqrt(last - first)), 1);

  EdgeIt firstSample = first;
  for (int i = 0; i < nSamples; i++) {
    EdgeIt picked = first + rnd.getULong(last - first);
    std::swap(*(first++), *picked);
  }

  std::sort(firstSample, first);
  return firstSample;
}