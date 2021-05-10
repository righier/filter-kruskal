#pragma once

#include "utils.h"
#include "random.h"

// the maximum number of buckets for this implementation is 256 = 2^8
constexpr size_t logBuckets = 8;
constexpr size_t numBuckets = 1 << logBuckets;
constexpr size_t numPivots = numBuckets - 1;

// calculates the sample size based on an oversampling coefficient
ISize calcSampleSize(ISize M) {
  double r = sqrt(double(M) / (2 * numBuckets * (logBuckets + 4)));
  return ISize(numPivots * max(r, 1.0));
}

// extracts a sample from the list
// and puts it in the last sampleSize elements of the list
void sample(vector<Edge> &edges, ISize begin, ISize end, ISize sampleSize) {
  static Random rnd(31);

  for (ISize i = 0; i < sampleSize; i++) {
    ISize index = rnd.getULong(begin, end);
    std::swap(edges[--end], edges[index]);
  }
}

// this needs to be called with pos == 1
// AND if edges.size() >= (sampleSize + numPivots);
void buildTree(vector<Edge> &edges, ISize beginEdges, ISize beginSamples,
               ISize endSamples, vector<int> &pivots, u32 pos) {
  ISize mid = beginSamples + (endSamples - beginSamples) / 2;
  pivots[pos] = edges[mid].w;
  // swap(edges[mid], edges[beginEdges]);
  if (2 * pos < numPivots) {
    buildTree(edges, 2 * beginEdges, beginSamples, mid, pivots, 2 * pos);
    buildTree(edges, 2 * beginEdges + 1, mid + 1, endSamples, pivots,
              2 * pos + 1);
  }
}

// builds the binary search tree used to identify the buckets
// the first pivot is at position 1 in the array
void buildTree(vector<Edge> &edges, ISize begin, ISize end, vector<int> &pivots,
               ISize sampleSize) {
  buildTree(edges, begin, end - sampleSize, end, pivots, 1);
}

// returns the bucket id of the specified element
// if the element is equal to the pivot it goes to the left
// the number of buckets cannot be greater than 256
u8 findBucket(const vector<int> &pivots, int val) {
  u32 id = 1;
  for (u32 i = 0; i < logBuckets; i++) {
    id = 2 * id + (val > pivots[id]);
  }
  return u8(id - numBuckets);  // in the paper it says id - k + 1, but this
                               // should be correct
}

// bucketSizes has size numBuckets and should be allocated for every layer
// bucketIds has size edges.size() and should be allocated once
void assignBuckets(vector<Edge> &edges, ISize begin, ISize end,
                   const vector<int> &pivots, vector<ISize> &bucketSizes,
                   vector<u8> &bucketIds) {
  for (ISize i = begin; i < end; i++) {
    Edge &edge = edges[i];
    u8 bid = findBucket(pivots, edge.w);
    ++bucketSizes[bid];
    bucketIds[i] = bid;
  }
}

// calculates the offset of every bucket starting from the start of the edges
// array
void prefixSum(vector<ISize> &bucketSizes, ISize begin) {
  ISize sum = begin;
  for (u32 i = 0; i < numBuckets; i++) {
    ISize curr = bucketSizes[i];
    bucketSizes[i] = sum;
    sum += curr;
  }
}

// puts every edge into the respective bucket
void distribute(vector<Edge> &edges, vector<Edge> &outEdges, ISize begin,
                ISize end, vector<ISize> &bucketSizes, vector<u8> &bucketIds) {
  for (ISize i = begin; i < end; i++) {
    outEdges[bucketSizes[bucketIds[i]]++] = edges[i];
  }
}

float sampleKruskal(DisjointSet &set, vector<Edge> &edges, vector<Edge> &outEdges,
                  ISize begin, ISize end, int N, vector<u8> &bucketIds,
                  int &card) {
  static vector<int> pivots(numPivots);

  ISize M = end - begin;
  if (M <= 1024) {
    return kruskal(set, edges, begin, end, N, card, true);
  }

  ISize sampleSize = calcSampleSize(M);

  vector<ISize> bucketSizes(numBuckets);

  sample(edges, begin, end, sampleSize);
  buildTree(edges, begin, end, pivots, sampleSize);
  assignBuckets(edges, begin, end, pivots, bucketSizes, bucketIds);
  prefixSum(bucketSizes, begin);
  distribute(edges, outEdges, begin, end, bucketSizes, bucketIds);

  float cost = 0;

  ISize newBegin = begin;
  for (u32 i = 0; i < numBuckets; i++) {
    ISize newEnd = bucketSizes[i];
    if (i > 0) {
      newEnd = filterAll(set, outEdges, newBegin, newEnd);
    }
    cost += filterKruskalNaive(set, outEdges.begin()+newBegin, outEdges.begin()+newEnd, N, card);
    // cost += sampleKruskal(set, outEdges, edges, newBegin, newEnd, N, bucketIds, card);
    newBegin = newEnd;

    if (card >= N - 1) {
      break;
    }
  }

  return cost;
}

float sampleKruskal(vector<Edge> &edges, int N) {
  int M = edges.size();
  vector<Edge> outEdges(M);
  vector<u8> bucketIds(M);
  DisjointSet set(N);
  int card = 0;
  return sampleKruskal(set, edges, outEdges, 0, M, N, bucketIds, card);
}
