#ifndef RESULT_VARIANT_OK
#define RESULT_VARIANT_OK

#include <istream>

namespace lib {

namespace details {
template <typename T> struct OkImpl {
  T _value;
  OkImpl(const T &value) : _value(value) {}
  OkImpl(T &&value) : _value(std::move(value)) {}
  template <typename V>
  friend std::ostream &operator<<(std::ostream &, const OkImpl<V> &);
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const OkImpl<T> &ok) {
  os << ok._value;
  return os;
}
template <> struct OkImpl<void> {};

// Ok implementation type
template <std::size_t, typename... Ts> struct OkImplType {
  using type = OkImpl<Ts...>;
};
template <> struct OkImplType<0> { using type = OkImpl<void>; };
} // namespace details

// Ok type
template <typename... Ts> class Ok {
  static_assert(sizeof...(Ts) <= 1,
                "Templated class Ok must have zero or one argument.");

private:
  static constexpr std::size_t size = sizeof...(Ts);
  typename details::OkImplType<size, Ts...>::type _impl;

public:
  Ok() requires(size == 0) {}

  template <typename V>
  Ok(V &&value) requires(size == 1) : _impl(std::forward<V>(value)) {}

  template <typename R>
  friend std::ostream &operator<<(std::ostream &os, const Ok<R> &ok);
};

template <class T> std::ostream &operator<<(std::ostream &os, const Ok<T> &ok) {
  os << ok._impl;
  return os;
}

template <class> struct isOk : std::false_type {};
template <class T> struct isOk<Ok<T>> : std::true_type {};
} // namespace lib

#endif
