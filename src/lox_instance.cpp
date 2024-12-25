#include "lox_instance.h"

#include "interpreter.h"
#include "lox_function.h"
#include "object.h"

namespace cclox {
auto LoxInstance::Create(const LoxClass& klass) -> LoxInstancePtr {
  return std::shared_ptr<LoxInstance>(new LoxInstance(klass));
}

auto LoxInstance::GetField(const Token& field) -> Object {
  const std::string& field_name = field.GetLexeme();
  if (fields_.contains(field_name)) {
    return fields_.at(field_name);
  }

  LoxCallablePtr method = klass_.FindMethod(field_name);
  if (method) {
    LoxCallablePtr new_method =
        static_pointer_cast<LoxFunction>(method)->Bind(shared_from_this());
    return Object{std::move(new_method)};
  }

  throw RuntimeError(field,
                     std::format("Undefined property '{}'.", field_name));
}

auto LoxInstance::SetField(const Token& field, const Object& value) -> void {
  fields_[field.GetLexeme()] = value;
}

auto LoxInstance::ToString() const -> std::string {
  return std::format("{} instance", klass_.ToString());
}
}  // namespace cclox
