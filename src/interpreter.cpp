#include "interpreter.h"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <variant>

#include "environment.h"
#include "expr.h"
#include "lox.h"
#include "object.h"
#include "stmt.h"
#include "token.h"
#include "token_type.h"

namespace cclox {
auto Interpreter::Interpret(const std::vector<StmtPtr>& statements) -> void {
  try {
    for (const auto& statement : statements) {
      ExecuteStatement(statement);
    }
  } catch (const RuntimeError& error) {
    Lox::ReportRuntimeError(error);
  }
}

//====================Methods to handle statement====================
auto Interpreter::ExecuteStatement(const StmtPtr& stmt) -> void {
  std::visit(*this, stmt);
}

auto Interpreter::operator()(const BlockStmtPtr& stmt) -> void {
  assert(stmt);
  // Create a new inner env for this block.
  // The inner env will has the current env as its enclosing env.
  auto inner_env = std::make_shared<Environment>(environment_);
  ExecuteBlockStatement(stmt->GetStatements(), std::move(inner_env));
}

auto Interpreter::operator()(const ClassStmtPtr& stmt [[maybe_unused]])
    -> void {}

auto Interpreter::operator()(const ExprStmtPtr& stmt) -> void {
  assert(stmt);
  EvaluateExpression(stmt->GetExpression());
}

auto Interpreter::operator()(const FunctionStmtPtr& stmt [[maybe_unused]])
    -> void {}

auto Interpreter::operator()(const IfStmtPtr& stmt [[maybe_unused]]) -> void {}

auto Interpreter::operator()(const PrintStmtPtr& stmt) -> void {
  assert(stmt);
  Object value = EvaluateExpression(stmt->GetExpression());
  std::cout << value.ToString() << '\n';
}

auto Interpreter::operator()(const ReturnStmtPtr& stmt [[maybe_unused]])
    -> void {}

auto Interpreter::operator()(const VarStmtPtr& stmt) -> void {
  assert(stmt);
  Object value{nullptr};
  const std::optional<ExprPtr>& initializer_opt = stmt->GetInitializer();
  if (initializer_opt) {
    value = EvaluateExpression(initializer_opt.value());
  }

  environment_->Define(stmt->GetVariable().GetLexeme(), value);
}

auto Interpreter::operator()(const WhileStmtPtr& stmt [[maybe_unused]])
    -> void {}

auto Interpreter::ExecuteBlockStatement(
    const std::vector<StmtPtr>& statements,
    std::shared_ptr<Environment> environment) -> void {
  std::shared_ptr<Environment> previous = std::move(environment_);
  try {
    environment_ = std::move(environment);

    for (const auto& statement : statements) {
      ExecuteStatement(statement);
    }
  } catch (const RuntimeError& error) {
    environment_ = std::move(previous);
    // Rethrow the exception.
    throw;
  }
  
  // Restore the current environment if no exception is thrown.
  environment_ = std::move(previous);
}

//====================Methods to handle expressions====================
auto Interpreter::EvaluateExpression(const ExprPtr& expr) -> Object {
  return std::visit(*this, expr);
}

auto Interpreter::operator()(const AssignExprPtr& expr) -> Object {
  assert(expr);
  Object value = EvaluateExpression(expr->GetValue());
  environment_->Assign(expr->GetVariable(), value);

  return value;
}

auto Interpreter::operator()(const BinaryExprPtr& expr) -> Object {
  assert(expr);
  Object left = EvaluateExpression(expr->GetLeftExpr());
  Object right = EvaluateExpression(expr->GetRightExpr());

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

auto Interpreter::operator()(const GroupingExprPtr& expr) -> Object {
  assert(expr);
  return EvaluateExpression(expr->GetExpr());
}

auto Interpreter::operator()(const LiteralExprPtr& expr) -> Object {
  assert(expr);
  return expr->GetValue();
}

auto Interpreter::operator()(const UnaryExprPtr& expr) -> Object {
  assert(expr);
  Object right = EvaluateExpression(expr->GetRightExpression());

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

auto Interpreter::operator()(const VariableExprPtr& expr) -> Object {
  assert(expr);
  return environment_->Get(expr->GetVariable());
}

// ====================Private method implementations====================

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