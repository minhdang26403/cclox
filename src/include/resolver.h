#ifndef RESOLVER_H_
#define RESOLVER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "expr.h"
#include "interpreter.h"
#include "stmt.h"

namespace cclox {
class Resolver {
 public:
  explicit Resolver(Interpreter& interpreter) : interpreter_(interpreter) {}

  auto ResolveStatements(const std::vector<StmtPtr>& statements) -> void;

  // ====================Statement Visitors====================
  auto operator()(const BlockStmtPtr& stmt) -> void;

  auto operator()(const ClassStmtPtr& stmt) -> void;

  auto operator()(const ExprStmtPtr& stmt) -> void;

  auto operator()(const FunctionStmtPtr& stmt) -> void;

  auto operator()(const IfStmtPtr& stmt) -> void;

  auto operator()(const PrintStmtPtr& stmt) -> void;

  auto operator()(const ReturnStmtPtr& stmt) -> void;

  auto operator()(const VarStmtPtr& stmt) -> void;

  auto operator()(const WhileStmtPtr& stmt) -> void;

  // ====================Expression Visitors====================
  auto operator()(const AssignExprPtr& expr) -> void;

  auto operator()(const BinaryExprPtr& expr) -> void;

  auto operator()(const CallExprPtr& expr) -> void;

  auto operator()(const GetExprPtr& expr) -> void;

  auto operator()(const GroupingExprPtr& expr) -> void;

  auto operator()(const LiteralExprPtr& expr) -> void;

  auto operator()(const LogicalExprPtr& expr) -> void;

  auto operator()(const SetExprPtr& expr) -> void;

  auto operator()(const ThisExprPtr& expr) -> void;

  auto operator()(const UnaryExprPtr& expr) -> void;

  auto operator()(const VariableExprPtr& expr) -> void;

 private:
  enum class FunctionType {
    NONE,
    FUNCTION,
    INITIALIZER,
    METHOD,
  };

  enum class ClassType {
    NONE,
    CLASS,
  };

  auto ResolveStatement(const StmtPtr& stmt) -> void;

  auto ResolveExpression(const ExprPtr& expr) -> void;

  auto BeginScope() -> void;

  auto EndScope() -> void;

  auto Declare(const Token& variable) -> void;

  auto Define(const Token& variable) -> void;

  auto ResolveLocalVariable(const ExprPtr& expr, const Token& variable) -> void;

  auto ResolveFunction(const FunctionStmtPtr& function, FunctionType type)
      -> void;

  Interpreter& interpreter_;
  std::vector<std::unordered_map<std::string, bool>> scopes_;
  FunctionType current_function_{FunctionType::NONE};
  ClassType current_class_{ClassType::NONE};
};
}  // namespace cclox

#endif  // RESOLVER_H_
