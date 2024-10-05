#pragma once

#include <cstdlib>
#include <iostream>

namespace impl {
// WRAPPED APPROACH
//
// MORE DESC HERE :)
template <typename T> class Wrapped {
private:
  struct HA {
    // THREAD SAFETY: These are not thread safe, for the sake of the experiment,
    //                we won't use atomic reference counting.
    uint32_t count;
    T thing;

    // Since this is internal, it shouldn't be called.
    // But we add the fail condition just in case.
    HA() {
      std::cerr << "HA DEFAULT CONSTRUCTOR CALLED" << std::endl;
      exit(1);
    }

    template <typename... Args>
    HA(uint32_t c, Args... args) : count(c), thing(args...) {}
  };

  HA *ha;
  T *ptr;

  // Moves strong RC away from current resource, for destruction or so that
  // it can point to a new resource
  void point_away() {
    if (--(ha->count) == 0)
      delete ha; // nothing will use this counter
  }

  // This is strange to have as public...
  template <typename... Args> Wrapped(Args... args) {
    ha = new HA(1, args...);
    ptr = &ha->thing; // for fast access
  }

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
    ptr = src.ptr;
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
    ptr = rhs.ptr;

    return *this;
  }

  // ```
  // Wrapped<int> my_int = Wrapped<Thing>::make(27);
  // cout << *my_thing << "\n"; -> 27
  // ```
  T operator*() { return *ptr; }

  // ```
  // Wrapped<Thing> my_thing = Wrapped<Thing>::make(27);
  // cout << my_thing->get_number() << "\n"; -> 27
  // ```
  T *operator->() { return ptr; }

  // ```
  // Wrapped<T> my_thing = Wrapped<Thing>::make(27);
  // assert(my_thing.operator->() == nullptr) -> true
  // ```
  template <typename... Args> static Wrapped<T> make(Args... args) {
    return Wrapped(args...);
  }
};
} // namespace impl
