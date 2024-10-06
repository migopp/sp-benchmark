#pragma once

#include <cassert>
#include <cstdlib>
#include <iostream>

namespace impl {
// DOUBLE INDIRECTION APPROACH
//
// Stores a single `uintptr_t` within each RC, using less memory at the
// detriment of having multiple layers of indirection.
//
// This should make dereferencing performance significantly worse,
// as chained dependent loads need to be performed.
template <typename T> class Double {
private:
  struct IS {
    // Put this first for alignment
    T *ptr;

    // THREAD SAFETY: These are not thread safe, for the sake of the experiment,
    //                we won't use atomic reference counting.
    uint32_t count;

    IS() : count(1) { ptr = new T(); }
    template <typename... Args> IS(Args... args) : count(1) {
      ptr = new T(args...);
    }
  };

  IS *is;

  // Moves strong RC away from current resource, for destruction or so that
  // it can point to a new resource
  void point_away() {
    if (--(is->count) == 0) {
      delete is->ptr; // free resource
      delete is;      // nothing else will use this counter
    }
  }

  // This is strange to have as public...
  template <typename... Args> Double(Args... args) { is = new IS(args...); }

public:
  // Calls default constructor for T. 1, 2 are equivalent.
  //
  // 1:
  // ```
  // Double<Thing> my_thing{};
  // ```
  //
  // 2:
  // ```
  // Double<Thing> my_thing = Double<Thing>::make();
  // ```
  Double() { is = new IS(); }

  ~Double() { point_away(); }

  // ```
  // Double<Thing> my_thing = Double<Thing>::make(27); // -> count = 1
  // Double<Thing> copy{my_thing}; // -> count = 2
  // assert(my_thing.operator->() == copy.operator->());
  // ```
  Double(const Double<T> &src) {
    src.is->count++;
    is = src.is;
  }

  // ```
  // Double<Thing> my_thing = Double<Thing>::make(27); // -> count = 1
  // Double<Thing> copy = my_thing; // -> count = 2
  // assert(my_thing.operator->() == copy.operator->());
  // ```
  Double<T> &operator=(const Double<T> &rhs) {
    rhs.is->count++;
    point_away();
    is = rhs.is;

    return *this;
  }

  // ```
  // Double<Thing> my_thing = Double<Thing>::make(27);
  // Double<Thing> copy = my_thing;
  // assert(my_thing == copy);
  // ```
  bool operator==(const Double<T> &o) const { return is->ptr == o.is->ptr; }

  // ```
  // Double<int> my_int = Double<Thing>::make(27);
  // cout << *my_thing << "\n"; // -> 27
  // ```
  T operator*() {
    assert(is->ptr != nullptr);
    return *is->ptr;
  }

  // ```
  // Double<Thing> my_thing = Double<Thing>::make(27);
  // cout << my_thing->get_number() << "\n"; // -> 27
  // ```
  T *operator->() {
    assert(is->ptr != nullptr);
    return is->ptr;
  }

  // ```
  // Double<T> my_thing = Double<Thing>::make(27);
  // assert(my_thing.operator->() == nullptr);
  // ```
  template <typename... Args> static Double<T> make(Args... args) {
    return Double(args...);
  }
};
} // namespace impl
