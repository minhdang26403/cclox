#ifndef LOX_FUNCTION_H_
#define LOX_FUNCTION_H_

#include <memory>
#include <string>
#include <vector>

#include "environment.h"
#include "lox_callable.h"
#include "object.h"
#include "stmt.h"

namespace cclox {
class LoxFunction : public LoxCallable {
 public:
  LoxFunction(const FunctionStmtPtr& declaration,
              std::shared_ptr<Environment> closure, bool is_initializer)
      : declaration_(declaration),
        closure_(std::move(closure)),
        is_initializer_(is_initializer) {}

  auto Arity() const noexcept -> size_t override;

  auto Call(Interpreter& interpreter, const std::vector<Object>& arguments)
      -> Object override;

  auto ToString() const -> std::string override;

  auto Bind(const LoxInstance& instance) const -> LoxCallablePtr;

 private:
  const FunctionStmtPtr& declaration_;
  std::shared_ptr<Environment> closure_;
  bool is_initializer_{false};
};

using LoxFunctionPtr = std::shared_ptr<LoxFunction>;
}  // namespace cclox

#endif  // LOX_FUNCTION_H_
