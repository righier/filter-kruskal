#include "utils.h"

#include <iostream>
#include <unordered_set>

using namespace std;

struct RandomGraphGeneratorSet: public GraphGenerator {
	int n, maxw;
	i64 m, maxm, i;
	i64 colls = 0;

	unordered_set<u64> usedEdges;

	Random &rnd;

	RandomGraphGeneratorSet(Random &_rnd, int _n, i64 _m, int _maxw):
	n(_n), maxw(_maxw), m(_m), rnd(_rnd) {
		maxm = (i64)n * ((i64)n-1) / 2;
		if (m > maxm) m = maxm;
		i = 0;
	}

	bool hasNext() {
		if (i < m) {
			return true;
		} else {
			cout << "collisions: " << colls << endl;
			cout << "usedEdges: " << usedEdges.size() << endl;
			cout << i << endl;
			return false;
		}
	}

	Edge next() {
		++i;

		while (true) {
			int a = rnd.getInt(n-1);
			int b = rnd.getInt(a+1, n);

			u64 ab = (u64(a) << 32) | u64(b);
			if (usedEdges.count(ab) == 0) {
				usedEdges.insert(ab);
				int weight = rnd.getInt(maxw);
				return Edge((int)a, (int)b, weight);
			} else {
				colls++;
			}
		}

	}

};

struct RandomGraphGeneratorDouble : public GraphGenerator {
	int n, maxw;
	double ilogp;
	Random &rnd;

	int a, b;

	RandomGraphGeneratorDouble(Random &_rnd, int _n, i64 _m, int _maxw):
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
		return Edge(a, b, weight);
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

void testDistribution1() {
	Random rnd(31);

	int n = 20;
	int tests = 10000000;
	double p = 0.5;

	vector<u64> counts(n, 0);

	for (int i = 0; i < tests; i++) {
		for (int j = 0; j < n; j++) {
			if (rnd.getDouble() < p) {
				counts[j]++;
			}
		}
	}

	for (int i = 0; i < n; i++) {
		cout << counts[i] << "\n";
	}
	cout << endl;
}

void testDistribution2() {
	Random rnd(31);

	int n = 50;
	int tests = 100000000;
	double p = 0.69;

	vector<u64> counts(n, 0);

	for (int i = 0; i < tests; i++) {
		int j = -1;

		int count = 0;
		while (true) {

			do {
				double p0 = rnd.getDouble();
				double logpp = log(p0) / log(1.0-p);
				if (p == 0) {
					cout << logpp << endl;
				}
				// int skip = max((int)ceil(logpp), 1);
				// skip = ceil(logpp);
				int skip = (int)logpp + 1;
				j += skip;
			} while (j < 0);

			if (j >= n) break;

			// counts[j]++;
			count++;
			// j++;
		}
		// cout << count << endl;
		counts[count]++;
	}

	for (int i = 0; i < n; i++) {
		cout << counts[i] << "\n";
	}
	cout << endl;
}

int main(int argc, char **argv) {
	
	// testDistribution1();
	// testDistribution2();
	// return 0;

	

	if (argc == 1) {
		cout << "NOT ENOUGH ARGS" << endl;
		return 0;
	}

	int s = atoi(argv[1]);

	int n = 60000;

	u64 m = 100000000;
	int maxw = 1000000;

	Random rnd(31);

	int tot = 0;

	if (s == 0) {
		Graph g = randomGraph(rnd, n, m);
		for (Node node: g) {
			for (NodeEdge edge: node) {
				tot += edge.first + edge.second;
			}
		}
	} else if (s == 1) {
		Graph g = randomGraph2(rnd, n, m);
		for (Node node: g) {
			for (NodeEdge edge: node) {
				tot += edge.first + edge.second;
			}
		}
	} else if (s == 2) {
		BetterGraph g = randomBetterGraph(rnd, n, m);
		for (int i = 0; i < n; i++) {
			for (const NodeEdge &edge: g[i]) {
				tot += edge.first + edge.second;
			}
		}
	} else if (s == 3) {
		vector<Edge> edges = randomEdges(rnd, n, m);
		for (Edge e: edges) {
			tot += e.a + e.b + 2 * e.w;
		}
	}

	cout << tot << endl;

	return 0;

	GraphGenerator *gg;

	/*
	int iter = 100;

	for (int j = 0; j < 100; j++) {
		double count = 0;

		for (int i = 0; i < iter; i++) {

			if (s == 1) {
				gg = new RandomGraphGenerator(rnd, n, m, maxw);
			} else if (s == 2) {
				gg = new RandomGraphGeneratorDouble(rnd, n, m, maxw);
			}

			while (gg->hasNext()){
				gg->next();
				count++;
			}
		}

		cout << "average: " << (int)round(count / iter) << endl;
	}

	return 0;
*/

	// for (int i = 0; i < 100; i++) {
	// 	cout << rnd.getFloat() << endl;
	// }

	// return 0;

	for (int i = 0; i < 100; i++) {

		if (s == 1) {
			gg = new RandomGraphGenerator(rnd, n, m, maxw);
		} else if (s == 2) {
			gg = new RandomGraphGeneratorDouble(rnd, n, m, maxw);
		} else if (s == 3) {
			gg = new RandomGraphGeneratorSet(rnd, n, m, maxw);
		}

		int count = 0;
		int tot = 0;
		while (gg->hasNext()) {
			Edge e = gg->next();
			// cout << a << " " << b << endl;
			tot += e.w + e.a + e.b;
			count++;
		}

		cout << tot << endl;
		cout << "edges: " << count << endl;

		delete gg;
	}



	return 0;
}