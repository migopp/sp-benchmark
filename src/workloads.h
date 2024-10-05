#include "double.h"
#include "single.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace impl;

template <template <typename> class P>
int simple_destruction(std::ofstream *log) {
  struct Thing {
    int thingy;
    ofstream *log;

    Thing(int t, ofstream *l) : thingy(t), log(l) {
      *log << "Thing::Thing()" << std::endl;
    }
    ~Thing() { *log << "Thing::~Thing()" << std::endl; }
  };

  for (int i = 0; i < 10000; ++i) {
    // make a thing
    P<Thing> thing_one = new Thing(27, log);
    {
      P<Thing> thing_two = thing_one; // copy it
      thing_one = nullptr;            // destruct original
    } // Thing::~Thing()
  }

  return 27;
};
