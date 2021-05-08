#pragma once

#include "utils.h"
#include "graph.h"
#include "pairingheap.h"

#include <limits.h>

float prim(const Graph &g) {

	int N = g.size();
	const float INF = std::numeric_limits<float>::infinity();
	vector<PairingHeap> cost;
	cost.reserve(N);
	for (int i = 0; i < N; i++) {
		cost.push_back(PairingHeap(INF, i));
	}

	vector<int> parent(N, -1);
	vector<bool> visited(N, false);
	
	cost[0].e.first = 0;
	PairingHeap *h = &cost[0];

	while(h != NULL) {
		int node = top(h).second;
		h = pop(h);
		visited[node] = true;
		for (NodeEdge e: g[node]) {
			float val = e.first;
			int i = e.second;
			if (val < cost[i].e.first and !visited[i]) {
				h = decreaseKey(h, &cost[i], val);
				parent[i] = node;
			}
		}
	}

	float out = 0;
	for (int i = 1; i < N; i++) {
		out += cost[i].e.first;
	}

	return out;
}

u64 prim2(const BetterGraph &g) {

	int N = g.size();
	const int INF = INT_MAX;
	vector<PairingHeap> cost;
	cost.reserve(N);
	for (int i = 0; i < N; i++) {
		cost.push_back(PairingHeap(INF, i));
	}

	vector<int> parent(N, -1);
	vector<bool> visited(N, false);
	
	cost[0].e.first = 0;
	PairingHeap *h = &cost[0];

	while(h != NULL) {
		int node = top(h).second;
		h = pop(h);
		visited[node] = true;
		for (const pair<int,int> &edge: g[node]) {
			int val = edge.first;
			int i = edge.second;
			if (val < cost[i].e.first and !visited[i]) {
				h = decreaseKey(h, &cost[i], val);
				parent[i] = node;
			}
		}
	}

	u64 out = 0;
	for (int i = 1; i < N; i++) {
		out += cost[i].e.first;
	}

	return out;
}
