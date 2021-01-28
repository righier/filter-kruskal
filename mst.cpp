#include <stack>

#include "utils.h"
#include "args.h"

#include "kruskal.h"
#include "filterkruskal.h"
#include "prim.h"
#include "samplesort.h"

int main(int argc, char **argv) {
	Args args(argc, argv);

	int N = args.getInt("-n", 20000);
	int M = args.getInt("-m", 10000000);
	string S = args.getString("-s", "filterkruskalrec2");
	bool verbose = args.getBool("-v");

	Random rnd(31);

	cout << N << " " << M << endl;

	for (int i = 0; i < 10; i++) {

		Graph graph;
		BetterGraph bgraph;
		vector<Edge> edges;

		if (S == "prim") {
			graph = randomGraph(rnd, N, M);
		} else if (S == "prim2") {
			bgraph = randomBetterGraph(rnd, N, M);
		}else {
			edges = randomEdges(rnd, N-1, M);
			edges.push_back(Edge(0, edges.size(), INT_MAX));

			if (verbose) {
				for (Edge e: edges) {
					cout << "(" << e.a << ") -- (" << e.b << "): " << e.w << endl;
				}
			}
		}

		u64 cost;

		if (S == "kruskal") {
			cost = kruskal(edges, N);
		} else if (S == "filterkruskal") {
			cost = filterKruskal(edges, N);
		} else if (S == "filterkruskalrec") {
			cost = filterKruskalRec(edges, N);
		} else if (S == "filterkruskalrec2") {
			cost = filterKruskalRec2(edges, N);
		} else if (S == "samplekruskal") {
			cost = sampleKruskal(edges, N);
		} else if (S == "prim") {
			cost = prim(graph);
		} else if (S == "prim2") {
			cost = prim2(bgraph);
		}

		cout << "cost: " << cost << endl;
	}

	return 0;
}