#include "interpreter.h"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <variant>

#include "expr.h"
#include "lox.h"
#include "object.h"
#include "token.h"
#include "token_type.h"

namespace cclox {
auto Interpreter::Interpret(const ExprPtr& expr) const -> void {
  try {
    Object value = Evaluate(expr);
    std::cout << value.ToString() << '\n';
  } catch (const RuntimeError& error) {
    Lox::ReportRuntimeError(error);
  }
}

auto Interpreter::Evaluate(const ExprPtr& expr) const -> Object {
  return std::visit(*this, expr);
}

auto Interpreter::operator()(const BinaryPtr& expr) const -> Object {
  Object left = Evaluate(expr->GetLeftExpr());
  Object right = Evaluate(expr->GetRightExpr());

  using enum TokenType;
  const Token& op = expr->GetOperator();

  switch (op.GetType()) {
    case BANG_EQUAL:
      return !Equal(left, right);
    case EQUAL_EQUAL:
      return Equal(left, right);
    case GREATER:
      return Greater(left, op, right);
    case GREATER_EQUAL:
      return !Less(left, op, right);
    case LESS:
      return Less(left, op, right);
    case LESS_EQUAL:
      return !Greater(left, op, right);
    case MINUS:
      return Subtract(left, op, right);
    case PLUS:
      return Add(left, op, right);
    case SLASH:
      return Divide(left, op, right);
    case STAR:
      return Multiply(left, op, right);
    default:
      break;
  }

  // Unreachable
  assert(false);
}

auto Interpreter::operator()(const GroupingPtr& expr) const -> Object {
  return Evaluate(expr->GetExpr());
}

auto Interpreter::operator()(const LiteralPtr& expr) const -> Object {
  return expr->GetValue();
}

auto Interpreter::operator()(const UnaryPtr& expr) const -> Object {
  Object right = Evaluate(expr->GetRightExpression());

  using enum TokenType;
  const Token& op = expr->GetOperator();

  switch (op.GetType()) {
    case BANG:
      return !right.IsTruthy();
    case MINUS:
      return Subtract(Object{static_cast<int32_t>(0)}, op, right);
    default:
      break;
  }

  // Unreachable
  assert(false);
}

auto Interpreter::Equal(const Object& left, const Object& right) const -> bool {
  std::optional<double> left_num = left.AsDouble();
  std::optional<double> right_num = right.AsDouble();
  // Arithmetic types need special treatment since we consider 0 == 0.0.
  // operator== defined in std::variant treats this as `false` since
  // the two variants hold different types.
  if (left_num && right_num) {
    return left_num == right_num;
  }
  return left.Value() == right.Value();
}

auto Interpreter::Greater(const Object& left, const Token& op,
                          const Object& right) const -> bool {
  auto [left_num, right_num] = GetNumberOperands(left, op, right);
  return left_num > right_num;
}

auto Interpreter::Less(const Object& left, const Token& op,
                       const Object& right) const -> bool {
  auto [left_num, right_num] = GetNumberOperands(left, op, right);
  return left_num < right_num;
}

auto Interpreter::Add(const Object& left, const Token& op,
                      const Object& right) const -> Object {
  if (left.IsString() && right.IsString()) {
    return Object(left.Get<std::string>() + right.Get<std::string>());
  }

  std::optional<double> left_num = left.AsDouble();
  std::optional<double> right_num = right.AsDouble();

  if (left_num && right_num) {
    if (left.IsInteger() && right.IsInteger()) {
      int32_t a = left.Get<int32_t>();
      int32_t b = right.Get<int32_t>();
      int32_t res = 0;
      if (!__builtin_add_overflow(a, b, &res)) {
        return Object{res};
      }
    }
    return Object{*left_num + *right_num};
  }

  throw RuntimeError(op, "Operands must be two numbers or two strings.");
}

auto Interpreter::Subtract(const Object& left, const Token& op,
                           const Object& right) const -> Object {
  auto [left_num, right_num] = GetNumberOperands(left, op, right);

  if (left.IsInteger() && right.IsInteger()) {
    int32_t res = 0;
    if (!__builtin_sub_overflow(static_cast<int32_t>(left_num),
                                static_cast<int32_t>(right_num), &res)) {
      return Object{res};
    }
  }

  return Object{left_num - right_num};
}

auto Interpreter::Divide(const Object& left, const Token& op,
                         const Object& right) const -> Object {
  auto [left_num, right_num] = GetNumberOperands(left, op, right);

  if (left.IsInteger() && right.IsInteger()) {
    return Object{static_cast<int32_t>(left_num) /
                  static_cast<int32_t>(right_num)};
  }
  
  auto obj = Object{left_num / right_num};
  return obj;
}

auto Interpreter::Multiply(const Object& left, const Token& op,
                           const Object& right) const -> Object {
  auto [left_num, right_num] = GetNumberOperands(left, op, right);

  if (left.IsInteger() && right.IsInteger()) {
    int32_t res = 0;
    if (!__builtin_mul_overflow(static_cast<int32_t>(left_num),
                                static_cast<int32_t>(right_num), &res)) {
      return Object{res};
    }
  }
  return Object{left_num * right_num};
}

auto Interpreter::GetNumberOperands(const Object& left, const Token& op,
                                    const Object& right) const
    -> std::pair<double, double> {
  std::optional<double> left_num = left.AsDouble();
  std::optional<double> right_num = right.AsDouble();

  if (!left_num || !right_num) {
    throw RuntimeError(op, "Operands must be numbers.");
  }

  return std::pair{left_num.value(), right_num.value()};
}

}  // namespace cclox