#include "benchmark.h"
#include "double.h"
#include "single.h"
#include "workloads.h"
#include "wrapped.h"
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>

using namespace impl;

int main() {
  /* INIT */

  // get time
  const auto now = std::chrono::system_clock::now();
  const auto now_time = std::chrono::system_clock::to_time_t(now);

  // create logs
  std::ofstream *sl = new std::ofstream(
      std::format("logs/{}_single.log", now_time), std::ios::app);
  std::ofstream *dl = new std::ofstream(
      std::format("logs/{}_double.log", now_time), std::ios::app);
  std::ofstream *wl = new std::ofstream(
      std::format("logs/{}_wrapped.log", now_time), std::ios::app);

  // create benchmarks
  Benchmark sb{std::format("res/{}_single.txt", now_time)};
  Benchmark db{std::format("res/{}_double.txt", now_time)};
  Benchmark wb{std::format("res/{}_wrapped.txt", now_time)};

  // signal
  std::cout << "Benchmarking..." << std::endl;

  /* BENCHMARK */
  sb.test(copy_destruction<Single>, sl, 27);
  db.test(copy_destruction<Double>, dl, 27);
  wb.test(copy_destruction<Wrapped>, wl, 27);

  // done
  return 0;
}
