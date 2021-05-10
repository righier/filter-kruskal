#pragma once

#include "kruskal.h"
#include "pivot.h"

static inline ISize kruskalThreshold(int N, ISize M) {
  UNUSED(M);
  UNUSED(N);
  return 1000;
}

static inline EdgeIt partitionCopy(EdgeIt afirst, EdgeIt alast, EdgeIt bfirst, EdgeIt blast, float pivot) {
  for (EdgeIt it = afirst; it < alast; it++) {
    if (it->w < pivot) {
      *bfirst = *it;
      ++bfirst;
    } else {
      *blast = *it;
      --blast;
    }
  }
  return blast;
}

static inline EdgeIt partition(EdgeIt first, EdgeIt last, float pivot) {
  while (first < last) {
    while (first < last & first->w < pivot) ++first;
    --last;
    while (first < last & last->w >= pivot) --last;
    if (first < last) {
      iter_swap(first, last);
      ++first;
    }
  }
  return last;
}

static inline EdgeIt partition2(EdgeIt first, EdgeIt last, float pivot) {
  while (true) {
    while (true) {
      if (first == last) return first;
      else if (first->w < pivot) ++first;
      else break;
    }
    --last;
    while (true) {
      if (first == last) return first;
      else if (last->w >= pivot) --last;
      else break;
    }
    iter_swap(first, last);
    ++first;
  }
}

static inline ISize partition2(Edges &edges, ISize first, ISize last, float pivot) {
  while (true) {
    while (true) {
      if (first == last) return first;
      else if (edges[first].w < pivot) ++first;
      else break;
    }
    --last;
    while (true) {
      if (first == last) return first;
      else if (edges[last].w >= pivot) --last;
      else break;
    }
    swap(edges[first], edges[last]);
    ++first;
  }
}

static inline pair<EdgeIt, EdgeIt> partitionTwoPivot(EdgeIt first, EdgeIt last, float p, float q) {
  if (q < p) swap(p, q);
  EdgeIt l=first, k=first, g=last-1;
  while (k <= g) {
    if (k->w < p) {
      iter_swap(k, l++);
    } else if (k->w > q) {
      if (g->w < p) {
        Edge temp = *g;
        *g-- = *k;
        *k = *l;
        *l++ = temp;
      } else {
        iter_swap(k, g--);
      }
    }
    ++k;
  }
  return make_pair(l, k);
}

static inline pair<EdgeIt, EdgeIt> partition3(EdgeIt first, EdgeIt last, float p, float q) {
  if (p > q) swap(p, q);

  EdgeIt l=first, k=first, g=last-1;

  while (k <= g) {
    if (k->w < p) {
      iter_swap(k, l);
      ++l;
    } else if (k->w >= q) {
      while (g->w > q & k < g) --g;
      iter_swap(k, g);
      --g;
      if (k->w < p) {
        iter_swap(k, l);
        ++l;
      }
    }
    ++k;
  }

  return make_pair(l, g);
}

static inline bool filter(DisjointSet &set, int a, int b) { return set.compare(a, b); }

// filter edges and returns the new end index of the array
static inline ISize filterAll(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end) {
  while (begin < end) {
    Edge &e = edges[begin];
    if (filter(set, e.a, e.b)) {
      edges[begin] = edges[--end];
    } else {
      ++begin;
    }
  }
  return end;
}

template <typename Iter>
static inline Iter filterAll(DisjointSet &set, Iter begin, Iter end) {
  while (begin < end) {
    const Edge &e = *begin;
    if (filter(set, e.a, e.b)) {
      *begin = *(--end);
    } else {
      ++begin;
    }
  }
  return end;
}

static inline float filterKruskalCopy(DisjointSet &set, EdgeIt afirst, EdgeIt alast, EdgeIt bfirst, EdgeIt blast, int N, int &card) {
  ISize M = alast - afirst;
  if (M < kruskalThreshold(N, M)) {
    return kruskal(set, afirst, alast, N, card, true);
  }

  EdgeIt pivot = pickPivotRandomPos(afirst, alast);
  float pivotVal = pivot->w;

  EdgeIt bmid = partitionCopy(afirst, alast, bfirst, blast, pivotVal);
  EdgeIt amid = afirst + (bmid - bfirst);

  float cost = filterKruskalCopy(set, bfirst, bmid, afirst, amid, N, card);
  if (card < N-1) {
    blast = filterAll(set, bmid, blast);
    cost += filterKruskalCopy(set, bmid, blast, amid, alast, N, card);
  }
  return cost;
}

