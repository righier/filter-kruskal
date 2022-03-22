#pragma once

#include <chrono>

template <typename Clock = std::chrono::high_resolution_clock>
class Timer {
 private:
  std::chrono::time_point<Clock> startTime;
  double totalTime = 0.0;
  size_t count = 0;

 public:
  void reset() {
    totalTime = 0.0;
    count = 0;
  }

  void start() { startTime = Clock::now(); }

  double delta() {
    auto now = Clock::now();
    std::chrono::duration<double> dt = now - startTime;

    double deltaTime = dt.count();
    totalTime += deltaTime;
    count += 1;

    return deltaTime;
  }

  double total() { return totalTime; }

  double avg() { return totalTime / count; }
};
