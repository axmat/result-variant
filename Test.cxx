#include "Result.hxx"

using namespace lib;
Result<float> return_result(float x) {
  if (x > 0)
    return x;
  else
    return Err("Error, x must be positive.");
}

struct AllocError : public Error {
  std::string _message;
  AllocError(std::size_t bytes)
      : _message("Error allocating " + std::to_string(bytes) + " bytes.") {}
  friend std::ostream &operator<<(std::ostream &, const AllocError &);
};
std::ostream &operator<<(std::ostream &os, const AllocError &err) {
  os << err._message;
  return os;
}

Result<AllocError> custom_err(size_t x) {
  if (x > 64)
    return AllocError(x);
  else
    return Ok();
}

int main() {

  {
    // Optional error
    using result_type = Result<>;
    static_assert(
        std::is_same_v<result_type::base_type, std::variant<Ok<>, Err<>>>);
  }

  {
    // Optional return type with default error type
    using result_type = Result<float>;
    static_assert(
        std::is_same_v<result_type::base_type, std::variant<Ok<float>, Err<>>>);
  }

  {
    // Custom error type
    using result_type = Result<AllocError>;
    static_assert(std::is_same_v<result_type::ok_type, Ok<>>);
    static_assert(std::is_same_v<result_type::err_type, Err<AllocError>>);
  }

  {
    auto r1 = return_result(0.5);
    static_assert(std::is_same_v<decltype(r1), Result<float>>);
    std::cout << std::boolalpha << r1.err() << std::endl; // false
    std::cout << std::boolalpha << r1.ok() << std::endl;  // true
    std::cout << r1.get_ok() << std::endl;                // 0.5

    auto r2 = return_result(-1.);
    std::cout << std::boolalpha << r2.err() << std::endl; // true
    std::cout << std::boolalpha << r2.ok() << std::endl;  // false
    std::cout << r2.get_err() << std::endl; // Error, x must be positive.
  }

  {
    auto r1 = custom_err(32);
    static_assert(std::is_same_v<decltype(r1), Result<AllocError>>);
    std::cout << std::boolalpha << r1.err() << std::endl; // false
    std::cout << std::boolalpha << r1.ok() << std::endl;  // true

    auto r2 = custom_err(1024);
    std::cout << std::boolalpha << r2.err() << std::endl; // true
    std::cout << std::boolalpha << r2.ok() << std::endl;  // false
    std::cout << r2.get_err() << std::endl; // Error allocating 1024 bytes.
  }
}