static inline float filterKruskalNaive2(DisjointSet &set, EdgeIt begin, EdgeIt end, int N, int &card) {
  u64 M = end - begin;
  if (M < 1000) {
    return kruskal(set, begin, end, N, card, true);
  }

  auto psize = (end - begin)/3;
  EdgeIt p = begin + psize;
  EdgeIt q = begin + 2*psize;
  float pval = p->w, qval = q->w;
  // iter_swap(begin++, p);
  // iter_swap(begin++, q);

  // EdgeIt pivot = pickPivotRandomPos(begin, end);
  // float pivotVal = pivot->w;
  // EdgeIt mid = partition(begin, end, [pivotVal](const Edge &e) {return e.w < pivotVal;});
  // EdgeIt mid = partition2(begin, end, pivotVal);

  EdgeIt endA, beginC;
  tie(endA, beginC) = partitionTwoPivot(begin, end, pval, qval);

  assert(pval <= endA->w && qval >= endA->w);
  assert(pval <= beginC->w && qval >= beginC->w);
  assert(pval > (endA-1)->w);
  assert(qval < (beginC+1)->w);

  ++beginC;

  float cost = filterKruskalNaive2(set, begin, endA, N, card);

  if (card < N-1) {
    // addEdgeToMst(set, *pivot, card, cost);
    EdgeIt endB = filterAll(set, endA, beginC);
    cost += filterKruskalNaive2(set, endA, endB, N, card);

    if (card < N-1) {
      end = filterAll(set, beginC, end);
      cost += filterKruskalNaive2(set, beginC, end, N, card);
    }
  }
  return cost;
}

#if 1

static inline float filterKruskalNaive(DisjointSet &set, EdgeIt begin, EdgeIt end, int N, int &card) {
  u64 M = end - begin;
  if (M < 500) {
    return kruskal(set, begin, end, N, card, true);
  }

  float pivotVal = pickPivotRandom(begin, end);
  // EdgeIt mid = partition(begin, end, [pivotVal](const Edge &e) {return e.w < pivotVal;});
  EdgeIt mid = partition2(begin, end, pivotVal);

  float cost = filterKruskalNaive(set, begin, mid, N, card);

  if (card < N-1) {
    // addEdgeToMst(set, *pivot, card, cost);
    end = filterAll(set, mid, end);
    cost += filterKruskalNaive(set, mid, end, N, card);
  }
  return cost;
}

#else

static inline float filterKruskalNaive(DisjointSet &set, EdgeIt begin, EdgeIt end, int N, int &card) {
  u64 M = end - begin;
  if (M < kruskalThreshold(N, M)) {
    return kruskal(set, begin, end, N, card, true);
  }

  auto psize = (end - begin)/3;
  EdgeIt p = begin + psize;
  EdgeIt q = begin + 2*psize;
  float pval = p->w, qval = q->w;
  // EdgeIt pivot = pickPivotRandomPos(begin, end);
  // float pivotVal = pivot->w;
  // EdgeIt mid = partition(begin, end, [pivotVal](const Edge &e) {return e.w < pivotVal;});
  // EdgeIt mid = partition2(begin, end, pivotVal);

  EdgeIt endA, beginC;
  tie(endA, beginC) = partition3(begin, end, p->w, p->w);

  assert(pval <= endA->w && qval >= endA->w);
  assert(pval <= beginC->w && qval >= beginC->w);
  assert(pval > (endA-1)->w);
  assert(qval < (beginC+1)->w);

  ++beginC;

  float cost = filterKruskalNaive(set, begin, endA, N, card);

  if (card < N-1) {
    // addEdgeToMst(set, *pivot, card, cost);
    EdgeIt endB = filterAll(set, endA, beginC);
    cost += filterKruskalNaive(set, endA, endB, N, card);

    if (card < N-1) {
      end = filterAll(set, beginC, end);
      cost += filterKruskalNaive(set, beginC, end, N, card);
    }
  }
  return cost;
}

#endif

template<typename SampleIt>
static inline float filterKruskalSkewed(DisjointSet &set, EdgeIt begin, EdgeIt end, SampleIt sample_begin, SampleIt sample_end, int N, int &card, bool first_iter=true) {
  ISize M = (end - begin);
  if (M < 500) {
    return kruskal(set, begin, end, N, card, true);
  }

  if (sample_begin == sample_end) {
    return filterKruskalNaive(set, begin, end, N, card);
  }

  ISize sample_size = sample_end - sample_begin;
  SampleIt pivot;
  if (first_iter) pivot = sample_begin + sample_size * std::min(0.5f, (float(N) * log2f(N) * 1.01f) / float(M));
  else pivot = sample_begin + sample_size/2;

  float pivotVal = *pivot;
  EdgeIt mid = partition2(begin, end, pivotVal);

  float cost = filterKruskalSkewed(set, begin, mid, sample_begin, pivot, N, card, false);
  if (card < N-1) {
    end = filterAll(set, mid, end);
    cost += filterKruskalSkewed(set, mid, end, pivot+1, sample_end, N, card, false);
  }
  return cost;
}

// better strategy if the array has many repetitions
static inline pair<ISize, ISize> threeWayPartitioning(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, float pivotVal, bool doFilter) {
  static vector<Edge> temp;
  temp.clear();

  ISize endA = begin, it = begin, beginB = end;

  while (it != beginB) {
    Edge &e = edges[it];

    if (doFilter && filter(set, e.a, e.b)) {
      ++it;
    } else {
      if (e.w < pivotVal)
        edges[endA++] = edges[it++];
      else if (e.w > pivotVal)
        swap(edges[it], edges[--beginB]);
      else
        temp.push_back(edges[it++]);
    }
  }

  copy(temp.begin(), temp.end(), edges.begin() + endA);
  return make_pair(endA, beginB);
}

