#pragma once

#include "utils.h"
#include "unionfind.h"

typedef vector<Edge>::iterator EdgeIt;
typedef vector<Edge>::size_type ISize;

bool addEdgeToMst(DisjointSet &set, const Edge &e, int &card, int &cost) {
	if (set.checkMerge(e.a, e.b)) {
		cost += e.w;
		++card;
		return true;
	} else return false;
} 

u64 kruskal(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, int N, int &card, bool doSort = true) {

	if (doSort) sort(edges.begin()+begin, edges.begin()+end);

	u64 cost = 0;
	for (auto it = begin; it != end; ++it) {
		if (addEdgeToMst(set, edges[it], card, cost) && card == N-1) {
			break;
		}
	}

	return cost;
}

u64 kruskal(vector<Edge> &edges, int N) {
	DisjointSet set(N);
	int card = 0;
	return kruskal(set, edges, 0, edges.size(), N, card);
}