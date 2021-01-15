#include <stack>

#include "utils.h"
#include "args.h"

#include "kruskal.h"
#include "prim.h"

int main(int argc, char **argv) {
	Args args(argc, argv);

	int N = args.getInt("-n", 20000);
	int M = args.getInt("-m", 100000000);
	string S = args.getString("-s", "filterkruskal");

	Random rnd(31);

	cout << N << " " << M << endl;

	for (int i = 0; i < 10; i++) {

		u64 cost;

		if (S == "kruskal") {
			auto edges = randomEdges(rnd, N, M);
			cost = kruskal(edges, N);

		} else if (S == "filterkruskal") {
			auto edges = randomEdges(rnd, N, M);
			cost = filterKruskal2(edges, N);

		} else if (S == "prim") {
			auto graph = randomGraph(rnd, N, M);
			cost = prim(graph);

		}

		cout << "cost: " << cost << endl;
	}

	return 0;
}