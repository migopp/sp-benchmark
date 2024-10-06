#pragma once

#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>

using namespace std;

namespace impl {
class Benchmark {
private:
  std::ofstream res;

public:
  Benchmark(string f_name) : res(f_name, std::ios::app) {
    if (!res) {
      cerr << "FAILED TO OPEN FILE " << f_name << endl;
      exit(1);
    }
  }
  ~Benchmark() { res.close(); }

  template <typename T> void test(const T &f, std::ofstream *log) {
    // time exec
    auto start = chrono::high_resolution_clock::now();
    f(log);
    auto stop = chrono::high_resolution_clock::now();

    // get duration
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

    // doc
    res << duration.count() << "\n";
  }
};
} // namespace impl
