#ifndef RESULT_VARIANT_RESULT
#define RESULT_VARIANT_RESULT

#include <concepts>
#include <iostream>
#include <string>
#include <type_traits>
#include <variant>

#include "Err.hxx"
#include "Ok.hxx"

namespace lib {
namespace details {
using ::lib::Err;
using ::lib::isErr;
using ::lib::Ok;

// Base type of Result
template <class...> struct ResultBase;

template <> struct ResultBase<> {
  using ok_type = Ok<>;
  using err_type = Err<>;
  using type = std::variant<Ok<>, Err<>>;
};

template <class T> struct ResultBase<T> {
  using ok_type = Ok<T>;
  using err_type = Err<>;
  using type = std::variant<Ok<T>, Err<>>;
};

template <class E>
requires(std::is_base_of_v<Error, E>) struct ResultBase<E> {
  using ok_type = Ok<>;
  using err_type = Err<E>;
  using type = std::variant<Ok<>, Err<E>>;
};

template <class T, class E> struct ResultBase<T, E> {
  using ok_type = Ok<T>;
  using err_type = Err<E>;
  using type = std::variant<Ok<T>, Err<E>>;
};

} // namespace details

/// \class Result
/// Result type that holds a return type and an error type
// - if no argument is provided, the base type is std::variant<Ok<>, Err<>>
// - if one argument of type E derived from Error is provided, the base type is
//    std::variant<Ok<>, Err<E>>
// - if one argument of type T is provided, the base type is 
//    std::variant<Ok<T>, Err<>>
// - if two arguments T and E are provided, the base type is 
//    std::variant<Ok<T>, Err<E>>
template <typename... Ts>
class Result : public details::ResultBase<Ts...>::type {
  static_assert(sizeof...(Ts) <= 2, "Template parameters not supported.");

public:
  using base_type = typename details::ResultBase<Ts...>::type;
  using ok_type = typename details::ResultBase<Ts...>::ok_type;
  using err_type = typename details::ResultBase<Ts...>::err_type;

public:
  template <class V> Result(V &&value) : base_type(std::forward<V>(value)) {}

  template <class V>
  Result(V &&value) requires(std::is_base_of_v<Error, V>)
      : base_type(Err<V>(std::forward<V>(value))) {}

  // Returns whether the result is a value (ok)
  [[nodiscard]] bool ok() const { return this->index() == 0; }

  // Returns whether the result is an error
  [[nodiscard]] bool err() const { return this->index() == 1; }

  // Returns the value of type Ok<V>
  [[nodiscard]] ok_type get_ok() const { return std::get<ok_type>(*this); }

  // Returns the error
  [[nodiscard]] err_type get_err() const { return std::get<err_type>(*this); }

  template <class... Rs>
  friend std::ostream &operator<<(std::ostream & /*os*/,
                                  const Result<Rs...> & /*res*/);
};

template <class... Ts>
std::ostream &operator<<(std::ostream &os, const Result<Ts...> &res) {
  using result_type = Result<Ts...>;
  std::visit(
      [&os, res](auto &&r) {
        using type = std::decay_t<decltype(r)>;
        if constexpr (std::is_same_v<type, typename result_type::err_type>) {
          os << res.get_err();
        } else {
          os << res.get_ok();
        }
      },
      res);
  return os;
}
} // namespace lib

#endif
