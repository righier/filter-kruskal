#pragma once

#include <stdlib.h>

#include <cassert>
#include <iostream>
#include <vector>

#include "pos.h"

using namespace std;

typedef uint64_t u64;
typedef int64_t i64;
typedef uint32_t u32;
typedef uint8_t u8;

#define UNUSED(x) (void)(x)

#define unreachable() \
  {                   \
    assert(0);        \
    abort();          \
  }

bool verbose();

template <typename T>
ostream& operator<<(ostream& out, std::vector<T> const& v) {
  for (auto const& x : v) {
    out << x << " ";
  }
  return out;
}
