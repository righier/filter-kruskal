#pragma once

#include "utils.h"

struct DisjointSet {
	vector<int> p;
	vector<int> r;

	DisjointSet(int N): p(N, 0), r(N) {
		for (int i = 0; i < N; i++) p[i] = i;
	}

	int find(int x) {
		int root = x;
		while (root != p[root]) root = p[root];
		while (x != root) tie(x, p[x]) = {p[x], root};
		return root;
	}

	void merge(int a, int b) {
		int aa = find(a);
		int bb = find(b);
		if (r[aa] < r[bb]) swap(aa, bb);
		else if (r[aa] == r[bb]) r[aa]++;
		p[bb] = aa;
	}

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