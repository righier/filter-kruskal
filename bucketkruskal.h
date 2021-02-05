#pragma once

#include "samplesort.h"

u64 bucketKruskal(DisjointSet &set, vector<Edge> &edges, vector<Edge> &outEdges, ISize begin, ISize end, int N, int &card, bool doFilter = false) {

	if (verbose()) {
		cout << "WTF" << endl;
	}

	if (doFilter) {
		filterAll(set, edges, begin, end);
	}

	ISize M = end - begin;
	if (M <= 1024) {
		return kruskal(set, edges, begin, end, N, card, true);
	}

	int minVal = edges[0].w;
	int maxVal = minVal;

	for (ISize it = begin; it < end; it++) {
		const Edge &e = edges[it];
		if (minVal > e.w) minVal = e.w;
		else if (maxVal < e.w) maxVal = e.w;
	}
	maxVal += 1;


	double stepSize = double(maxVal - minVal) / double(numBuckets);

	vector<ISize> bucketSizes(numBuckets);

	if (verbose()) {
		cout << "begin: " << begin << endl;
		cout << "end: " << end << endl;
		cout << "bucket sizes: " << bucketSizes << endl;
	}

	for (ISize it = begin; it < end; it++) {
		const Edge &e = edges[it];
		int bucketId = int(double(e.w - minVal) / stepSize);
		bucketSizes[bucketId]++;
	}

	if (verbose()) {
		cout << "bucket sizes: " << bucketSizes << endl;
	}


	// sample(edges, begin, end, sampleSize);
	// buildTree(edges, begin, end, pivots, sampleSize);
	// assignBuckets(edges, begin, end, pivots, bucketSizes, bucketIds);
	prefixSum(bucketSizes, begin);

	// distribute(edges, outEdges, begin, end, bucketSizes, bucketIds);

	for (ISize it = begin; it < end; it++) {
		const Edge &e = edges[it];
		int bucketId = int(double(e.w - minVal) / stepSize);
		outEdges[bucketSizes[bucketId]++] = e;
	}

	if (verbose()) {
		cout << "prefix sums: " << bucketSizes << endl;
	}

	u64 cost = 0;

	ISize newBegin = begin;
	for (u32 i = 0; i < numBuckets; i++) {
		ISize newEnd = bucketSizes[i];

		if (verbose()) {
			cout << endl;
			cout << "newBegin: " << newBegin << endl;
			cout << "newEnd: " << newEnd << endl;
		}

		doFilter = i > 0;
		cost += filterKruskalRec(set, edges, newBegin, newEnd, N, card, doFilter);
		// cost += bucketKruskal(set, outEdges, edges, newBegin, newEnd, N, card, doFilter);

		if (card >= N-1) {
			break;
		}

		newBegin = newEnd;
	}

	return cost;

}

u64 bucketKruskal(vector<Edge> &edges, int N) {
	int M = edges.size();
	vector<Edge> outEdges(M);
	DisjointSet set(N);
	int card = 0;
	return bucketKruskal(set, edges, outEdges, 0, M, N, card);
}