#pragma once

#include "utils.h"

typedef std::pair<float,int> NodeEdge;
typedef std::vector<NodeEdge> Node;
typedef std::vector<Node> Graph;

struct Edge {
    int a, b;
    float w;

    Edge() {}
    Edge(int a, int b, float w): a(a), b(b), w(w) {}

    bool operator<(const Edge &other) {
        return w < other.w;
    }
};

typedef std::vector<Edge> Edges;

struct HalfEdge {
	int b;
	float w; // b = other node, w = edge weight

	HalfEdge() {}
	HalfEdge(int b, float w): b(b), w(w) {}
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
    float w;
	int b;
	i64 next;

	LinkNode() {}
	LinkNode(float w, int b, i64 next): w(w), b(b), next(next) {}
};

struct LinkedGraph {
	vector<LinkNode> buffer;
	vector<i64> nodes;

	LinkedGraph(int n) : nodes(n, -1) {}

	void addHalf(int a, int b, float w) {
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

static std::vector<Edge> generatorToEdges(GraphGenerator &gen) {
	std::vector<Edge> edges;
	while(gen.hasNext()) {
		edges.emplace_back(gen.next());
	}
	return edges;
}

static Graph edgesToGraph(const std::vector<Edge> &edges) {
    int n = 0;
    for (const Edge &e: edges) {
        n = std::max(n, std::max(e.a, e.b)+1);
    }

    Graph g(n);
    for (const Edge &e: edges) {
        g[e.a].emplace_back(std::make_pair(e.w, e.b));
        g[e.b].emplace_back(std::make_pair(e.w, e.a));
    }
    return g;
}
