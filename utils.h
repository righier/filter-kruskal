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
typedef uint32_t u32;
typedef uint8_t u8;

#define UNUSED(x) (void)(x)

typedef std::pair<int,int> NodeEdge;
typedef std::vector<NodeEdge> Node;
typedef std::vector<Node> Graph;

struct Edge {
	int a, b, w;

	Edge() {}
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

typedef vector<NodeEdge>::size_type HSize;

struct EdgeIterator {
	const NodeEdge *_begin, *_end;

	EdgeIterator(const NodeEdge *_begin, const NodeEdge *_end): 
	_begin(_begin), _end(_end) { }

	const NodeEdge *begin() { return _begin; }
	const NodeEdge *end() { return _end; }

};

struct LinkNode {
	int w, b;
	i64 next;

	LinkNode() {}
	LinkNode(int w, int b, i64 next): w(w), b(b), next(next) {}
};

struct LinkedGraph {
	vector<LinkNode> buffer;
	vector<i64> nodes;

	LinkedGraph(int n) : nodes(n, -1) {}

	void addHalf(int a, int b, int w) {
		i64 next = nodes[a];
		nodes[a] = buffer.size();
		buffer.emplace_back(LinkNode(w, b, next));
		nodes[a];
	}

	void add(const Edge &e) {
		addHalf(e.a, e.b, e.w);
		addHalf(e.b, e.a, e.w);
	}

};

// Representation for static sparse graphs
struct BetterGraph {
	vector<NodeEdge> edges;
	vector<int> nodes;

	BetterGraph() {}
	BetterGraph(int N, const vector<Edge> &oldEdges): nodes(N+1, 0) {

		// count how many edges there are for every vertex
		for (Edge e: oldEdges) {
			nodes[e.a+1]++;
			nodes[e.b+1]++;
		}
		// calculate the prefix sum
		for (int i = 1; i < (int)nodes.size(); i++) {
			nodes[i] += nodes[i-1];
		}
		// allocate array for edges (we store every edge 2 times)
		edges.resize(oldEdges.size() * 2);

		vector<int> it = nodes;
		for (Edge e: oldEdges) {
			edges[it[e.a]++] = make_pair(e.w, e.b);
			edges[it[e.b]++] = make_pair(e.w, e.a);
		}

	}


	BetterGraph(vector<NodeEdge> &oldEdges, vector<int> &oldNodes) {
		std::swap(edges, oldEdges);
		std::swap(nodes, oldNodes);
	}

	EdgeIterator operator[](HSize i) const {
		auto a = nodes[i];
		auto b = nodes[i+1];
		auto _begin = &edges[a];
		return EdgeIterator(_begin, _begin + (b - a));
	}

	HSize size() const {
		return nodes.size();
	}

};



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

#define MAXW INT_MAX

static inline Graph randomGraph(Random &rnd, int n, int m, int maxw = MAXW) {
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

static inline vector<Edge> randomEdges(Random &rnd, int n, i64 m, int maxw = MAXW) {
	vector<Edge> edges;
	edges.reserve(m * 1.001); // for very large graphs the number of edges is really close to m
	RandomGraphGenerator gen(rnd, n, m, maxw);
	while(gen.hasNext()) {
		edges.emplace_back(gen.next());
	}
	cout << "edges: " << edges.size() << endl;
	return edges;
}

static inline Graph randomGraph2(Random &rnd, int n, int m, int maxw = MAXW) {
	vector<Edge> edges = randomEdges(rnd, n, m, maxw);
	vector<int> counts(n, 0);
	for (Edge &edge: edges) {
		++counts[edge.a];
		++counts[edge.b];
	}

	Graph g(n);
	for (int i = 0; i < n; i++) {
		g[i].reserve(counts[i]);
	}

	for (Edge &edge: edges) {
		g[edge.a].emplace_back(std::make_pair(edge.w, edge.b));
		g[edge.b].emplace_back(std::make_pair(edge.w, edge.a));
	}

	return g;
}

static inline BetterGraph randomBetterGraph(Random &rnd, int n, i64 m, int maxw = MAXW) {
	RandomGraphGenerator gen(rnd, n, m, maxw);
	vector<int> nodes(n+2, 0);
	vector<Edge> edges;
	edges.reserve(m * 1.001);
	while(gen.hasNext()) {
		Edge e = gen.next();
		edges.push_back(e);
		++nodes[e.a+2];
		++nodes[e.b+2];
	}

	for (int i = 3; i < n+2; i++) {
		nodes[i] += nodes[i-1];
	}

	vector<NodeEdge> buffer(2*edges.size());

	for (Edge &e: edges) {
		buffer[nodes[e.a+1]++] = make_pair(e.w, e.b);
		buffer[nodes[e.b+1]++] = make_pair(e.w, e.a);
	}

	BetterGraph g(buffer, nodes);

	cout << "edges: " << edges.size() << endl;

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