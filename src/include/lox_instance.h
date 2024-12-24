#ifndef LOX_INSTANCE_H_
#define LOX_INSTANCE_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "lox_class.h"

namespace cclox {
class LoxInstance : public std::enable_shared_from_this<LoxInstance> {
 public:
  using FieldMap = std::unordered_map<std::string, Object>;

  explicit LoxInstance(const LoxClass& klass) : klass_(klass) {}

  auto GetField(const Token& field) -> Object;

  auto SetField(const Token& field, const Object& value) -> void;

  auto ToString() const -> std::string;

 private:
  const LoxClass& klass_;
  FieldMap fields_;
};
}  // namespace cclox

#endif  // LOX_INSTANCE_H_
