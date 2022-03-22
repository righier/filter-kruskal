#pragma once

#include <cassert>
#include <iostream>
#include <vector>

typedef uint64_t u64;
typedef int64_t i64;
typedef uint32_t u32;
typedef uint32_t i32;
typedef uint8_t u8;

#define UNUSED(x) (void)(x)

#define unreachable() \
  {                   \
    assert(0);        \
    abort();          \
  }

bool verbose();

template <typename T>
std::ostream& operator<<(std::ostream& out, std::vector<T> const& v) {
  for (auto const& x : v) {
    out << x << " ";
  }
  return out;
}
