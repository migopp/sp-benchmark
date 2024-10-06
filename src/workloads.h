#include "double.h"
#include "single.h"
#include <cstdint>
#include <fstream>
#include <iostream>

using namespace std;
using namespace impl;

// COPY DESTRUCTION
//
// Not the most useful workload, but gives a sense of how expensive
// copies are for a given approach.
template <template <typename> class P>
void copy_destruction(std::ofstream *log) {
  constexpr uint64_t NUM_DELETIONS = 2ULL << 18;

  struct Thing {
    int thingy;
    ofstream *log;

    Thing(int t, ofstream *l) : thingy(t), log(l) {
      *log << "Thing::Thing()\n";
    }
    ~Thing() { *log << "Thing::~Thing()\n"; }
  };

  for (uint64_t i = 0; i < NUM_DELETIONS; ++i) {
    // make a thing
    P<Thing> thing_one = P<Thing>::make(27, log);
    {
      P<Thing> thing_two = thing_one; // copy it
    }
  } // Thing::~Thing()
};

// BASIC ACCESS
//
// Also not a realistic workload, but will give us a feel for how
// expensive these are in aggregate. The compiler can probably do
// things to make the performance here better than average.
template <template <typename> class P> void basic_access(std::ofstream *log) {
  constexpr uint64_t POINTER_DEREFS = 2ULL << 24;

  struct Counter {
    uint64_t count;

    Counter() : count(0) {}

    void add() { count++; }
  };

  // make ptr
  P<Counter> c_ptr = P<Counter>::make();

  // deref and add
  for (uint64_t i = 0; i < POINTER_DEREFS; ++i) {
    c_ptr->add();
    *log << c_ptr->count << "\n";
  }

  // make sure we did it right
  assert(c_ptr->count == POINTER_DEREFS);
}
