#include "resolver.h"
#include "expr.h"
#include "lox.h"
#include "stmt.h"

namespace cclox {
// ====================Statement Visitors====================
auto Resolver::operator()(const BlockStmtPtr& stmt) -> void {
  BeginScope();
  ResolveStatements(stmt->GetStatements());
  EndScope();
}

auto Resolver::operator()(const ClassStmtPtr& stmt) -> void {
  ClassType enclosing_class = current_class_;
  current_class_ = ClassType::CLASS;

  Declare(stmt->GetClassName());
  Define(stmt->GetClassName());

  BeginScope();
  scopes_.back().emplace("this", true);

  for (const auto& method_var : stmt->GetClassMethods()) {
    FunctionType declaration = FunctionType::METHOD;
    const FunctionStmtPtr& method = std::get<FunctionStmtPtr>(method_var);
    if (method->GetFunctionName().GetLexeme() == "init") {
      declaration = FunctionType::INITIALIZER;
    }
    ResolveFunction(method, declaration);
  }

  EndScope();

  current_class_ = enclosing_class;
}

auto Resolver::operator()(const ExprStmtPtr& stmt) -> void {
  ResolveExpression(stmt->GetExpression());
}

auto Resolver::operator()(const FunctionStmtPtr& stmt) -> void {
  Declare(stmt->GetFunctionName());
  Define(stmt->GetFunctionName());

  ResolveFunction(stmt, FunctionType::FUNCTION);
}

auto Resolver::operator()(const IfStmtPtr& stmt) -> void {
  ResolveExpression(stmt->GetCondition());
  ResolveStatement(stmt->GetThenBranch());
  const std::optional<StmtPtr>& else_branch_opt = stmt->GetElseBranch();
  if (else_branch_opt) {
    ResolveStatement(else_branch_opt.value());
  }
}

auto Resolver::operator()(const PrintStmtPtr& stmt) -> void {
  ResolveExpression(stmt->GetExpression());
}

auto Resolver::operator()(const ReturnStmtPtr& stmt) -> void {
  if (current_function_ == FunctionType::NONE) {
    Lox::Error(interpreter_.GetOutputStream(), stmt->GetKeyword(),
               "Can't return from top-level code.");
  }

  const std::optional<ExprPtr>& value_expr_opt = stmt->GetValue();
  if (value_expr_opt) {
    if (current_function_ == FunctionType::INITIALIZER) {
      Lox::Error(interpreter_.GetOutputStream(), stmt->GetKeyword(),
                 "Can't return a value from an initializer.");
    }
    ResolveExpression(value_expr_opt.value());
  }
}

auto Resolver::operator()(const VarStmtPtr& stmt) -> void {
  Declare(stmt->GetVariable());
  const std::optional<ExprPtr>& initializer_opt = stmt->GetInitializer();
  if (initializer_opt) {
    ResolveExpression(initializer_opt.value());
  }
  Define(stmt->GetVariable());
}

auto Resolver::operator()(const WhileStmtPtr& stmt) -> void {
  ResolveExpression(stmt->GetCondition());
  ResolveStatement(stmt->GetBody());
}

auto Resolver::ResolveStatements(const std::vector<StmtPtr>& statements)
    -> void {
  for (const auto& statement : statements) {
    ResolveStatement(statement);
  }
}

auto Resolver::ResolveStatement(const StmtPtr& stmt) -> void {
  std::visit(*this, stmt);
}

auto Resolver::ResolveExpression(const ExprPtr& expr) -> void {
  // The visitor needs access to the `expr_var` variable, so we define a lambda
  // instead of implementing overload methods
  std::visit(*this, expr);
}

auto Resolver::operator()(const AssignExprPtr& expr) -> void {
  ResolveExpression(expr->GetValue());
  ResolveLocalVariable(ExprPtr{expr}, expr->GetVariable());
}

auto Resolver::operator()(const BinaryExprPtr& expr) -> void {
  ResolveExpression(expr->GetLeftExpression());
  ResolveExpression(expr->GetRightExpression());
}

auto Resolver::operator()(const CallExprPtr& expr) -> void {
  ResolveExpression(expr->GetCallee());

  for (const auto& argument : expr->GetArguments()) {
    ResolveExpression(argument);
  }
}

auto Resolver::operator()(const GetExprPtr& expr) -> void {
  ResolveExpression(expr->GetObject());
}

auto Resolver::operator()(const GroupingExprPtr& expr) -> void {
  ResolveExpression(expr->GetExpression());
}

auto Resolver::operator()([[maybe_unused]] const LiteralExprPtr& expr) -> void {
  // A literal expression doesn't mention any variables, so nothing to
  // resolve
}

auto Resolver::operator()(const LogicalExprPtr& expr) -> void {
  ResolveExpression(expr->GetLeftExpression());
  ResolveExpression(expr->GetRightExpression());
}

auto Resolver::operator()(const SetExprPtr& expr) -> void {
  ResolveExpression(expr->GetValue());
  ResolveExpression(expr->GetObject());
}

auto Resolver::operator()(const ThisExprPtr& expr) -> void {
  if (current_class_ == ClassType::CLASS) {
    Lox::Error(interpreter_.GetOutputStream(), expr->GetKeyword(),
               "Can't use 'this' outside of a class.");
  }
  ResolveLocalVariable(ExprPtr{expr}, expr->GetKeyword());
}

auto Resolver::operator()(const UnaryExprPtr& expr) -> void {
  ResolveExpression(expr->GetRightExpression());
}

auto Resolver::operator()(const VariableExprPtr& expr) -> void {
  std::cout << expr->GetVariable().GetLexeme() << '\n';
  if (!scopes_.empty()) {
    auto it = scopes_.back().find(expr->GetVariable().GetLexeme());
    if (it != scopes_.back().end() && it->second == false) {
      Lox::Error(interpreter_.GetOutputStream(), expr->GetVariable(),
                 "Can't read local variable in its own initializer.");
    }
  }

  ResolveLocalVariable(ExprPtr{expr}, expr->GetVariable());
}

auto Resolver::BeginScope() -> void {
  // Create a new environment object for the block scope
  scopes_.emplace_back();
}

auto Resolver::EndScope() -> void {
  scopes_.pop_back();
}

auto Resolver::Declare(const Token& variable) -> void {
  if (scopes_.empty()) {
    return;
  }

  std::unordered_map<std::string, bool>& scope = scopes_.back();
  if (scope.contains(variable.GetLexeme())) {
    Lox::Error(interpreter_.GetOutputStream(), variable,
               "Already a variable with this name in this scope.");
  }

  scope[variable.GetLexeme()] = false;
}

auto Resolver::Define(const Token& variable) -> void {
  if (scopes_.empty()) {
    return;
  }
  std::unordered_map<std::string, bool>& scope = scopes_.back();
  scope[variable.GetLexeme()] = true;
}

auto Resolver::ResolveLocalVariable(const ExprPtr& expr, const Token& variable)
    -> void {
  if (scopes_.empty()) {
    return;
  }

  for (size_t i = scopes_.size() - 1; i >= 0; i--) {
    if (scopes_.at(i).contains(variable.GetLexeme())) {
      interpreter_.ResolveVariable(expr, scopes_.size() - 1 - i);
      return;
    }
  }
}

auto Resolver::ResolveFunction(const FunctionStmtPtr& function,
                               FunctionType type) -> void {
  FunctionType enclosing_function = current_function_;
  current_function_ = type;

  BeginScope();

  for (const auto& param : function->GetParams()) {
    Declare(param);
    Define(param);
  }
  ResolveStatements(function->GetBody());

  EndScope();

  current_function_ = enclosing_function;
}

}  // namespace cclox
