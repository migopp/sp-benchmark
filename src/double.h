#pragma once

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
    // THREAD SAFETY: These are not thread safe, for the sake of the experiment,
    //                we won't use atomic reference counting.
    uint32_t count;
    T *ptr;

    IS() : count(0) { ptr = nullptr; }
    IS(T *p) : count(p == nullptr ? 0 : 1) { ptr = p; }
  };

  IS *is;

  // Moves strong RC away from current resource, for destruction or so that
  // it can point to a new resource
  void point_away() {
    if (--(is->count) == 0) {
      if (is->ptr != nullptr)
        delete is->ptr; // free resource if we need to
      delete is;        // nothing else will use this counter
    }
  }

  // This is strange to have as public...
  template <typename... Args> Double(Args... args) {
    is = new IS();
    is->count = 1;
    is->ptr = new T(args...);
  }

public:
  // DO NOT CALL THIS
  Double() {
    std::cerr << "DOUBLE DEFAULT CONSTRUCTOR CALLED" << std::endl;
    exit(1);
  }

  ~Double() { point_away(); }

  // ```
  // Double<Thing> my_thing = Double<Thing>::make(27); -> count = 1
  // Double<Thing> copy{my_thing}; -> count = 2
  // assert(my_thing.operator->() == copy.operator->()) -> true
  // ```
  Double(const Double<T> &src) {
    src.is->count++;
    is = src.is;
  }

  // ```
  // Double<Thing> my_thing = Double<Thing>::make(27); -> count = 1
  // Double<Thing> copy = my_thing; -> count = 2
  // assert(my_thing.operator->() == copy.operator->()) -> true
  // ```
  Double<T> &operator=(const Double<T> &rhs) {
    rhs.is->count++;
    point_away();
    is = rhs.is();
  }

  // ```
  // Double<int> my_int = Double<Thing>::make(27);
  // cout << *my_thing << "\n"; -> 27
  // ```
  T operator*() { return *is->ptr; }

  // ```
  // Double<Thing> my_thing = Double<Thing>::make(27);
  // cout << my_thing->get_number() << "\n"; -> 27
  // ```
  T *operator->() { return is->ptr; }

  // ```
  // Double<T> my_thing = Double<Thing>::make(27);
  // assert(my_thing.operator->() == nullptr) -> true
  // ```
  template <typename... Args> static Double<T> make(Args... args) {
    return Double(args...);
  }
};
} // namespace impl
