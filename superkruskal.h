
/*******************************************************************************
 * ssssort.h
 *
 * Super Scalar Sample Sort
 *
 *******************************************************************************
 * Copyright (C) 2014 Timo Bingmann <tb@panthema.net>
 * Copyright (C) 2016 Lorenz Hübschle-Schneider <lorenz@4z2.de>
 * Copyright (C) 2016 Morwenn <morwenn29@hotmail.fr>
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <iterator>
#include <memory>
#include <random>

// Compiler hints about invariants, inspired by ICC's __assume()
#define __assume(cond)                    \
  do {                                    \
    if (!(cond)) __builtin_unreachable(); \
  } while (0)

#include "filterkruskal.h"

namespace ssssort {

constexpr std::size_t basecase_size = 1024;
constexpr std::size_t logBuckets = 8;
constexpr std::size_t numBuckets = 1 << logBuckets;
using bucket_t = std::uint32_t;  // TODO: try smaller bucket size
thread_local Random rnd(31);

struct Sampler {
  static void draw_sample(EdgeIt begin, EdgeIt end, Edge* samples,
                          std::size_t sample_size) {
    assert(begin <= end);
    std::size_t max = static_cast<std::size_t>(end - begin);
    for (std::size_t i = 0; i < sample_size; ++i) {
      std::size_t index = rnd.getULong(max--);  // biased, don't care
      std::iter_swap(begin + index, begin + max);
      samples[i] = *(begin + max);
    }
  }
};

/**
 * Classify elements into buckets. Template parameter treebits specifies the
 * log2 of the number of buckets (= 1 << treebits).
 */
template <std::size_t treebits = logBuckets, typename bktsize_t = std::size_t>
struct Classifier {
  const std::size_t num_splitters = (1 << treebits) - 1;
  const std::size_t splitters_size = 1 << treebits;
  Edge splitters[1 << treebits];

  /// maps items to buckets
  bucket_t* const bktout;
  /// counts bucket sizes
  std::unique_ptr<bktsize_t[]> bktsize;

  /**
   * Constructs the splitter tree from the given samples
   */
  Classifier(const Edge* samples, const std::size_t sample_size,
             bucket_t* const bktout)
      : bktout(bktout), bktsize(std::make_unique<bktsize_t[]>(1 << treebits)) {
    std::fill(bktsize.get(), bktsize.get() + (1 << treebits), 0);
    build_recursive(samples, samples + sample_size, 1);
  }

  /// recursively builds splitter tree. Used by constructor.
  void build_recursive(const Edge* lo, const Edge* hi, std::size_t pos) {
    __assume(hi >= lo);
    const Edge* mid = lo + (hi - lo) / 2;
    splitters[pos] = *mid;

    if (2 * pos < num_splitters) {
      build_recursive(lo, mid, 2 * pos);
      build_recursive(mid + 1, hi, 2 * pos + 1);
    }
  }

  /*
   * What follows is an ugly SFINAE switch.  Theoretically, the first case
   * suffices for all types - there's no reason why passing const int& should
   * be slower than int.  However, g++ emits weird code when `key` is passed
   * by reference, so force it to dereference `key` for the call. It generates
   * the same code as when using operator>(key, splitters[i]) instead of
   * compare(splitters[i], key), which it doesn't if `key` is a reference.
   */
  /// Push an element down the tree one step. Inlined.
  constexpr bucket_t step(bucket_t i, const Edge& key) const {
    __assume(i > 0);
    return 2 * i + (splitters[i] < key);
  }

  /// Find the bucket for a single element
  constexpr bucket_t find_bucket(const Edge& key) const {
    bucket_t i = 1;
    while (i <= num_splitters) i = step(i, key);
    return (i - static_cast<bucket_t>(splitters_size));
  }

  /**
   * Find the bucket for U elements at the same time. This version will be
   * unrolled by the compiler.  Degree of unrolling is a template parameter, 4
   * is a good choice usually.
   */
  template <int U>
  inline void find_bucket_unroll(EdgeIt key, bucket_t* __restrict__ obkt) {
    bucket_t i[U];
    for (int u = 0; u < U; ++u) i[u] = 1;

    for (std::size_t l = 0; l < treebits; ++l) {
      // step on all U keys
      for (int u = 0; u < U; ++u) i[u] = step(i[u], *(key + u));
    }
    for (int u = 0; u < U; ++u) {
      i[u] -= splitters_size;
      obkt[u] = i[u];
      bktsize[i[u]]++;
    }
  }

  /// classify all elements by pushing them down the tree and saving bucket id
  inline void classify(EdgeIt begin, EdgeIt end,
                       bucket_t* __restrict__ bktout = nullptr) {
    if (bktout == nullptr) bktout = this->bktout;
    for (EdgeIt it = begin; it != end;) {
      bucket_t bucket = find_bucket(*it++);
      *bktout++ = bucket;
      bktsize[bucket]++;
    }
  }

