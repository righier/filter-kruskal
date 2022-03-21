#include <fstream>
#include <stack>

#include "args.h"
#include "bucketkruskal.h"
#include "filterkruskal.h"
#include "graph.h"
#include "kruskal.h"
#include "prim.h"
#include "randomgraphs.h"
#include "samplesort.h"
#include "superkruskal.h"
#include "timer.h"
#include "utils.h"

bool isVerbose = false;
bool verbose() { return isVerbose; }

struct RunResult {
  double time;
  double genTime;
  float cost;
};

typedef pair<Edges, Graph> GraphData;

int countDuplicates(const Edges &edges) {
  unordered_map<float, int> count;
  int best = 0;
  for (Edge e : edges) {
    best = max(best, ++count[e.w]);
  }
  return best;
}

pair<double, double> genGraph(Random &rnd, int N, int M, string &graphType,
                              Edges &edges, Graph &graph,
                              bool skipAdjList = false) {
  Timer<> timer;
  timer.start();

  if (graphType == "randgraph")
    randomGraph(rnd, N, M, 1.0f, edges);
  else if (graphType == "geomgraph")
    randomGeometricGraphSeq(rnd, N, M, 1.0f, edges);
  else if (graphType == "hardgraph")
    randomGraphOneLong(rnd, N, M, 1.0f, edges);
  else {
    cout << "invalid generator name" << endl;
    unreachable();
  }

  double t1 = timer.delta();

  // int dups = countDuplicates(edges);
  // cout << "Duplicates: " << dups << endl;

  timer.start();
  if (!skipAdjList) {
    graph = Graph(N);
    edgesToGraph(edges, N, graph);
  }
  double t2 = timer.delta();

  return {t1, t2};
}

RunResult run(int N, int M, int T, string &algorithm, string &graphType,
              Edges edges, Graph &graph, float skew = 0.5) {
  UNUSED(M);
  UNUSED(graphType);

  Timer<> timer;
  float totalCost = 0;

  timer.start();

  float cost;
  if (algorithm == "kruskal")
    cost = kruskal(edges, N);
  else if (algorithm == "filterkruskalnaive")
    cost = filterKruskalNaive(edges, N);
  else if (algorithm == "filterkruskalskewed")
    cost = filterKruskalSkewed(edges, N, skew);
  else if (algorithm == "filterkruskalnaive2")
    cost = filterKruskalNaive2(edges, N);
  else if (algorithm == "filterkruskalcopy")
    cost = filterKruskalCopy(edges, N);
  else if (algorithm == "filterkruskal")
    cost = filterKruskal(edges, N);
  else if (algorithm == "filterkruskalrec")
    cost = filterKruskalRec(edges, N);
  else if (algorithm == "filterkruskalrec2")
    cost = filterKruskalRec2(edges, N);
  else if (algorithm == "samplekruskal")
    cost = sampleKruskal(edges, N);
  else if (algorithm == "bucketkruskal")
    cost = bucketKruskal(edges, N);
  else if (algorithm == "superkruskal")
    cost = superKruskal(edges, N);
  else if (algorithm == "prim")
    cost = prim(graph);
  else {
    cout << "invalid algorithm name" << endl;
    unreachable();
  }

  double delta = timer.delta();
  totalCost += cost;

  RunResult output{delta,
                   0,  // disabled graph generation measurement for now
                   totalCost};

  return output;
}

