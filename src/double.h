/*
    Double Indirection

    Attempts to save space by halving the size
    of each strong pointer via structure to house
    all pertinent data.

    Stores:
        - ptr
        - reference counts

    Makes strong pointers more space efficient at
    the cost of a double indirection every deref.
 */

#pragma once

#include <atomic>

namespace impl {
// intermediate structure
template <typename T> struct IS {
  T *ptr;
  std::atomic<uint32_t> count;

  IS() : count(0) { ptr = nullptr; }
  IS(T *p) : count(p == nullptr ? 0 : 1) { ptr = p; }
};

template <typename T> class Double {
private:
  IS<T> *is;

  void point_away() {
    if (is->count.fetch_sub(1) == 1) { // = 0
      if (is->ptr != nullptr)
        delete is->ptr;
      delete is; // last one
    }
  }

public:
  Double() { is = new IS<T>(); }

  ~Double() { point_away(); }

  Double(const Double<T> &src) {
    src.is->count.fetch_add(1);
    is = src.is;
  }

  Double(T *p) { is = new IS<T>(p); }

  Double<T> &operator=(const Double<T> &rhs) {
    rhs.is->count.fetch_add(1);
    point_away();
    is = rhs.is();
  }

  Double<T> &operator=(T *p) {
    point_away();
    is = new IS<T>(p);

    return *this;
  }

  T *operator->() { return is->ptr; }
};
} // namespace impl
