#ifndef LOX_INSTANCE_H_
#define LOX_INSTANCE_H_

#include <string>
#include <unordered_map>

#include "lox_class.h"

namespace cclox {
class LoxInstance {
 public:
  using FieldMap = std::unordered_map<std::string, Object>;

  LoxInstance(const LoxClass& klass) : klass_(klass) {}

  auto GetField(const Token& field) -> Object;

  auto SetField(const Token& field, const Object& value) -> void;

  auto ToString() const -> std::string;

 private:
  const LoxClass& klass_;
  FieldMap fields_;
};
}  // namespace cclox

#endif  // LOX_INSTANCE_H_