  /// Classify all elements with unrolled bucket finding implementation
  template <int U>
  void classify_unroll(EdgeIt begin, EdgeIt end) {
    bucket_t* bktout = this->bktout;
    EdgeIt it = begin;
    for (; it + U < end; it += U, bktout += U) {
      find_bucket_unroll<U>(it, bktout);
    }
    // process remainder
    __assume(end - it <= U);
    classify(it, end, bktout);
  }

  /**
   * Distribute the elements in [in_begin, in_end) into consecutive buckets,
   * storage for which begins at out_begin.  Need to class classify or
   * classify_unroll before to fill the bktout and bktsize arrays.
   */
  template <std::size_t U>
  void distribute(EdgeIt in_begin, EdgeIt in_end, EdgeIt out_begin) {
    assert(in_begin <= in_end);
    // exclusive prefix sum
    for (std::size_t i = 0, sum = 0; i < numBuckets; ++i) {
      bktsize_t curr_size = bktsize[i];
      bktsize[i] = sum;
      sum += curr_size;
    }
    const std::size_t n = static_cast<std::size_t>(in_end - in_begin);
    std::size_t i;
    for (i = 0; i + U < n; i += U) {
      for (std::size_t u = 0; u < U; ++u) {
        *(out_begin + bktsize[bktout[i + u]]++) =
            std::move(*(in_begin + i + u));
      }
    }
    // process the rest
    __assume(n - i <= U);
    for (; i < n; ++i) {
      *(out_begin + bktsize[bktout[i]]++) = std::move(*(in_begin + i));
    }
  }
};

// Factor to multiply number of buckets by to obtain the number of samples drawn
inline std::size_t oversampling_factor(std::size_t n) {
  double r = std::sqrt(double(n) / (2 * numBuckets * (logBuckets + 4)));
  return std::max<std::size_t>(r, 1);
}

/**
 * Internal sorter (argument list isn't all that pretty).
 *
 * It is assumed that the range out_begin to out_begin + (end - begin) is valid.
 */
float ssssort_int(DisjointSet& set, EdgeIt begin, EdgeIt end, EdgeIt out_begin,
                  bucket_t* __restrict__ bktout, int N, int& card) {
  assert(begin <= end);

  if (begin == end) {
    return 0.0f;
  }

  const std::size_t n = static_cast<std::size_t>(end - begin);

  // draw and sort sample
  const std::size_t sample_size = oversampling_factor(n) * numBuckets;
  auto samples = std::make_unique<Edge[]>(sample_size);
  Sampler::draw_sample(begin, end, samples.get(), sample_size);
  std::sort(samples.get(), samples.get() + sample_size);

  if (samples[0] == samples[sample_size - 1]) {
    // All samples are equal. Clean up and fall back to std::sort
    samples.reset(nullptr);
    return kruskal(set, begin, end, N, card, true);
  }

  // classify elements
  Classifier<logBuckets> classifier(samples.get(), sample_size, bktout);
  samples.reset(nullptr);
  classifier.template classify_unroll<6>(begin, end);
  classifier.template distribute<4>(begin, end, out_begin);

  float cost = 0.f;

  // Recursive calls. offset is the offset into the arrays (/iterators) for
  // the current bucket.
  std::size_t offset = 0;
  for (std::size_t i = 0; i < numBuckets; ++i) {
    if (card >= N - 1) break;

    auto out_end =
        filterAll(set, out_begin + offset, out_begin + classifier.bktsize[i]);

    size_t size = out_end - out_begin;
    if (size == 0) continue;  // empty bucket
    if (size <= basecase_size || (n / size) < 2) {
      cost += kruskal(set, out_begin + offset, out_end, N, card, true);
    } else {
      // large bucket, apply sample sort recursively
      cost += ssssort_int(set, out_begin + offset,
                          out_end,  // = out_begin + offset + size
                          begin + offset, bktout + offset, N, card);
    }
    // offset += classifier.bktsize[i] - offset;
    offset = classifier.bktsize[i];
  }

  return cost;
}

/**
 * Sort the range [begin, end).
 *
 * Uses <= 3*(end-begin)*sizeof(value_type) bytes of additional memory
 */
float ssssort(DisjointSet& set, EdgeIt begin, EdgeIt end, int N) {
  assert(begin <= end);
  const std::size_t n = static_cast<std::size_t>(end - begin);

  int card = 0;
  if (n < basecase_size) {
    return kruskal(set, begin, end, N, card, true);
  }

  vector<Edge> out(n);
  // auto out = std::make_unique<Edge[]>(n);
  auto bktout = std::make_unique<bucket_t[]>(n);
  return ssssort_int(set, begin, end, out.begin(), bktout.get(), N, card);
}

}  // namespace ssssort

float superKruskal(Edges& edges, int N) {
  DisjointSet set(N);
  return ssssort::ssssort(set, edges.begin(), edges.end(), N);
}