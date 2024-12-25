#ifndef LOX_INSTANCE_H_
#define LOX_INSTANCE_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "lox_class.h"

namespace cclox {
class LoxInstance : public std::enable_shared_from_this<LoxInstance> {
 public:
  // Ensure that client code cannot directly call the constructor and can only
  // create instances of LoxInstance as std::shared_ptr to support the usage of
  // shared_from_this.
  static auto Create(const LoxClass& klass) -> LoxInstancePtr;

  auto GetField(const Token& field) -> Object;

  auto SetField(const Token& field, const Object& value) -> void;

  auto ToString() const -> std::string;

  using FieldMap = std::unordered_map<std::string, Object>;

 private:
  explicit LoxInstance(const LoxClass& klass) : klass_(klass) {}

  const LoxClass& klass_;
  FieldMap fields_;
};
}  // namespace cclox

#endif  // LOX_INSTANCE_H_
