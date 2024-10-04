#include "double.h"
#include "single.h"
#include <iostream>

using namespace std;

extern void single_simple_test();

int main() {
  // init
  cout << "Benchmarking..." << endl;

  // test
  single_simple_test();

  // done
  return 0;
}
