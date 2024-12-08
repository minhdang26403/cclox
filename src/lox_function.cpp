#include "lox_function.h"

#include "environment.h"
#include "interpreter.h"
#include "object.h"
#include "return.h"

namespace cclox {
auto LoxFunction::Arity() const noexcept -> size_t {
  return declaration_->GetParams().size();
}

auto LoxFunction::Call(Interpreter& interpreter,
                       const std::vector<Object>& arguments) -> Object {
  auto environment = std::make_shared<Environment>(closure_);
  const std::vector<Token>& params = declaration_->GetParams();
  for (size_t i = 0; i < params.size(); i++) {
    environment->Define(params[i].GetLexeme(), arguments[i]);
  }

  try {
    interpreter.ExecuteBlockStatement(declaration_->GetBody(),
                                      std::move(environment));
  } catch (const Return& return_obj) {
    const std::optional<Object>& return_value_opt = return_obj.GetReturnValue();
    if (return_value_opt) {
      return return_value_opt.value();
    }
  }

  return Object{nullptr};
}

auto LoxFunction::ToString() const -> std::string {
  return std::format("<fn {}>", declaration_->GetName().GetLexeme());
}

}  // namespace cclox
