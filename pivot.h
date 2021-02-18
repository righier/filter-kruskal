#pragma once

#include "utils.h"
#include "kruskal.h"

// pick the first element as pivot
int pickPivot(vector<Edge> &edges, ISize begin, ISize end) {
	UNUSED(edges);
	UNUSED(end);
	return edges[begin].w;
}

// pick a random pivot
int pickPivotRandom(vector<Edge> &edges, ISize begin, ISize end) {
	static Random rnd(31);
	// ISize i = rnd.getInt(begin, end);
	// swap(edges[begin], edges[end]);
	return edges[rnd.getULong(begin, end)].w;
}

ISize pickPivotRandomPos(vector<Edge> &edges, ISize begin, ISize end) {
	UNUSED(edges);
	static Random rnd(31);

	return rnd.getULong(begin, end);
}

// sample only 3 elements: first, middle, last
int pickPivotSample3(vector<Edge> &edges, ISize begin, ISize end) {
	int a = edges[begin].w;
	int b = edges[end-1].w;
	int c = edges[(begin+end)/2].w;

	if (a > b) swap(a, b);
	if (c < a) return a;
	if (b < c) return b;
	else return c;
}

// sample only 3 random elements
int pickPivotSample3Random(vector<Edge> &edges, ISize begin, ISize end) {
	static Random rnd(31);
	int a = edges[rnd.getInt(begin, end)].w;
	int b = edges[rnd.getInt(begin, end)].w;
	int c = edges[rnd.getInt(begin, end)].w;

	if (a > b) swap(a, b);
	if (c < a) return a;
	if (b < c) return b;
	else return c;
}

// sample sqrt(k) random elements
int pickPivotSampleRootK(vector<Edge> &edges, ISize begin, ISize end) {
	static Random rnd(31);
	static vector<int> v;
	int samples = max(int(sqrtf(end - begin)), 1);

	v.clear();
	for (int i = 0; i < samples; i++) {
		ISize j = rnd.getInt(begin, end);
		v.push_back(edges[j].w);
	}

	// sort(v.begin(), v.end());
	nth_element(v.begin(), v.begin() + samples/2, v.end());

	int pivot = v[samples/2];

	return pivot;
}