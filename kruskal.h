#pragma once

#include "utils.h"
#include "unionfind.h"

typedef vector<Edge>::iterator EdgeIt;
typedef vector<Edge>::size_type ISize;

u64 kruskal(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, int N, int &card, bool doSort = true) {

	if (doSort) sort(edges.begin()+begin, edges.begin()+end);

	u64 cost = 0;
	for (auto it = begin; it != end; ++it) {
		Edge &e = edges[it];
		if (set.checkMerge(e.a, e.b)) {
			cost += e.w;
			if (++card == N-1) break;
		}
	}

	return cost;
}

ISize kruskalThreshold(int N, ISize M) {
	UNUSED(M);
	return N;
}

// pick the first element as pivot
int pickPivot(vector<Edge> &edges, ISize begin, ISize end) {
	UNUSED(edges);
	UNUSED(end);
	return edges[begin].w;
}

// pick a random pivot
int pickPivotRandom(vector<Edge> &edges, ISize begin, ISize end) {
	static Random rnd(31);
	return edges[rnd.getInt(begin, end)].w;
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

bool filter(DisjointSet &set, int a, int b) {
	return set.p[a] == set.p[b] || set.find(a) == set.find(b);
}

/*
ISize twoWayPartitioning(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, int pivotVal, bool doFilter) {
	ISize it;


	return it;
}
*/

// better strategy if the array has many repetitions
pair<ISize, ISize> threeWayPartitioning(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, int pivotVal, bool doFilter) {
	static vector<Edge> temp;
	temp.clear();

	ISize endA = begin, it = begin, beginB = end;

	while (it != beginB) {
		Edge e = edges[it];

		if (doFilter && filter(set, e.a, e.b)) {
			++it;
		} else {
			if (e.w == pivotVal) {
				temp.push_back(edges[it++]);
			} else if (e.w < pivotVal) {
				if (endA != it) {
					edges[endA] = edges[it];
				}
				++endA;
				++it;
			} else {
				if (--beginB != it){
					swap(edges[it], edges[beginB]);
				}
			}
		}
	}

	copy(temp.begin(), temp.end(), edges.begin() + endA);
	return make_pair(endA, beginB);
}

u64 filterKruskalRec(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, int N, int &card, bool doFilter = false) {
	ISize M = end - begin;

	if (M <= kruskalThreshold(N, M)) {
		return kruskal(set, edges, begin, end, N, card, true);
	}

	int pivotVal = pickPivotSampleRootK(edges, begin, end);
	ISize endA, beginB;
	tie(endA, beginB) = threeWayPartitioning(set, edges, begin, end, pivotVal, doFilter);

	u64 cost = filterKruskalRec(set, edges, begin, endA, N, card);
	if (card < N-1) {
		cost += kruskal(set, edges, endA, beginB, N, card, false);
		if (card < N-1) {
			cost += filterKruskalRec(set, edges, beginB, end, N, card, true);
		}
	}

	return cost;
}

u64 filterKruskalRec2(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, int N, int &card, bool doFilter = false) {
	ISize M = end - begin;

	if (M <= kruskalThreshold(N, M)) {
		return kruskal(set, edges, begin, end, N, card, true);
	}

	int pivotVal = pickPivotRandom(edges, begin, end);
	ISize endA, beginB;
	tie(endA, beginB) = threeWayPartitioning(set, edges, begin, end, pivotVal, doFilter);

	u64 cost = filterKruskalRec2(set, edges, begin, endA, N, card);
	if (card < N-1) {
		cost += kruskal(set, edges, endA, beginB, N, card, false);
		if (card < N-1) {
			cost += filterKruskalRec2(set, edges, beginB, end, N, card, true);
		}
	}

	return cost;
}

u64 filterKruskal(vector<Edge> &edges, int N) {
	stack<pair<ISize, ISize>> ranges;
	DisjointSet set(N);

	int card = 0;
	u64 cost = 0;

	ISize begin = 0;
	ISize end = edges.size();
	bool goingLeft = true;

	while ((goingLeft || !ranges.empty()) && card < N-1) {
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
			tie(endA, beginB) = threeWayPartitioning(set, edges, begin, end, pivotVal, !goingLeft);
			ranges.emplace(make_pair(begin, endA));
			end = endA;
			goingLeft = true;
		}
	}

	return cost;
}

u64 filterKruskalRec(vector<Edge> &edges, int N) {
	DisjointSet set(N);
	int card = 0;
	return filterKruskalRec(set, edges, 0, edges.size(), N, card);
}

u64 filterKruskalRec2(vector<Edge> &edges, int N) {
	DisjointSet set(N);
	int card = 0;
	return filterKruskalRec2(set, edges, 0, edges.size(), N, card);
}

u64 kruskal(vector<Edge> &edges, int N) {
	DisjointSet set(N);
	int card = 0;
	return kruskal(set, edges, 0, edges.size(), N, card);
}