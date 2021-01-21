#pragma once

#include "utils.h"
#include "unionfind.h"

typedef vector<Edge>::iterator EdgeIt;
typedef vector<Edge>::size_type ISize;

u64 kruskal(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, int N, int &card, bool doSort = true) {
	if (doSort) {
		sort(edges.begin()+begin, edges.begin()+end);
	}

	u64 cost = 0;
	for (auto it = begin; it != end; ++it) {
		int w, a, b; tie(w, a, b) = edges[it];
		if (set.checkMerge(a, b)) {
			cost += w;
			if (++card == N-1) break;
		}
	}

	return cost;
}

double T = 1.0;

ISize kruskalThreshold(int N, ISize M) {
	UNUSED(M);
	return N;
}

int pickPivot(vector<Edge> &edges, ISize begin, ISize end) {
	UNUSED(edges);
	UNUSED(end);
	return get<0>(edges[begin]);
}

int pickPivotRandom(vector<Edge> &edges, ISize begin, ISize end) {
	static Random rnd(31);
	return get<0>(edges[rnd.getInt(begin, end)]);
}

// sample only 3 elements
int pickPivotSample3(vector<Edge> &edges, ISize begin, ISize end) {
	int a = get<0>(edges[begin]);
	int b = get<0>(edges[end-1]);
	int c = get<0>(edges[(begin+end)/2]);

	if (a > b) swap(a, b);
	if (b < c) return b;
	else return c;
}

// sample
int pickPivotSampleRootK(vector<Edge> &edges, ISize begin, ISize end) {
	static Random rnd(31);
	static vector<int> v;
	int samples = max(int(sqrtf(end - begin)), 1);

	v.clear();
	for (int i = 0; i < samples; i++) {
		ISize j = rnd.getInt(begin, end);
		v.push_back(get<0>(edges[j]));
	}

	sort(v.begin(), v.end());
	int pivot = v[samples/2];

	return pivot;
}

/*
void twoWayPartitioning(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, bool doFilter) {

}

void filter(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end) {

}
*/

u64 filterKruskalRecNaive(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, int N, int &card) {
	ISize M = end - begin;

	if (M <= kruskalThreshold(N, M)) return kruskal(set, edges, begin, end, N, card);

	int pivotVal = pickPivotRandom(edges, begin, end);

	auto endA = begin;
	auto beginB = end;

	auto it = begin;

	while (it != beginB) {
		int val, a, b;
		tie(val, a, b) = edges[it];

		if (set.find(a) == set.find(b)) {
			++it;
		} else {
			if (val < pivotVal) {
				if (endA != it) {
					edges[endA] = edges[it];
				}
				++endA;
				++it;
			} else {
				if (--beginB != it){
					swap(edges[it], edges[beginB]);
				}
			}
		}
	}

	u64 cost = filterKruskalRecNaive(set, edges, begin, endA, N, card);

	if (card < N-1) {
		cost += filterKruskalRecNaive(set, edges, beginB, end, N, card);
	}

	return cost;
}


u64 filterKruskalRec(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, int N, int &card, vector<Edge> &temp, bool doFilter) {
	ISize M = end - begin;

	if (M <= kruskalThreshold(N, M)) {
		return kruskal(set, edges, begin, end, N, card, true);
	}

	int pivotVal = pickPivotRandom(edges, begin, end);

	auto endA = begin;
	auto beginB = end;

	auto it = begin;

	temp.clear();

	while (it != beginB) {
		int val, a, b;
		tie(val, a, b) = edges[it];

		if (doFilter && set.find(a) == set.find(b)) {
			++it;
		} else {
			if (val == pivotVal) {
				temp.push_back(edges[it++]);
			} else if (val < pivotVal) {
				if (endA != it) {
					edges[endA] = edges[it];
				}
				++endA;
				++it;
			} else {
				if (--beginB != it){
					swap(edges[it], edges[beginB]);
				}
			}
		}
	}

	copy(temp.begin(), temp.end(), edges.begin() + endA);

	u64 cost = filterKruskalRec(set, edges, begin, endA, N, card, temp, false);

	if (card < N-1) {
		cost += kruskal(set, edges, endA, beginB, N, card, false);
		if (card < N-1) {
			cost += filterKruskalRec(set, edges, beginB, end, N, card, temp, true);
		}
	}


	return cost;
}

