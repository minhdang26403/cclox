#include "lox_class.h"

#include <memory>

#include "lox_function.h"
#include "lox_instance.h"
#include "object.h"

namespace cclox {
auto LoxClass::FindMethod(const std::string& name) const -> LoxCallablePtr {
  if (methods_.contains(name)) {
    return methods_.at(name);
  }

  if (superclass_) {
    std::optional<LoxCallablePtr> lox_callable = superclass_->AsLoxCallable();
    // The `lox_callable` optional should contain a LoxCallPtr value. Otherwise,
    // it's an unrecoverable error, so let the code throw an exception when
    // calling `value()` on the optional.
    auto superclass_ptr = static_pointer_cast<LoxClass>(lox_callable.value());
    return superclass_ptr->FindMethod(name);
  }

  return nullptr;
}

auto LoxClass::Arity() const noexcept -> size_t {
  LoxCallablePtr initializer = FindMethod("init");
  if (initializer) {
    return initializer->Arity();
  }

  return 0;
}

auto LoxClass::Call(Interpreter& interpreter,
                    const std::vector<Object>& arguments) -> Object {
  LoxInstancePtr instance = LoxInstance::Create(*this);
  LoxCallablePtr initializer = FindMethod("init");
  if (initializer) {
    std::static_pointer_cast<LoxFunction>(initializer)
        ->Bind(instance)
        ->Call(interpreter, arguments);
  }

  return Object{std::move(instance)};
}

auto LoxClass::ToString() const -> std::string {
  return name_;
}
}  // namespace cclox
