#ifndef NATIVE_FUNCTION_
#define NATIVE_FUNCTION_

#include <chrono>
#include <string>
#include <vector>

#include "lox_callable.h"
#include "object.h"

namespace cclox {
class NativeClockFunction : public LoxCallable {
 public:
  auto Arity() const noexcept -> size_t { return 0; }

  auto Call(Interpreter&, const std::vector<Object>&) -> Object {
    using namespace std::chrono;
    auto now =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch())
            .count();
    return Object{static_cast<double>(now) / 1000.0};
  }

  auto ToString() const -> std::string { return "<native fn>"; }
};
}  // namespace cclox

#endif  // NATIVE_FUNCTION_H_
