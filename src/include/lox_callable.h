#ifndef LOX_CALLABLE_H_
#define LOX_CALLABLE_H_

#include <string>
#include <vector>

#include "interpreter.h"
#include "object.h"

namespace cclox {
class LoxCallable {
 public:
  virtual ~LoxCallable() = default;

  virtual auto Arity() const noexcept -> size_t = 0;

  virtual auto Call(Interpreter& interpreter,
                    const std::vector<Object>& arguments) -> Object = 0;

  virtual auto ToString() const -> std::string = 0;
};
}  // namespace cclox

#endif  // LOX_CALLABLE_H_