void runBenchmark() {
  int nSteps = 14;
  int T = 20;
  vector<string> graphTypes = {"randgraph", "hardgraph", "geomgraph"};
  // vector<string> graphTypes = {"geomgraph"};
  vector<string> algos = {"prim", "filterkruskalskewed"};

  Random rnd(123498);

  for (string &graphType : graphTypes) {
    ofstream fout;
    string fileName = graphType + ".csv";
    fout.open(fileName, ofstream::out | ofstream::trunc);

    int totalTasks = (nSteps)*T * algos.size();
    int taskCount = 0;

    // print header
    fout << "N,M,";
    for (auto &s : algos) fout << s << ",";
    fout << endl;

    for (int i = 0; i < nSteps; i++) {
      double density = (double)i / (double)nSteps;
      // int M = 100000000;
      // int maxM = round(M / density);
      // int N = round(0.5 * (sqrt(8.0*maxM + 1.0) + 1.0));
      int N = 16385;
      i64 maxM = (i64)N * ((i64)N - 1) / 2;
      i64 M = (i64)N * (1LL << i);

      cout << M << endl;

      vector<double> totalTime(algos.size(), 0);

      double avgM = 0;

      for (int i = 0; i < T; i++) {
        Timer<> genTimer;
        genTimer.start();

        Edges edges;
        Graph graph;

        auto genTime = genGraph(rnd, N, M, graphType, edges, graph);
        // double genTime = genTimer.delta();

        // cout << "Graph: " << graphType << " " << N << " " << edges.size()
        //  << " " << genTime.first << " " << genTime.second << endl;

        avgM += edges.size();

        vector<int> algoPerm(algos.size());
        iota(algoPerm.begin(), algoPerm.end(), 0);
        random_shuffle(algoPerm.begin(), algoPerm.end());
        for (int j : algoPerm) {
          RunResult result = run(N, M, 1, algos[j], graphType, edges, graph);
          totalTime[j] += result.time;
          taskCount++;
        }
      }

      cout << "M: " << (int)round(avgM / T) << endl;
      cout << "Progress: " << (int)(100 * (taskCount / (double)totalTasks))
           << "%" << endl;

      fout << N << "," << (int)round(avgM / T) << ",";

      for (double timeElapsed : totalTime) {
        fout << (timeElapsed * 1000 / T) << ",";
      }
      fout << endl;
    }
  }
}

template <typename F>
void measure(F &f) {
  Timer<> timer;
  timer.start();
  f();
  double time = timer.delta();
  std::cout << time << std::endl;
}

int main(int argc, char **argv) {
  Args args(argc, argv);

  int N = args.getInt("-n", 30000);
  int M = args.getInt("-m", 1000000);
  int T = args.getInt("-t", 10);
  string S = args.getString("-s", "filterkruskalnaive");
  string G = args.getString("-g", "randgraph");
  float skew = args.getFloat("--skew", 0.5f);
  isVerbose = args.getBool("-v");

  bool isBenchmark = args.getBool("--benchmark");

  cout << "hello" << endl;

  // Random rnd(31);
  // Edges es;
  // randomGeometricGraphSeq(rnd, 10, 20, 1.0, es);
  // for (Edge &e: es) {
  //   cout << e.a << " " << e.b << " " << e.w << endl;
  // }
  // return 0;

  // auto fn = [&](){ Random rnd(31); Edges es; randomGraph(rnd, N, M, 1.0, es);
  // }; measure(fn);

  // auto fn2 = [&](){ Random rnd(31); Edges es; randomGraphDense(rnd, N,
  // M, 1.0, es); }; measure(fn2);

  // auto fn4 = [&](){ Random rnd(31); Edges es; randomGeometricGraphSeq(rnd, N,
  // M, 1.0, es); }; measure(fn4);

  if (isBenchmark) {
    runBenchmark();
  } else {
    Random rnd(31);
    Timer<> genTimer;
    genTimer.start();
    Edges edges;
    Graph graph;
    genGraph(rnd, N, M, G, edges, graph, S != "prim");
    double genTime = genTimer.delta();
    RunResult out = run(N, M, T, S, G, edges, graph, skew);
    cout << S << " " << N << " " << edges.size() << " cost: " << out.cost
         << " , time: " << 1000 * out.time
         << ", gen+solve time: " << 1000 * (out.time + genTime) << endl;
  }

  return 0;
}