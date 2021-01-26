#pragma once

#include <iostream>
#include <vector>
#include <climits>
#include <cassert>
#include <tuple>
#include <algorithm>
#include <cmath>
#include <queue>

#include <stdlib.h>

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

struct EdgeIterator {
	const HalfEdge *_begin, *_end;

	EdgeIterator(const HalfEdge *_begin, const HalfEdge *_end): 
	_begin(_begin), _end(_end) { }

	const HalfEdge *begin() {
		return _begin;
	}

	const HalfEdge *end() {
		return _end;
	}

};

// Representation for static sparse graphs
struct BetterGraph {
	HalfEdge *edges;
	HalfEdge **nodes;

	int N;

	BetterGraph(): edges(NULL), nodes(NULL) {}

	BetterGraph(const BetterGraph &&old) noexcept : edges(old.edges), nodes(old.nodes) {} 

	BetterGraph &operator=(BetterGraph&& other) noexcept {
		std::swap(edges, other.edges);
        std::swap(nodes, other.nodes);
        std::swap(N, other.N);

        return *this;
    }

	BetterGraph(int N, const vector<Edge> &oldEdges): N(N) {

		// cout << sizeof(HalfEdge) << endl;
		// cout << 2 * oldEdges.size() << endl;
		// cout << sizeof(HalfEdge) * 2 * oldEdges.size() << endl;

		edges = (HalfEdge*)malloc(sizeof(HalfEdge) * 2 * oldEdges.size());
		nodes = (HalfEdge**)malloc((N+1) * sizeof(HalfEdge *));

		// cout << edges << endl;
		// cout << nodes << endl;
		// cout << nodes[0] << endl;

		// std::fill(nodes, nodes + N2, 0);

		// for (Edge e:oldEdges) {
			// cout << e.a << "--" << e.b << ": " << e.w << endl;
		// }

		// for (int i = 0; i < N; i++) {
			// cout << (u64)nodes[i] << endl;
		// }

		vector<u64> count(N, 0);

		// count how many edges there are for every vertex
		for (Edge e: oldEdges) {
			count[e.a]++;
			count[e.b]++;
		}

		// for (int i = 0; i < N2; i++) {
		// 	cout << (u64)nodes[i] << endl;
		// }
		// calculate the prefix sum
		nodes[0] = nodes[1] = edges;
		for (int i = 1; i < N; i++) {
			nodes[i+1] = edges + count[i-1];
			count[i] += count[i-1];
		}

		for (Edge e: oldEdges) {
			*(nodes[e.a+1]++) = HalfEdge(e.b, e.w);
			*(nodes[e.b+1]++) = HalfEdge(e.a, e.w);
		}

		// for (int i = 0; i < N2; i++) {
		// 	cout << nodes[i] << endl;
		// }

		// cout << "final representation:\n" << endl;
		// for (int i = 0; i < N; i++) {
		// 	for (const HalfEdge *e = &edges[nodes[i]]; e < &edges[nodes[i+1]]; e++) {
		// 		cout << i << "--" << e->b << ": " << e->w << endl;
		// 	}
		// }
	}

	~BetterGraph() {
		free(edges);
		free(nodes);
	}

	EdgeIterator operator[](u64 i) const {
		auto a = nodes[i];
		auto b = nodes[i+1];
		// auto _begin = edges + a;
		return EdgeIterator(a, b);
	}

	int size() const {
		return N;
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