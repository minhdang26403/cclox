#include "environment.h"
#include "interpreter.h"

namespace cclox {
auto Environment::Create() -> std::shared_ptr<Environment> {
  return std::shared_ptr<Environment>(new Environment());
}

auto Environment::Create(const std::shared_ptr<Environment>& enclosing)
    -> std::shared_ptr<Environment> {
  return std::shared_ptr<Environment>(new Environment(enclosing));
}

auto Environment::Get(const Token& variable) const -> Object {
  const std::string& variable_name = variable.GetLexeme();
  if (values_.contains(variable_name)) {
    return values_.at(variable_name);
  }

  if (enclosing_) {
    return enclosing_->Get(variable);
  }

  throw RuntimeError{variable, "Undefined variable '" + variable_name + "'."};
}

auto Environment::GetAt(uint64_t distance, const Token& name) -> Object {
  return Ancestor(distance)->Get(name);
}

auto Environment::Define(const std::string& name, const Object& value) -> void {
  values_[name] = value;
}

auto Environment::Assign(const Token& variable, const Object& value) -> void {
  const std::string& variable_name = variable.GetLexeme();
  if (values_.contains(variable_name)) {
    values_[variable_name] = value;
    return;
  }

  if (enclosing_) {
    enclosing_->Assign(variable, value);
    return;
  }

  throw RuntimeError(variable, "Undefined variable '" + variable_name + "'.");
}

auto Environment::AssignAt(uint64_t distance, const Token& variable,
                           const Object& value) -> void {
  Ancestor(distance)->Assign(variable, value);
}

auto Environment::GetEnclosingEnvironment() const noexcept
    -> const std::shared_ptr<Environment>& {
  return enclosing_;
}

auto Environment::Ancestor(uint64_t distance) -> std::shared_ptr<Environment> {
  std::shared_ptr<Environment> environment = shared_from_this();
  for (uint32_t i = 0; i < distance; i++) {
    environment = environment->GetEnclosingEnvironment();
  }

  return environment;
}
}  // namespace cclox
