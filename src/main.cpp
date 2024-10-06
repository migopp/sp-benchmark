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

  // copy destruction
  std::cout << "\n~~~\nStarting copy destruction tests..." << std::endl;
  sb.test(copy_destruction<Single>, sl);
  std::cout << "\tFinished single\n";
  db.test(copy_destruction<Double>, dl);
  std::cout << "\tFinished double\n";
  wb.test(copy_destruction<Wrapped>, wl);
  std::cout << "\tFinished wrapped\n";
  std::cout << "Finished copy destruction tests\n~~~\n" << std::endl;

  // pointer dereferences
  std::cout << "\n~~~\nStarting basic access tests..." << std::endl;
  sb.test(basic_access<Single>, sl);
  std::cout << "\tFinished single\n";
  db.test(basic_access<Double>, dl);
  std::cout << "\tFinished double\n";
  wb.test(basic_access<Wrapped>, wl);
  std::cout << "\tFinished wrapped\n";
  std::cout << "Finished basic access tests\n~~~\n" << std::endl;

  // done
  return 0;
}
