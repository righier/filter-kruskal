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

typedef uint64_t u64;
typedef int64_t i64;
typedef uint8_t u8;

#define UNUSED(x) (void)(x)

struct Edge {
	int a, b, w;

	Edge(int a, int b, int w): a(a), b(b), w(w) {}

	bool operator<(const Edge &rhs) {
		return w < rhs.w;
	}
};

struct HalfEdge {
	int b, w; // b = other node, w = edge weight

	HalfEdge() {}
	HalfEdge(int b, int w): b(b), w(w) {}
};

typedef vector<HalfEdge>::size_type HSize;

// Representation for static sparse graphs
struct BetterGraph {
	vector<HalfEdge> edges;
	vector<HSize> nodes;

	BetterGraph() {}
	BetterGraph(int N, const vector<Edge> &oldEdges): nodes(N+1, 0) {
		// count how many edges there are for every vertex
		for (Edge e: oldEdges) {
			nodes[e.a+1]++;
			nodes[e.b+1]++;
		}
		// calculate the prefix sum
		for (HSize i = 1; i < nodes.size(); i++) {
			nodes[i] += nodes[i-1];
		}
		// allocate array for edges (we store every edge 2 times)
		edges.resize(oldEdges.size() * 2);

		vector<HSize> it = nodes;
		for (Edge e: oldEdges) {
			edges[it[e.a]++] = HalfEdge(e.b, e.w);
			edges[it[e.b]++] = HalfEdge(e.a, e.w);
		}
	}

	// returns the pointer to the first edge of the node i
	const HalfEdge *operator[](HSize i) const { 
		return &edges[nodes[i]];
	}

	HSize size() const {
		return nodes.size();
	}

};

typedef std::pair<int,int> NodeEdge;
typedef std::vector<NodeEdge> Node;
typedef std::vector<Node> Graph;

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
		auto out = Edge(a, b, weight);
		advance();
		return out;
	}

	void advance() {

		double p0 = rnd.getDouble();
		double logpp = log(p0) * ilogp;
		int skip = max(int(logpp) + 1, int(1));
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
	u64 count = 0;
	while(gen.hasNext()) {
		Edge e = gen.next();
		g[e.a].emplace_back(std::make_pair(e.w, e.b));
		g[e.b].emplace_back(std::make_pair(e.w, e.a));
		count++;
	}
	cout << "edges: " << count << endl;
	return g;
}

static inline vector<Edge> randomEdges(Random &rnd, int n, i64 m, int maxw = 10000000) {
	vector<Edge> edges;
	edges.reserve(m * 1.001); // for very large graphs the number of edges is really close to m
	RandomGraphGenerator gen(rnd, n, m, maxw);
	while(gen.hasNext()) {
		edges.emplace_back(gen.next());
	}
	cout << "edges: " << edges.size() << endl;
	return edges;
}

static inline BetterGraph randomBetterGraph(Random &rnd, int n, i64 m, int maxw = 10000000) {
	vector<Edge> edges = randomEdges(rnd, n, m, maxw);
	BetterGraph g(n, edges);
	return g;
}

static inline vector<Edge> graphToEdges(const Graph &g) {
	vector<Edge> edges;
	for(int i = 0; i < (int)g.size(); i++) {
		for (auto &edge: g[i]) {
			if (i < edge.second) {
				edges.emplace_back(Edge(i, edge.second, edge.first));
			}
		}
	}	
	return edges;
}