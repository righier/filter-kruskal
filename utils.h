#pragma once

#include <iostream>
#include <vector>
#include <climits>
#include <cassert>
#include <tuple>
#include <algorithm>
#include <cmath>

#include "random.h"

using namespace std;

typedef std::tuple<int,int,int> Edge;
typedef std::pair<int,int> NodeEdge;
typedef std::vector<NodeEdge> Node;
typedef std::vector<Node> Graph;

typedef uint64_t u64;
typedef int64_t i64;
typedef uint8_t u8;

struct RandomGraphGenerator {
	int n, maxw;
	i64 m, maxm, i, edgeCount;
	float ilogp;
	Random &rnd;

	RandomGraphGenerator(Random &_rnd, int _n, i64 _m, int _maxw):
	n(_n), maxw(_maxw), m(_m), rnd(_rnd) {
		maxm = (i64)n * ((i64)n-1) / 2;
		if (m > maxm) m = maxm;
		ilogp = 1.0f / log(1.0f - (float)m / (float)maxm);
		i = -1;
		edgeCount = 0;
		advance();
	}

	bool hasNext() {
		return i < maxm;
	}

	Edge next() {
		++edgeCount;
		i64 a = (i64)(sqrt(0.25 + 2*i) - 0.5);
		i64 b = a*(a+1)/2 - i + n - 1;
		a = n - a - 2;
		int weight = rnd.getInt(maxw);
		advance();
		return {weight, (int)a, (int)b};
	}

	void advance() {
		float p0 = rnd.getFloat();
		float logpp = logf(p0) * ilogp;
		i64 skip = max((i64)ceilf(logpp), (i64)1);

		i += skip;
		if (!hasNext()) {
			cout << "edges: " << edgeCount << endl;
		}
	}
};

static inline Graph randomGraph(Random &rnd, int n, int m, int maxw = INT_MAX) {
	Graph g(n);
	RandomGraphGenerator gen(rnd, n, m, maxw);
	while(gen.hasNext()) {
		int weight, i, j;
		tie(weight, i, j) = gen.next();
		g[i].emplace_back(std::make_pair(weight, j));
		g[j].emplace_back(std::make_pair(weight, i));
	}
	return g;
}

static inline vector<Edge> randomEdges(Random &rnd, int n, i64 m, int maxw = INT_MAX) {
	vector<Edge> edges;
	edges.reserve(m * 1.001); // for very large graphs the number of edges is really close to m
	RandomGraphGenerator gen(rnd, n, m, maxw);
	while(gen.hasNext()) {
		edges.emplace_back(gen.next());
	}
	return edges;
}

static inline vector<Edge> graphToEdges(const Graph &g) {
	vector<Edge> edges;
	for(int i = 0; i < (int)g.size(); i++) {
		for (auto &edge: g[i]) {
			if (i < edge.second) {
				edges.emplace_back(std::make_tuple(edge.first, i, edge.second));
			}
		}
	}	
	return edges;
}