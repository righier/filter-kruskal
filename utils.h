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

struct GraphGenerator {

	virtual bool hasNext() = 0;
	virtual Edge next() = 0;
};

struct RandomGraphGenerator: public GraphGenerator {
	int n, maxw;
	double ilogp;
	Random &rnd;

	int a, b;

	RandomGraphGenerator(Random &_rnd, int _n, i64 _m, int _maxw):
	n(_n), maxw(_maxw), rnd(_rnd) {
		i64 m = _m;
		i64 maxm = i64(n) * (i64(n)-1) / 2;
		if (m > maxm) m = maxm;

		ilogp = 1.0 / log(1.0 - double(m) / double(maxm));
		a = b = 0;

		advance();
	}

	bool hasNext() {
		return a < n-1;
	}

	Edge next() {
		int weight = rnd.getInt(maxw);
		advance();
		return make_tuple(weight, a, b);
	}

	void advance() {

		double p0 = rnd.getDouble();
		double logpp = log(p0) * ilogp;
		int skip = int(logpp) + 1;
		b += skip;

		while (b >= n) {
			++a;
			b = b - n + a + 1;
		}

	}
};

static inline Graph randomGraph(Random &rnd, int n, int m, int maxw = 10000000) {
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

static inline vector<Edge> randomEdges(Random &rnd, int n, i64 m, int maxw = 10000000) {
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