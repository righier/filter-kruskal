#pragma once

#include "utils.h"

struct DisjointSet {
	vector<int> p;
	vector<int> r;

	DisjointSet(int N): p(N, 0), r(N) {
		for (int i = 0; i < N; i++) p[i] = i;
	}

	// naive
	int find0(int x) {
		if (p[x] == x) return x;
		return find(p[x]);
	}

	// recursive path compression
	int find(int x) {
		if (p[x] == x) return x;
		return (p[x] = find(p[x]));
	}

	// iterative path compression
	int find2(int x) {
		int root = x;
		while (root != p[root]) root = p[root];
		while (x != root) {
			int temp = p[x];
			p[x] = root;
			x = temp;
		}
		return root;
	}

	// path splitting
	int find3(int x) {
		while (p[x] != x) {
			int parent = p[x];
			p[x] = p[p[x]];
			x = parent;
		}
		return x;
	}

	// path halving
	int find4(int x) {
		while (p[x] != x) {
			p[x] = p[p[x]];
			x = p[x];
		}
		return x;
	}

	// union by rank
	void merge(int a, int b) {
		int aa = find(a);
		int bb = find(b);
		if (r[aa] < r[bb]) swap(aa, bb);
		else if (r[aa] == r[bb]) r[aa]++;
		p[bb] = aa;
	}

	// union by rank
	bool checkMerge(int a, int b) {
		int aa = find(a);
		int bb = find(b);
		if (aa == bb) return false;

		if (r[aa] < r[bb]) swap(aa, bb);
		else if (r[aa] == r[bb]) r[aa]++;
		p[bb] = aa;
		return true;
	}

};