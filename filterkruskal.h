#pragma once

#include "kruskal.h"
#include "pivot.h"

ISize kruskalThreshold(int N, ISize M) {
	UNUSED(M);
	UNUSED(N);
	return 10;
}

bool filter(DisjointSet &set, int a, int b) {
	return set.compare(a, b);
}

// filter edges and returns the new end index of the array
ISize filterAll(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end) {
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

// better strategy if the array has many repetitions
pair<ISize, ISize> threeWayPartitioning(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, float pivotVal, bool doFilter) {
	static vector<Edge> temp;
	temp.clear();

	ISize endA = begin, it = begin, beginB = end;

	while (it != beginB) {
		Edge &e = edges[it];

		if (doFilter && filter(set, e.a, e.b)) {
			++it;
		} else {
			if (e.w < pivotVal) edges[endA++] = edges[it++];
			else if (e.w > pivotVal) swap(edges[it], edges[--beginB]);
			else temp.push_back(edges[it++]);
		}
	}

	copy(temp.begin(), temp.end(), edges.begin() + endA);
	return make_pair(endA, beginB);
}

pair<ISize, ISize> twoWayPartitioning(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, float pivotVal, bool doFilter) {
	ISize endA = begin, it = begin, beginB = end;

	while (it < beginB) {
		Edge &e = edges[it];

		if (doFilter && filter(set, e.a, e.b)) {
			++it;
		} else {
			if (e.w <= pivotVal) edges[endA++] = edges[it++];
			else swap(edges[it], edges[--beginB]);
		}
	}

	return make_pair(endA, beginB);
}

float filterKruskalRec(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, int N, int &card, bool doFilter = false) {
	ISize M = end - begin;

	if (M <= kruskalThreshold(N, M)) {
		return kruskal(set, edges, begin, end, N, card, true);
	}

	ISize pivot = pickPivotRandomPos(edges, begin, end);
	float pivotVal = edges[pivot].w;
	std::swap(edges[begin], edges[pivot]);
	++begin;

	ISize endA, beginB;
	tie(endA, beginB) = twoWayPartitioning(set, edges, begin, end, pivotVal, doFilter);

	float cost = filterKruskalRec(set, edges, begin, endA, N, card, false);

	if (card < N-1) {
		cost += kruskal(set, edges, begin-1, begin, N, card, false);
		cost += filterKruskalRec(set, edges, beginB, end, N, card, false);
	}

	return cost;
}


float filterKruskalRec2(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, int N, int &card, bool doFilter = false) {
	ISize M = end - begin;

	if (M <= kruskalThreshold(N, M)) {
		return kruskal(set, edges, begin, end, N, card, true);
	}

	int pivotVal = pickPivotRandom(edges, begin, end);
	ISize endA, beginB;
	tie(endA, beginB) = threeWayPartitioning(set, edges, begin, end, pivotVal, doFilter);

	int oldCard = card;
	float cost = filterKruskalRec2(set, edges, begin, endA, N, card, false);
	if (card < N-1) {
		cost += kruskal(set, edges, endA, beginB, N, card, false);
		if (card < N-1) {
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