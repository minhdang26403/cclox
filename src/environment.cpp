#include "environment.h"
#include "interpreter.h"

namespace cclox {
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
  }

  throw RuntimeError(variable, "Undefined variable '" + variable_name + "'.");
}
}  // namespace cclox
