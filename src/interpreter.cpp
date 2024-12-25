#include "interpreter.h"

#include <cassert>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <variant>

#include "environment.h"
#include "expr.h"
#include "lox.h"
#include "lox_callable.h"
#include "lox_class.h"
#include "lox_function.h"
#include "lox_instance.h"
#include "native_clock_function.h"
#include "object.h"
#include "return.h"
#include "stmt.h"
#include "token.h"
#include "token_type.h"

namespace cclox {
Interpreter::Interpreter() {
  DefineNativeFunctions();
}

Interpreter::Interpreter(std::ostream& output) : output_(output) {
  DefineNativeFunctions();
}

auto Interpreter::Interpret(const std::vector<StmtPtr>& statements) -> void {
  try {
    for (const auto& statement : statements) {
      ExecuteStatement(statement);
    }
  } catch (const RuntimeError& error) {
    Lox::ReportRuntimeError(output_, error);
  }
}

auto Interpreter::ResolveVariable(const ExprPtr& expr, uint64_t depth) -> void {
  locals_[expr] = depth;
}

auto Interpreter::GetOutputStream() const -> std::ostream& {
  return output_;
}

// ====================Methods to handle statement====================
auto Interpreter::ExecuteStatement(const StmtPtr& stmt) -> void {
  std::visit(*this, stmt);
}

auto Interpreter::operator()(const BlockStmtPtr& stmt) -> void {
  assert(stmt);
  // Create a new inner env for this block.
  // The inner env will has the current env as its enclosing env.
  auto inner_env = Environment::Create(environment_);
  ExecuteBlockStatement(stmt->GetStatements(), std::move(inner_env));
}

auto Interpreter::operator()(const ClassStmtPtr& stmt) -> void {
  const Token& class_name = stmt->GetClassName();

  environment_->Define(class_name.GetLexeme(), Object{nullptr});
  LoxClass::MethodMap methods;
  for (const auto& method_var : stmt->GetClassMethods()) {
    const auto& method = std::get<FunctionStmtPtr>(method_var);
    std::string method_name = method->GetFunctionName().GetLexeme();
    const bool is_initializer = method_name == "init";
    auto function =
        std::make_shared<LoxFunction>(method, environment_, is_initializer);
    methods.emplace(std::move(method_name), std::move(function));
  }

  auto klass =
      std::make_shared<LoxClass>(class_name.GetLexeme(), std::move(methods));
  environment_->Assign(class_name, Object{klass});
}

auto Interpreter::operator()(const ExprStmtPtr& stmt) -> void {
  assert(stmt);
  EvaluateExpression(stmt->GetExpression());
}

auto Interpreter::operator()(const FunctionStmtPtr& stmt) -> void {
  auto function = std::make_shared<LoxFunction>(stmt, environment_, false);
  environment_->Define(stmt->GetFunctionName().GetLexeme(),
                       Object{std::move(function)});
}

auto Interpreter::operator()(const IfStmtPtr& stmt) -> void {
  Object result = EvaluateExpression(stmt->GetCondition());
  const std::optional<StmtPtr>& else_branch_opt = stmt->GetElseBranch();
  if (result.IsTruthy()) {
    ExecuteStatement(stmt->GetThenBranch());
  } else if (else_branch_opt) {
    ExecuteStatement(else_branch_opt.value());
  }
}

auto Interpreter::operator()(const PrintStmtPtr& stmt) -> void {
  assert(stmt);
  Object value = EvaluateExpression(stmt->GetExpression());
  output_ << value.ToString() << '\n';
}

auto Interpreter::operator()(const ReturnStmtPtr& stmt) -> void {
  std::optional<Object> value;
  const std::optional<ExprPtr>& value_expr_opt = stmt->GetValue();
  if (value_expr_opt) {
    value = EvaluateExpression(value_expr_opt.value());
  }

  throw Return(std::move(value));
}

auto Interpreter::operator()(const VarStmtPtr& stmt) -> void {
  assert(stmt);
  Object value{nullptr};
  const std::optional<ExprPtr>& initializer_opt = stmt->GetInitializer();
  if (initializer_opt) {
    value = EvaluateExpression(initializer_opt.value());
  }

  environment_->Define(stmt->GetVariable().GetLexeme(), value);
}

auto Interpreter::operator()(const WhileStmtPtr& stmt) -> void {
  while (EvaluateExpression(stmt->GetCondition()).IsTruthy()) {
    ExecuteStatement(stmt->GetBody());
  }
}

auto Interpreter::ExecuteBlockStatement(
    const std::vector<StmtPtr>& statements,
    std::shared_ptr<Environment> environment) -> void {
  using std::move;
  std::shared_ptr<Environment> previous = move(environment_);
  try {
    environment_ = move(environment);

    for (const auto& statement : statements) {
      ExecuteStatement(statement);
    }
  } catch (const std::runtime_error&) {
    // Catch generic `std::runtime_error`, which is the base class of both
    // `RuntimeError` exception and `Return` exception so that we can restore
    // environment in both cases.
    environment_ = move(previous);
    throw;
  }

  // Restore the current environment if no exception is thrown.
  environment_ = move(previous);
}

// ====================Methods to handle expressions====================
auto Interpreter::EvaluateExpression(const ExprPtr& expr) -> Object {
  return std::visit(*this, expr);
}

auto Interpreter::operator()(const AssignExprPtr& expr) -> Object {
  assert(expr);
  Object value = EvaluateExpression(expr->GetValue());

  if (locals_.contains(expr)) {
    environment_->AssignAt(locals_.at(expr), expr->GetVariable(), value);
  } else {
    globals_->Assign(expr->GetVariable(), value);
  }

  return value;
}

auto Interpreter::operator()(const BinaryExprPtr& expr) -> Object {
  assert(expr);
  Object left = EvaluateExpression(expr->GetLeftExpression());
  Object right = EvaluateExpression(expr->GetRightExpression());

  using enum TokenType;
  const Token& op = expr->GetOperator();

  switch (op.GetType()) {
    case BANG_EQUAL:
      return Object{!Equal(left, right)};
    case EQUAL_EQUAL:
      return Object{Equal(left, right)};
    case GREATER:
      return Object{Greater(left, op, right)};
    case GREATER_EQUAL:
      return Object{!Less(left, op, right)};
    case LESS:
      return Object{Less(left, op, right)};
    case LESS_EQUAL:
      return Object{!Greater(left, op, right)};
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

auto Interpreter::operator()(const CallExprPtr& expr) -> Object {
  Object callee = EvaluateExpression(expr->GetCallee());

  std::vector<Object> arguments;
  arguments.reserve(expr->GetArguments().size());
  for (const auto& argument : expr->GetArguments()) {
    arguments.emplace_back(EvaluateExpression(argument));
  }

  std::optional<LoxCallablePtr> function_opt = callee.AsLoxCallable();
  if (!function_opt) {
    throw RuntimeError(expr->GetParen(),
                       "Can only call functions and classes.");
  }

  const LoxCallablePtr& function = function_opt.value();
  if (arguments.size() != function->Arity()) {
    throw RuntimeError(expr->GetParen(),
                       std::format("Expected {} arguments but got {}.",
                                   function->Arity(), arguments.size()));
  }

  return function->Call(*this, arguments);
}

auto Interpreter::operator()(const GetExprPtr& expr) -> Object {
  Object object = EvaluateExpression(expr->GetObject());
  std::optional<LoxInstancePtr> instance_opt = object.AsLoxInstance();
  if (instance_opt) {
    return instance_opt.value()->GetField(expr->GetProperty());
  }

  throw RuntimeError(expr->GetProperty(), "Only instances have properties.");
}

auto Interpreter::operator()(const GroupingExprPtr& expr) -> Object {
  assert(expr);
  return EvaluateExpression(expr->GetExpression());
}

auto Interpreter::operator()(const LiteralExprPtr& expr) -> Object {
  assert(expr);
  return expr->GetValue();
}

auto Interpreter::operator()(const LogicalExprPtr& expr) -> Object {
  assert(expr);
  Object left = EvaluateExpression(expr->GetLeftExpression());

  if (expr->GetOperator().GetType() == TokenType::OR) {
    if (left.IsTruthy()) {
      return left;
    }
  } else {
    if (!left.IsTruthy()) {
      return left;
    }
  }

  return EvaluateExpression(expr->GetRightExpression());
}

auto Interpreter::operator()(const SetExprPtr& expr) -> Object {
  Object object = EvaluateExpression(expr->GetObject());

  std::optional<LoxInstancePtr> lox_instance_opt = object.AsLoxInstance();
  if (!lox_instance_opt) {
    throw RuntimeError(expr->GetProperty(), "Only instances have fields.");
  }

  Object value = EvaluateExpression(expr->GetValue());
  lox_instance_opt.value()->SetField(expr->GetProperty(), value);
  return value;
}

auto Interpreter::operator()(const ThisExprPtr& expr) -> Object {
  return LookUpVariable(expr->GetKeyword(), expr);
}

auto Interpreter::operator()(const UnaryExprPtr& expr) -> Object {
  assert(expr);
  Object right = EvaluateExpression(expr->GetRightExpression());

  using enum TokenType;
  const Token& op = expr->GetOperator();

  switch (op.GetType()) {
    case BANG:
      return Object{!right.IsTruthy()};
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
  return LookUpVariable(expr->GetVariable(), expr);
}

// ====================Private method implementations====================
auto Interpreter::DefineNativeFunctions() -> void {
  environment_->Define("clock",
                       Object{std::make_shared<NativeClockFunction>()});
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

auto Interpreter::LookUpVariable(const Token& variable, const ExprPtr& expr)
    -> Object {
  if (locals_.contains(expr)) {
    return environment_->GetAt(locals_.at(expr), variable);
  } else {
    return globals_->Get(variable);
  }
}

}  // namespace cclox
