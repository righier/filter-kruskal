#pragma once

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>

#include "utils.h"

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

    // std::cout << indent << "box: " << minp << "--" << maxp << "  node: " <<
    // pos << std::endl;

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
    kdTree *inst;

    QueryResult(float d, kdTree *inst) : d(d), inst(inst) {}

    bool operator<(const QueryResult &other) const { return d < other.d; }
  };

  kdTree &closest(const Pos &p, int id) {
    QueryResult best(std::numeric_limits<float>::infinity(), this);
    closest(p, id, best);
    return *best.inst;
  }

  std::vector<QueryResult> closestK(const Pos &p, int id, int k) {
    std::priority_queue<QueryResult> best;
    best.emplace(QueryResult(std::numeric_limits<float>::infinity(), this));

    // std::cout << "closest to: " << id << " " << p << " K: " << k <<
    // std::endl;

    closestK(p, id, k, best);

    std::vector<QueryResult> results;
    results.reserve(k);
    while (!best.empty()) {
      results.push_back(best.top());
      best.pop();
    }
    return results;
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

  float minDist(const Pos &p) {
    return len2(max(Pos(0, 0), abs(p - boxCenter) - boxHalfSize));
  }

  void closest(const Pos &p, int id, QueryResult &best) {
    if (minDist(p) < best.d) {
      float d = dist(p, pos);
      if (this->id != id && d < best.d) {
        best.d = d;
        best.inst = this;
      }

      if (comp()(p, pos)) {
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
    QueryResult worst = best.top();
    if (best.size() < k || minDist(p) < worst.d) {
      float d = dist2(p, pos);
      if (this->id != id && (best.size() < k || d < worst.d)) {
        if (best.size() == k) best.pop();
        best.emplace(QueryResult(d, this));
      }

      if (comp()(p, pos)) {
        if (left) left->closestK(p, id, k, best);
        if (right) right->closestK(p, id, k, best);
      } else {
        if (right) right->closestK(p, id, k, best);
        if (left) left->closestK(p, id, k, best);
      }
    }
  }
};