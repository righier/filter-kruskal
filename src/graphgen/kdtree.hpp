#pragma once

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>

#include "../utils/base.hpp"
#include "pos.hpp"

class kdTree {
 public:
  kdTree(std::vector<Pos> &points)
      : kdTree(points.begin(), points.begin(), points.end(), 0) {}

  template <class Iter>
  kdTree(Iter realBegin, Iter begin, Iter end, bool dim, int depth = 0) {
    Iter mid = begin + (end - begin) / 2;
    Pos minp = *begin, maxp = *begin;
    for (Iter it = begin; it < end; it++) {
      minp = min(minp, *it);
      maxp = max(maxp, *it);
    }
    this->boxHalfSize = (maxp - minp) / 2.f;
    this->boxCenter = minp + this->boxHalfSize;
    this->dim = dim;

    std::nth_element(begin, mid, end, comp());
    this->pos = *mid;
    this->id = mid - realBegin;

    std::string indent = "";
    for (int i = 0; i < depth; i++) indent += "  ";

    if (mid - begin > 0) {
      this->left = new kdTree(realBegin, begin, mid, !dim, depth + 1);
    }
    if (end - mid > 1) {
      this->right = new kdTree(realBegin, mid + 1, end, !dim, depth + 1);
    }
  }

  ~kdTree() {
    if (left) delete left;
    if (right) delete right;
  }

  int getId() const { return id; }
  Pos getPos() const { return pos; }

  struct QueryResult {
    float d;
    int id;

    QueryResult(float d, int id) : d(d), id(id) {}

    bool operator<(const QueryResult &other) const { return d < other.d; }
  };

  QueryResult closest(const Pos &p, int id) {
    QueryResult best(dist2(this->pos, p), id);
    closest(p, id, best);
    return best;
  }

  void closestK(const Pos &p, int id, int k, std::vector<int> &results) {
    std::priority_queue<QueryResult> best;

    closestK(p, id, k, best);

    size_t nResults = best.size();
    results.resize(nResults);
    while (nResults--) {
      results[nResults] = best.top().id;
      best.pop();
    }
  }

  std::string getName() { return "p" + std::to_string(id); }

  void printDot() {
    std::cout << "digraph {" << std::endl;
    printDotNodes();
    printDotEdges();
    std::cout << "}" << std::endl;
  }

  void printDotNodes() {
    std::cout << getName() << " [pos = \"" << pos.x << "," << pos.y << "!\"];"
              << std::endl;
    if (left) left->printDotNodes();
    if (right) right->printDotNodes();
  }

  void printEdge(kdTree *other) {
    std::cout << getName() << "->" << other->getName() << ";" << std::endl;
  }

  void printDotEdges() {
    if (left) {
      printEdge(left);
      left->printDotEdges();
    }
    if (right) {
      printEdge(right);
      right->printDotEdges();
    }
  }

 private:
  kdTree *left = nullptr, *right = nullptr;
  Pos pos, boxCenter, boxHalfSize;
  int id;
  bool dim;

  std::function<bool(const Pos &, const Pos &)> comp() {
    return dim ? compY : compX;
  }
  static bool compX(const Pos &a, const Pos &b) { return a.x < b.x; }
  static bool compY(const Pos &a, const Pos &b) { return a.y < b.y; }

  bool comp2(const Pos &a, const Pos &b) {
    if (dim)
      return compY(a, b);
    else
      return compX(a, b);
  }

  float minDist(const Pos &p) {
    return len2(max(Pos(0, 0), abs(p - boxCenter) - boxHalfSize));
  }

  void closest(const Pos &p, int id, QueryResult &best) {
    if (minDist(p) < best.d) {
      float d = dist(p, pos);
      if (this->id != id && d < best.d) {
        best.d = d;
        best.id = this->id;
      }

      if (comp2(p, pos)) {
        if (left) left->closest(p, id, best);
        if (right) right->closest(p, id, best);
      } else {
        if (right) right->closest(p, id, best);
        if (left) left->closest(p, id, best);
      }
    }
  }

  void closestK(const Pos &p, int id, int k,
                std::priority_queue<QueryResult> &best) {
    float worstDist = std::numeric_limits<float>::infinity();
    if (!best.empty()) {
      worstDist = best.top().d;
    }
    if (best.size() < (size_t)k || minDist(p) < worstDist) {
      float d = dist2(p, pos);
      if (this->id != id && (best.size() < (size_t)k || d < worstDist)) {
        if (best.size() == (size_t)k) best.pop();
        best.emplace(QueryResult(d, this->id));
      }

      if (comp2(p, pos)) {
        if (left) left->closestK(p, id, k, best);
        if (right) right->closestK(p, id, k, best);
      } else {
        if (right) right->closestK(p, id, k, best);
        if (left) left->closestK(p, id, k, best);
      }
    }
  }
};