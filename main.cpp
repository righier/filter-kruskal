#include <stack>

#include "utils.h"
#include "graph.h"
#include "randomgraphs.h"
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
	string S = args.getString("-s", "kruskal");
	string G = args.getString("-g", "randgraph");
	isVerbose = args.getBool("-v");

	Random rnd(31);

	cout << N << " " << M << endl;

	Timer<> timer;
	Timer<> genTimer;

	for (int i = 0; i < 10; i++) {

		Graph graph;
		vector<Edge> edges;

		genTimer.start();

		if (G == "randgraph") {
			edges = randomGraph(rnd, N, M, 1.0f);
		} else if (G == "geomgraph") {
			edges = randomGeometricGraphFast(rnd, N, M, 1.0f);
		} else if (G == "hardgraph") {
			edges = randomGraphOneLong(rnd, N, M, 1.0f);
		} else {
			cout << "invalid generator name" << endl;
			unreachable();
		}

		if (verbose()) {
			for (Edge e : edges) {
				cout << "(" << e.a << ") -- (" << e.b << "): " << e.w << endl;
			}
		}

		if (S == "prim") {
			graph = edgesToGraph(edges);
		}

		double genDelta = genTimer.delta();


		timer.start();

		float cost;

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
		} else {
			cout << "invalid algorithm name" << endl;
			unreachable();
		}

		double delta = timer.delta();

		cout << "cost: " << cost << " , time: " << 1000 * delta << ", gen+solve time: " << 1000 * (delta + genDelta) << endl;
	}

	cout << "average time: " << 1000 * timer.avg() << endl;
	cout << "gen+solve average time: " << 1000 * (timer.avg() + genTimer.avg()) << endl;

	return 0;
}