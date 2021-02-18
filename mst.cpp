#include <stack>

#include "utils.h"
#include "args.h"
#include "timer.h"

#include "kruskal.h"
#include "filterkruskal.h"
#include "prim.h"
#include "samplesort.h"
#include "bucketkruskal.h"

bool isVerbose = false;

bool verbose() {
	return isVerbose;
}

int main(int argc, char **argv) {
	Args args(argc, argv);

	int N = args.getInt("-n", 20000);
	int M = args.getInt("-m", 10000000);
	string S = args.getString("-s", "filterkruskalrec2");
	isVerbose = args.getBool("-v");

	Random rnd(31);

	cout << N << " " << M << endl;

	Timer<> timer;

	for (int i = 0; i < 10; i++) {

		Graph graph;
		BetterGraph bgraph;
		vector<Edge> edges;

		if (S == "prim") {
			graph = randomGraph(rnd, N, M);
		} else if (S == "prim2") {
			bgraph = randomBetterGraph(rnd, N, M);
		} else {
			edges = randomEdges(rnd, N-1, M);
			edges.push_back(Edge(0, N-1, INT_MAX));

			if (false && verbose()) {
				for (Edge e: edges) {
					cout << "(" << e.a << ") -- (" << e.b << "): " << e.w << endl;
				}
			}
		}

		timer.start();

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
		} else if (S == "bucketkruskal") {
			cost = bucketKruskal(edges, N);
		} else if (S == "prim") {
			cost = prim(graph);
		} else if (S == "prim2") {
			cost = prim2(bgraph);
		}

		double delta = timer.delta();

		cout << "cost: " << cost << " , time: " << 1000 * delta << endl;
	}

	cout << "average time: " << 1000 * timer.avg() << endl;

	return 0;
}