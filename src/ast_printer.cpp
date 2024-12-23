#include "ast_printer.h"
#include <format>
#include "expr.h"

namespace cclox {
// ====================AST Printer for Statements====================
auto ASTPrinter::Print(const StmtPtr& stmt) const -> std::string {
  return std::visit(*this, stmt);
}

auto ASTPrinter::operator()(const BlockStmtPtr& block_stmt) const
    -> std::string {
  std::string s = "(block ";

  for (const auto& stmt : block_stmt->GetStatements()) {
    s.append(Print(stmt));
  }
  s.push_back(')');

  return s;
}

auto ASTPrinter::operator()(const ClassStmtPtr&) const -> std::string {
  return "";
}

auto ASTPrinter::operator()(const ExprStmtPtr& stmt) const -> std::string {
  return Parenthesize(";", stmt->GetExpression());
}

auto ASTPrinter::operator()(const FunctionStmtPtr& stmt) const -> std::string {
  std::string s = std::format("(fun {}(", stmt->GetFunctionName().GetLexeme());

  for (const auto& param : stmt->GetParams()) {
    s.append(param.GetLexeme());
    s.push_back(' ');
  }
  s.pop_back();
  s.append(") ");

  for (const auto& body : stmt->GetBody()) {
    s.append(Print(body));
  }
  s.push_back(')');

  return s;
}

auto ASTPrinter::operator()(const IfStmtPtr& stmt) const -> std::string {
  const std::optional<StmtPtr>& else_branch_opt = stmt->GetElseBranch();
  if (else_branch_opt) {
    return Parenthesize("if-else", stmt->GetCondition(), stmt->GetThenBranch(),
                        else_branch_opt.value());
  }

  return Parenthesize("if", stmt->GetCondition(), stmt->GetThenBranch());
}

auto ASTPrinter::operator()(const PrintStmtPtr& stmt) const -> std::string {
  return Parenthesize("print", stmt->GetExpression());
}

auto ASTPrinter::operator()(const ReturnStmtPtr& stmt) const -> std::string {
  const std::optional<ExprPtr>& value_opt = stmt->GetValue();
  if (value_opt) {
    return Parenthesize("return", value_opt.value());
  }

  return "(return)";
}

auto ASTPrinter::operator()(const VarStmtPtr& stmt) const -> std::string {
  const std::optional<ExprPtr>& initializer_opt = stmt->GetInitializer();
  if (initializer_opt) {
    return Parenthesize("var " + stmt->GetVariable().GetLexeme() + " =",
                        initializer_opt.value());
  }

  return std::format("(var {})", stmt->GetVariable().GetLexeme());
}

auto ASTPrinter::operator()(const WhileStmtPtr& stmt) const -> std::string {
  return Parenthesize("while", stmt->GetCondition(), stmt->GetBody());
}

// ====================AST Printer for Expressions====================
auto ASTPrinter::Print(const ExprPtr& expr) const -> std::string {
  return std::visit(*this, expr);
}

auto ASTPrinter::operator()(const AssignExprPtr& expr) const -> std::string {
  return Parenthesize("= " + expr->GetVariable().GetLexeme(), expr->GetValue());
}

auto ASTPrinter::operator()(const BinaryExprPtr& expr) const -> std::string {
  return Parenthesize(expr->GetOperator().GetLexeme(),
                      expr->GetLeftExpression(), expr->GetRightExpression());
}

auto ASTPrinter::operator()(const CallExprPtr& expr) const -> std::string {
  std::string s = "(call ";

  for (const auto& argument : expr->GetArguments()) {
    s.append(Print(argument));
  }
  s.push_back(')');

  return s;
}

auto ASTPrinter::operator()(const GetExprPtr& expr) const -> std::string {
  return std::format("(. {} {})", Print(expr->GetObject()),
                     expr->GetProperty().GetLexeme());
}

auto ASTPrinter::operator()(const GroupingExprPtr& expr) const -> std::string {
  return Parenthesize("group", expr->GetExpr());
}

auto ASTPrinter::operator()(const LiteralExprPtr& expr) const -> std::string {
  return expr->GetValue().ToString();
}

auto ASTPrinter::operator()(const LogicalExprPtr& expr) const -> std::string {
  return Parenthesize(expr->GetOperator().GetLexeme(),
                      expr->GetLeftExpression(), expr->GetRightExpression());
}

auto ASTPrinter::operator()(const SetExprPtr& expr) const -> std::string {
  return std::format("(= {} {} {})", Print(expr->GetObject()),
                     expr->GetProperty().GetLexeme(), Print(expr->GetValue()));
}

auto ASTPrinter::operator()([[maybe_unused]] const ThisExprPtr& expr) const
    -> std::string {
  return "this";
}

auto ASTPrinter::operator()(const UnaryExprPtr& expr) const -> std::string {
  return Parenthesize(expr->GetOperator().GetLexeme(),
                      expr->GetRightExpression());
}

auto ASTPrinter::operator()(const VariableExprPtr& expr) const -> std::string {
  return expr->GetVariable().GetLexeme();
}

auto ASTPrinter::Parenthesize(std::string_view name, const ExprPtr& expr) const
    -> std::string {
  return std::format("({} {})", name, Print(expr));
}

auto ASTPrinter::Parenthesize(std::string_view name, const ExprPtr& expr1,
                              const ExprPtr& expr2) const -> std::string {
  return std::format("({} {} {})", name, Print(expr1), Print(expr2));
}

auto ASTPrinter::Parenthesize(std::string_view name, const ExprPtr& expr,
                              const StmtPtr& stmt) const -> std::string {
  return std::format("({} {} {})", name, Print(expr), Print(stmt));
}

auto ASTPrinter::Parenthesize(std::string_view name, const ExprPtr& expr,
                              const StmtPtr& stmt1, const StmtPtr& stmt2) const
    -> std::string {
  return std::format("({} {} {} {})", name, Print(expr), Print(stmt1),
                     Print(stmt2));
}
};  // namespace cclox
