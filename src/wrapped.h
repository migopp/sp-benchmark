#pragma once

#include <cstdlib>
#include <iostream>

namespace impl {
// WRAPPED SINGLE INDIRECTION APPROACH
//
// An attempt to reap the memory benefit of the double indirection
// approach with the speed benefit of the single indirection approach.
//
// This approach is only really worth it if only strong RCs are supported,
// because weak RCs will hold the resource space hostage, even if there are
// no strong references left.
template <typename T> class Wrapped {
private:
  struct HA {
    // Put this first for alignment
    T thing;

    // THREAD SAFETY: These are not thread safe, for the sake of the experiment,
    //                we won't use atomic reference counting.
    uint32_t count;

    // Since this is internal, it shouldn't be called.
    // But we add the fail condition just in case.
    HA() {
      std::cerr << "HA DEFAULT CONSTRUCTOR CALLED" << std::endl;
      exit(1);
    }

    template <typename... Args>
    HA(uint32_t c, Args... args) : thing(args...), count(c) {}
  };

  HA *ha;

  // Moves strong RC away from current resource, for destruction or so that
  // it can point to a new resource
  void point_away() {
    if (--(ha->count) == 0)
      delete ha; // nothing will use this counter
  }

  // This is strange to have as public...
  template <typename... Args> Wrapped(Args... args) { ha = new HA(1, args...); }

public:
  // DO NOT CALL THIS
  Wrapped() {
    std::cerr << "WRAPPED DEFAULT CONSTRUCTOR CALLED" << std::endl;
    exit(1);
  }

  ~Wrapped() { point_away(); }

  // ```
  // Wrapped<Thing> my_thing = Wrapped<Thing>::make(27); -> count = 1
  // Wrapped<Thing> copy{my_thing}; -> count = 2
  // assert(my_thing.operator->() == copy.operator->()) -> true
  // ```
  Wrapped(const Wrapped<T> &src) {
    src.ha->count++;
    ha = src.ha;
  }

  // ```
  // Wrapped<Thing> my_thing = Wrapped<Thing>::make(27); -> count = 1
  // Single<Thing> copy = my_thing; -> count = 2
  // assert(my_thing.operator->() == copy.operator->()) -> true
  // ```
  Wrapped<T> &operator=(const Wrapped<T> &rhs) {
    rhs.ha->count++;
    point_away();
    ha = rhs.ha;

    return *this;
  }

  // ```
  // Wrapped<int> my_int = Wrapped<Thing>::make(27);
  // cout << *my_thing << "\n"; -> 27
  // ```
  T operator*() { return *ha; }

  // ```
  // Wrapped<Thing> my_thing = Wrapped<Thing>::make(27);
  // cout << my_thing->get_number() << "\n"; -> 27
  // ```
  T *operator->() { return ha; }

  // ```
  // Wrapped<T> my_thing = Wrapped<Thing>::make(27);
  // assert(my_thing.operator->() == nullptr) -> true
  // ```
  template <typename... Args> static Wrapped<T> make(Args... args) {
    return Wrapped(args...);
  }
};
} // namespace impl
