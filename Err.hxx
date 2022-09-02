#ifndef RESULT_VARIANT_ERR
#define RESULT_VARIANT_ERR

namespace lib {

class Error {
protected:
  std::string _message;

public:
  virtual ~Error() {}
};

namespace details {

template <typename E> struct ErrImpl {
  E _err;

  ErrImpl(const E &err) : _err(err) {}
  ErrImpl(E &&err) : _err(std::move(err)) {}

  template <class T>
  friend std::ostream &operator<<(std::ostream &, const ErrImpl<T> &);
};
template <typename E>
std::ostream &operator<<(std::ostream &os, const ErrImpl<E> &err) {
  os << err._err;
  return os;
}

template <> struct ErrImpl<void> {
  std::string _message;
  ErrImpl(const std::string &message) : _message(message) {}
  ErrImpl(std::string &&message) : _message(std::move(message)) {}

  template <class E>
  friend std::ostream &operator<<(std::ostream &, const ErrImpl<E> &);
};

template <>
std::ostream &operator<<(std::ostream &os, const ErrImpl<void> &err) {
  os << err._message;
  return os;
}

// Err implementation type
template <std::size_t, typename... E> struct ErrImplType {
  using type = ErrImpl<E...>;
};
template <> struct ErrImplType<0> { using type = ErrImpl<void>; };

} // namespace details

template <typename... Es> class Err {
  static_assert(
      sizeof...(Es) <= 1,
      "Requires zero or one argument. Multiple error types are not supported.");

private:
  static constexpr std::size_t size = sizeof...(Es);
  typename details::ErrImplType<size, Es...>::type _impl;

public:
  Err(const std::string &message) requires(size == 0) : _impl(message) {}
  Err(std::string &&message) requires(size == 0) : _impl(std::move(message)) {}

  template <typename V> Err(V &&value) requires(size == 1)
   : _impl(std::forward<V>(value)) {}

  template <typename... Ts>
  friend std::ostream &operator<<(std::ostream &, const Err<Ts...> &);
};

template <typename... Es>
std::ostream &operator<<(std::ostream &os, const Err<Es...> &err) {
  os << err._impl;
  return os;
}

template <class> struct isErr : std::false_type {};
template <class T> struct isErr<Err<T>> : std::true_type {};
} // namespace lib

#endif
