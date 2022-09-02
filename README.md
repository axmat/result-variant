## Result type in C++

Result type implemented by inheriting from std::variant. It support only basic operations and it should be used only for learning purpose.

If you want to use a result type in your c++11, 14 or 17 projects, an implementation of std::expected is avaible at [https://github.com/TartanLlama/expected](https://github.com/TartanLlama/expected).

## Usage example

```
#include "Result.hxx"
using namespace lib;

Result<> test(float x) {
  if (x>0)
    return Ok();
  else
    return Err("Input must be positive.");
}

int main() {
  auto res = test(-2.);
  if (res.err()) {
    std::cout << res.get_err() << std::endl;
  }
}
```

