#include "single.h"
#include <iostream>

using namespace impl;

struct Thing {
  int thingy;

  Thing(int t) : thingy(t) { std::cout << "Thing::Thing()" << std::endl; }
  ~Thing() { std::cout << "Thing::~Thing()" << std::endl; }
};

void single_simple_test() {
  Single<Thing> thing_one = new Thing(27);
  {
    Single<Thing> thing_two = thing_one;
    thing_one = nullptr;
  }
}