u64 filterKruskalRec2(DisjointSet &set, vector<Edge> &edges, ISize begin, ISize end, int N, int &card, vector<Edge> &temp, bool doFilter) {
	ISize M = end - begin;

	if (M <= kruskalThreshold(N, M)) {
		return kruskal(set, edges, begin, end, N, card, true);
	}

	int pivotVal = pickPivotRandom(edges, begin, end);

	auto endA = begin;
	auto beginB = end;

	auto it = begin;

	temp.clear();

	while (it != beginB) {
		int val, a, b;
		tie(val, a, b) = edges[it];

		if (doFilter && (set.p[a] == set.p[b] || set.find(a) == set.find(b))) {
			++it;
		} else {
			if (val == pivotVal) {
				temp.push_back(edges[it++]);
			} else if (val < pivotVal) {
				if (endA != it) {
					edges[endA] = edges[it];
				}
				++endA;
				++it;
			} else {
				if (--beginB != it){
					swap(edges[it], edges[beginB]);
				}
			}
		}
	}

	copy(temp.begin(), temp.end(), edges.begin() + endA);

	u64 cost = filterKruskalRec2(set, edges, begin, endA, N, card, temp, false);

	if (card < N-1) {
		cost += kruskal(set, edges, endA, beginB, N, card, false);
		if (card < N-1) {
			cost += filterKruskalRec2(set, edges, beginB, end, N, card, temp, true);
		}
	}


	return cost;
}

u64 filterKruskal(vector<Edge> &edges, int N) {
	stack<pair<ISize, ISize>> ranges;
	ranges.emplace(make_pair(0, edges.size()));

	DisjointSet set(N);

	int card = 0;
	u64 cost = 0;

	bool pivotGoLeft = false;

	while (!ranges.empty() && card < N-1) {

		ISize begin, end;
		tie(begin, end) = ranges.top();
		ranges.pop();

		ISize M = end - begin;

		if (M < kruskalThreshold(N, M)) {

			cost += kruskal(set, edges, begin, end, N, card);

		} else {

			int pivotVal = pickPivot(edges, begin, end);

			ISize endA = begin;
			ISize beginB = end;

			ISize it = begin;

			while (it != beginB) {
				int val, a, b;
				tie(val, a, b) = edges[it];

				if (set.find(a) == set.find(b)) {
					it++;
				} else {
					if (val == pivotVal) {
						if (pivotGoLeft) {
							val--;
						} 
						pivotGoLeft = !pivotGoLeft;
					}
					if (val < pivotVal) {
						if (it != endA) {
							edges[endA] = edges[it];
						}
						it++;
						endA++;
					} else {
						if (it != --beginB) {
							swap(edges[it], edges[beginB]);
						}
					}
				}
			}

			ranges.push(make_pair(beginB, end));
			ranges.push(make_pair(begin, endA));
		}
	}

	return cost;
}

u64 filterKruskalRecNaive(vector<Edge> &edges, int N) {
	DisjointSet set(N);
	int card = 0;
	return filterKruskalRecNaive(set, edges, 0, edges.size(), N, card);
}

u64 filterKruskalRec(vector<Edge> &edges, int N) {
	DisjointSet set(N);
	int card = 0;
	vector<Edge> temp;
	return filterKruskalRec(set, edges, 0, edges.size(), N, card, temp, false);
}

u64 filterKruskalRec2(vector<Edge> &edges, int N) {
	DisjointSet set(N);
	int card = 0;
	vector<Edge> temp;
	return filterKruskalRec2(set, edges, 0, edges.size(), N, card, temp, false);
}

u64 kruskal(vector<Edge> &edges, int N) {
	DisjointSet set(N);
	int card = 0;
	return kruskal(set, edges, 0, edges.size(), N, card);
}