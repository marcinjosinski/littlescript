#ifndef SCRIPT_UTILS_HPP
#define SCRIPT_UTILS_HPP

#include <cstdlib>

using byte = unsigned char;

template <typename... F> class overload : private F... {
public:
  overload(F... f) : F(f)... {}
  using F::operator()...;
};

struct error {
  explicit error(const char *m) : func{m} {};
  const char *func;
};

template <typename T> class size {
public:
  explicit size(T value) : value_{value} {}
  T get() const { return value_; }

private:
  T value_;
};

using space_size = size<std::size_t>;

#endif