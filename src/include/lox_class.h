#ifndef LOX_CLASS_H_
#define LOX_CLASS_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "lox_callable.h"
#include "object.h"

namespace cclox {
class LoxClass : public LoxCallable {
 public:
  using MethodMap = std::unordered_map<std::string, LoxCallablePtr>;

  LoxClass(std::string name, std::optional<Object> superclass,
           MethodMap methods)
      : name_(std::move(name)),
        superclass_(std::move(superclass)),
        methods_(std::move(methods)) {}

  auto FindMethod(const std::string& name) const -> LoxCallablePtr;

  auto Arity() const noexcept -> size_t override;

  auto Call(Interpreter& interpreter, const std::vector<Object>& arguments)
      -> Object override;

  auto ToString() const -> std::string override;

 private:
  std::string name_;
  std::optional<Object> superclass_;
  MethodMap methods_;
};

using LoxClassPtr = std::shared_ptr<LoxClass>;
}  // namespace cclox

#endif  // LOX_CLASS_H_