static inline pair<ISize, ISize> twoWayPartitioning(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, float pivotVal, bool doFilter) {
  ISize endA = begin, it = begin, beginB = end;

  while (it < beginB) {
    Edge &e = edges[it];

    if (doFilter && filter(set, e.a, e.b)) {
      ++it;
    } else {
      if (e.w <= pivotVal)
        edges[endA++] = edges[it++];
      else
        swap(edges[it], edges[--beginB]);
    }
  }

  return make_pair(endA, beginB);
}

float filterKruskalRec(DisjointSet &set, vector<Edge> &edges, ISize begin,
                       ISize end, int N, int &card, bool doFilter = false) {
  ISize M = end - begin;

  if (M <= kruskalThreshold(N, M)) {
    return kruskal(set, edges, begin, end, N, card, true);
  }

  ISize pivot = pickPivotRandomPos(edges, begin, end);
  float pivotVal = edges[pivot].w;
  std::swap(edges[begin], edges[pivot]);
  ++begin;

  ISize endA, beginB;
  tie(endA, beginB) =
      twoWayPartitioning(set, edges, begin, end, pivotVal, doFilter);

  float cost = filterKruskalRec(set, edges, begin, endA, N, card, false);

  if (card < N - 1) {
    cost += kruskal(set, edges, begin - 1, begin, N, card, false);
    cost += filterKruskalRec(set, edges, beginB, end, N, card, false);
  }

  return cost;
}

float filterKruskalRec2(DisjointSet &set, vector<Edge> &edges, ISize begin,
                        ISize end, int N, int &card, bool doFilter = false) {
  ISize M = end - begin;

  if (M <= kruskalThreshold(N, M)) {
    return kruskal(set, edges, begin, end, N, card, true);
  }

  int pivotVal = pickPivotRandom(edges, begin, end);
  ISize endA, beginB;
  tie(endA, beginB) =
      threeWayPartitioning(set, edges, begin, end, pivotVal, doFilter);

  int oldCard = card;
  float cost = filterKruskalRec2(set, edges, begin, endA, N, card, false);
  if (card < N - 1) {
    cost += kruskal(set, edges, endA, beginB, N, card, false);
    if (card < N - 1) {
      doFilter = (card - oldCard) > 0;
      if (doFilter == 0) {
        cout << "saved: " << end - beginB << endl;
      }
      cost += filterKruskalRec2(set, edges, beginB, end, N, card, doFilter);
    }
  }

  return cost;
}

float filterKruskal(vector<Edge> &edges, int N) {
  stack<pair<ISize, ISize>> ranges;
  DisjointSet set(N);

  int card = 0;
  float cost = 0;

  ISize begin = 0;
  ISize end = edges.size();
  bool goingLeft = true;

  while ((goingLeft || !ranges.empty()) && card < N - 1) {
    if (!goingLeft) {
      tie(begin, end) = ranges.top();
      ranges.pop();
    }

    ISize M = end - begin;
    if (M < kruskalThreshold(N, M)) {
      cost += kruskal(set, edges, begin, end, N, card);
      goingLeft = false;
    } else {
      int pivotVal = pickPivotRandom(edges, begin, end);
      ISize endA, beginB;
      tie(endA, beginB) =
          threeWayPartitioning(set, edges, begin, end, pivotVal, !goingLeft);
      ranges.emplace(make_pair(begin, endA));
      end = endA;
      goingLeft = true;
    }
  }

  return cost;
}

float filterKruskalRec(vector<Edge> &edges, int N) {
  DisjointSet set(N);
  int card = 0;
  return filterKruskalRec(set, edges, 0, edges.size(), N, card);
}

float filterKruskalRec2(vector<Edge> &edges, int N) {
  DisjointSet set(N);
  int card = 0;
  return filterKruskalRec2(set, edges, 0, edges.size(), N, card);
}

float filterKruskalNaive(vector<Edge> &edges, int N) {
  DisjointSet set(N);
  int card = 0;
  return filterKruskalNaive(set, edges.begin(), edges.end(), N, card);
}

float filterKruskalNaive2(vector<Edge> &edges, int N) {
  DisjointSet set(N);
  int card = 0;
  return filterKruskalNaive2(set, edges.begin(), edges.end(), N, card);
}

float filterKruskalCopy(vector<Edge> &edges, int N) {
  DisjointSet set(N);
  Edges out(edges.size());
  int card = 0;
  return filterKruskalCopy(set, edges.begin(), edges.end(), out.begin(), out.end(), N, card);
}

float filterKruskalSkewed(vector<Edge> &edges, int N, float skew=0.5f) {
  DisjointSet set(N);
  auto samples = pickSamplesRootK(edges.begin(), edges.end());
  int card = 0;
  return filterKruskalSkewed(set, edges.begin(), edges.end(), samples.begin(), samples.end(), N, card, true);
}