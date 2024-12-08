#ifndef LOX_FUNCTION_H_
#define LOX_FUNCTION_H_

#include <vector>
#include <string>
#include <memory>

#include "environment.h"
#include "lox_callable.h"
#include "stmt.h"

namespace cclox {
class LoxFunction : public LoxCallable {
 public:
  LoxFunction(const FunctionStmtPtr& declaration,
              const std::shared_ptr<Environment>& closure)
      : declaration_(declaration), closure_(closure) {}

  auto Arity() const noexcept -> size_t override;

  auto Call(Interpreter& interpreter, const std::vector<Object>& arguments)
      -> Object override;

  auto ToString() const -> std::string override;

 private:
  const FunctionStmtPtr& declaration_;
  std::shared_ptr<Environment> closure_;
};
}  // namespace cclox

#endif  // LOX_FUNCTION_H_
