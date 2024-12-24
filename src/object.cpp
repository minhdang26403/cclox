#include "object.h"

#include <cstddef>
#include <optional>
#include <sstream>
#include <variant>

#include "lox_callable.h"
#include "lox_instance.h"

namespace cclox {
auto Object::AsInteger() const noexcept -> std::optional<int32_t> {
  if (auto pval = std::get_if<int32_t>(&value_)) {
    return *pval;
  }
  if (auto pval = std::get_if<double>(&value_)) {
    return static_cast<int32_t>(*pval);
  }
  return std::nullopt;
}

auto Object::AsDouble() const noexcept -> std::optional<double> {
  if (auto pval = std::get_if<double>(&value_)) {
    return *pval;
  }
  if (auto pval = std::get_if<int32_t>(&value_)) {
    return static_cast<double>(*pval);
  }
  return std::nullopt;
}

auto Object::AsString() const noexcept -> std::optional<std::string> {
  if (auto pval = std::get_if<std::string>(&value_)) {
    return *pval;
  }
  return std::nullopt;
}

auto Object::AsLoxCallable() const noexcept -> std::optional<LoxCallablePtr> {
  if (auto pval = std::get_if<LoxCallablePtr>(&value_)) {
    return *pval;
  }
  return std::nullopt;
}

auto Object::AsLoxInstance() const noexcept -> std::optional<LoxInstancePtr> {
  if (auto pval = std::get_if<LoxInstancePtr>(&value_)) {
    return *pval;
  }
  return std::nullopt;
}

auto Object::IsTruthy() const noexcept -> bool {
  return std::visit(
      [](auto&& v) -> bool {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, bool>) {
          return v;
        } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
          return false;
        } else if constexpr (std::is_same_v<T, int32_t>) {
          return v != 0;
        } else if constexpr (std::is_same_v<T, double>) {
          return v != 0.0;
        } else if constexpr (std::is_same_v<T, std::string>) {
          return !v.empty();
        } else if constexpr (std::is_same_v<T, LoxCallablePtr>) {
          return v != nullptr;
        } else if constexpr (std::is_same_v<T, LoxInstancePtr>) {
          return v != nullptr;
        }
      },
      value_);
}

auto Object::ToString() const -> std::string {
  return std::visit(
      [](auto&& value) -> std::string {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>) {
          return "nil";
        } else if constexpr (std::is_same_v<T, bool>) {
          return value ? "true" : "false";
        } else if constexpr (  // NOLINT(readability/braces)
            std::is_same_v<T, LoxCallablePtr> ||
            std::is_same_v<T, LoxInstancePtr>) {
          return value->ToString();
        } else {
          std::ostringstream oss;
          oss << value;
          return oss.str();
        }
      },
      value_);
}
}  // namespace cclox
