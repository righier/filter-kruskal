#pragma once

#include <cmath>
#include <iostream>

struct Pos {
  float x, y;
  Pos() : Pos(0, 0) {}
  Pos(float x, float y) : x(x), y(y) {}
};

static Pos operator*(const Pos &a, const float s) {
  return Pos(a.x * s, a.y * s);
}
static Pos operator/(const Pos &a, const float s) { return a * (1.f / s); }
static Pos operator+(const Pos &a, const Pos &b) {
  return Pos(a.x + b.x, a.y + b.y);
}
static Pos operator-(const Pos &a, const Pos &b) {
  return Pos(a.x - b.x, a.y - b.y);
}

static float dot(const Pos &a, const Pos &b) { return a.x * b.x + a.y * b.y; }
static float len2(const Pos &a) { return dot(a, a); }
static float len(const Pos &a) { return std::sqrt(len2(a)); }

static float dist2(const Pos &a, const Pos &b) { return len2(a - b); }
static float dist(const Pos &a, const Pos &b) { return len(a - b); }

static Pos min(const Pos &a, const Pos &b) {
  return Pos(std::min(a.x, b.x), std::min(a.y, b.y));
}
static Pos max(const Pos &a, const Pos &b) {
  return Pos(std::max(a.x, b.x), std::max(a.y, b.y));
}
static Pos abs(const Pos &a) { return Pos(std::abs(a.x), std::abs(a.y)); }

static std::ostream &operator<<(std::ostream &os, const Pos &p) {
  return os << "(" << p.x << ", " << p.y << ")";
}