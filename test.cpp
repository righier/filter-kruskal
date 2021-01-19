#include "utils.h"

#include <iostream>
#include <unordered_set>

using namespace std;

struct RandomGraphGenerator2: public GraphGenerator {
	int n, maxw;
	i64 m, maxm, i;
	i64 colls = 0;

	unordered_set<u64> usedEdges;

	Random &rnd;

	RandomGraphGenerator2(Random &_rnd, int _n, i64 _m, int _maxw):
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
				return make_tuple(weight, (int)a, (int)b);
			} else {
				colls++;
			}
		}

	}

};

struct RandomGraphGenerator3 : public GraphGenerator {
	int n, maxw;
	// i64 edgeCount;
	float ilogp;
	Random &rnd;

	int a, b;

	RandomGraphGenerator3(Random &_rnd, int _n, i64 _m, int _maxw):
	n(_n), maxw(_maxw), rnd(_rnd) {
		i64 m = _m;
		i64 maxm = (i64)n * ((i64)n-1) / 2;
		if (m > maxm) m = maxm;

		ilogp = 1.0f / logf(1.0f - (float)m / (float)maxm);
		a = b = 0;

		advance();
	}

	bool hasNext() {
		return a < n;
	}

	Edge next() {
		int weight = rnd.getInt(maxw);
		advance();
		return make_tuple(weight, a, b);
	}

	void advance() {
		float p0 = rnd.getFloat();
		float logpp = logf(p0) * ilogp;
		int skip = max((int)ceilf(logpp), 1);

		b += skip;

		while (b >= n) {
			++a;
			b -= n - a - 1;
		}

	}
};

int main(int argc, char **argv) {

	if (argc == 1) {
		cout << "NOT ENOUGH ARGS" << endl;
		return 0;
	}

	int s = atoi(argv[1]);

	int n = 60000;
	u64 m = 100000000;
	int maxw = 1000000;

	Random rnd(31);

	GraphGenerator *gg;


	for (int i = 0; i < 10; i++) {

		if (s == 1) {
			gg = new RandomGraphGenerator(rnd, n, m, maxw);
		} else if (s == 2) {
			gg = new RandomGraphGenerator2(rnd, n, m, maxw);
		} else if (s == 3) {
			gg = new RandomGraphGenerator3(rnd, n, m, maxw);
		}

		int tot = 0;
		while (gg->hasNext()) {
			int w, a, b;
			tie(w, a, b) = gg->next();
			tot += w + a + b;
		}

		cout << tot << endl;
	}



	return 0;
}