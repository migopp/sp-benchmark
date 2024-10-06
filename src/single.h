#pragma once

#include <cassert>
#include <cstdlib>
#include <iostream>

namespace impl {

// SINGLE INDIRECTION APPROACH
//
// Stores 2 `uintptr_t` within each strong RC, using more space for the
// benefit of only a single layer of indirection.
//
// Dereferencing this should be comparable in performance to manually
// dereferencing a raw ptr.
template <typename T> class Single {
private:
  // THREAD SAFETY: These are not thread safe, for the sake of the experiment,
  //                we won't use atomic reference counting.
  uint32_t *count;
  T *ptr;

  // Moves strong RC away from current resource, for destruction or so that
  // it can point to a new resource
  void point_away() {
    if (--(*count) == 0) {
      delete ptr;   // free resource
      delete count; // nothing else will use this counter
    }
  }

  // This is strange to have as public...
  template <typename... Args> Single(Args... args) {
    count = new uint32_t(1);
    ptr = new T(args...);
  }

public:
  // Calls default constructor for T. 1, 2 are equivalent.
  //
  // 1:
  // ```
  // Single<Thing> my_thing{};
  // ```
  //
  // 2:
  // ```
  // Single<Thing> my_thing = Single<Thing>::make();
  // ```
  Single() {
    count = new uint32_t(1);
    ptr = new T();
  }

  ~Single() { point_away(); }

  // ```
  // Single<Thing> my_thing = Single<Thing>::make(27); // -> count = 1
  // Single<Thing> copy{my_thing}; // -> count = 2
  // assert(my_thing.operator->() == copy.operator->());
  // ```
  Single(const Single<T> &src) {
    assert(src.ptr != nullptr);
    (*src.count)++;
    count = src.count;
    ptr = src.ptr;
  }

  // ```
  // Single<Thing> my_thing = Single<Thing>::make(27); // -> count = 1
  // Single<Thing> copy = my_thing; // -> count = 2
  // assert(my_thing.operator->() == copy.operator->());
  // ```
  Single<T> &operator=(const Single<T> &rhs) {
    assert(rhs.ptr != nullptr);
    (*rhs.count)++;
    point_away();
    count = rhs.count;
    ptr = rhs.ptr;

    return *this;
  }

  // ```
  // Single<Thing> my_thing = Single<Thing>::make(27);
  // Single<Thing> copy = my_thing;
  // assert(my_thing == copy);
  // ```
  bool operator==(const Single<T> &o) const { return ptr == o.ptr; }

  // ```
  // Single<int> my_int = Single<Thing>::make(27);
  // cout << *my_thing << "\n"; // -> 27
  // ```
  T operator*() {
    assert(ptr != nullptr);
    return *ptr;
  }

  // ```
  // Single<Thing> my_thing = Single<Thing>::make(27);
  // cout << my_thing->get_number() << "\n"; // -> 27
  // ```
  T *operator->() {
    assert(ptr != nullptr);
    return ptr;
  }

  // ```
  // Single<T> my_thing = Single<Thing>::make(27);
  // assert(my_thing.operator->() != nullptr)
  // ```
  template <typename... Args> static Single<T> make(Args... args) {
    return Single(args...);
  }
};
} // namespace impl
