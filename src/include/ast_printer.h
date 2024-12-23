#ifndef AST_PRINTER_H_
#define AST_PRINTER_H_

#include <string>

#include "expr.h"
#include "stmt.h"

namespace cclox {
/**
 * @brief A visitor class that converts Abstract Syntax Tree (AST) nodes to
 * their string representation.
 *
 * This class implements the visitor pattern using std::variant to traverse and
 * print different types of expressions and statements in the AST. Each
 * operator() overload handles a specific type of expression node and statement
 * node and formats it according to a Lisp-like prefix notation.
 */
class ASTPrinter {
 public:
  // ====================AST Printer for Statements====================
  auto Print(const StmtPtr& stmt) const -> std::string;

  auto operator()(const BlockStmtPtr& block_stmt) const -> std::string;

  auto operator()(const ClassStmtPtr&) const -> std::string;

  auto operator()(const ExprStmtPtr& stmt) const -> std::string;

  auto operator()(const FunctionStmtPtr& stmt) const -> std::string;

  auto operator()(const IfStmtPtr& stmt) const -> std::string;

  auto operator()(const PrintStmtPtr& stmt) const -> std::string;

  auto operator()(const ReturnStmtPtr& stmt) const -> std::string;

  auto operator()(const VarStmtPtr& stmt) const -> std::string;

  auto operator()(const WhileStmtPtr& stmt) const -> std::string;

  // ====================AST Printer for Expressions====================
  /**
   * @brief Prints the abstract syntax tree.
   * @param expr the top expression of the AST.
   * @return the string representation of the AST.
   */
  auto Print(const ExprPtr& expr) const -> std::string;

  auto operator()(const AssignExprPtr& expr) const -> std::string;

  /**
   * @brief Formats a binary expression node.
   * @param expr Shared pointer to the binary expression.
   * @return String representation in the format "(operator left_expr
   * right_expr)".
   */
  auto operator()(const BinaryExprPtr& expr) const -> std::string;

  auto operator()(const CallExprPtr& expr) const -> std::string;

  auto operator()(const GetExprPtr& expr) const -> std::string;

  /**
   * @brief Formats a grouping expression node.
   * @param expr Shared pointer to the grouping expression.
   * @return String representation in the format "(group expr)".
   */
  auto operator()(const GroupingExprPtr& expr) const -> std::string;

  /**
   * @brief Formats a literal expression node.
   * @param expr Shared pointer to the literal expression.
   * @return String representation of the literal value.
   */
  auto operator()(const LiteralExprPtr& expr) const -> std::string;

  auto operator()(const LogicalExprPtr& expr) const -> std::string;

  auto operator()(const SetExprPtr& expr) const -> std::string;

  auto operator()(const ThisExprPtr& expr) const -> std::string;

  /**
   * @brief Formats a unary expression node.
   * @param expr Shared pointer to the unary expression.
   * @return String representation in the format "(operator expr)".
   */
  auto operator()(const UnaryExprPtr& expr) const -> std::string;

  auto operator()(const VariableExprPtr& expr) const -> std::string;

 private:
  /**
   * @brief Helper function to format an expression with one operand.
   * @param name The operator or node type name.
   * @param expr The operand expression.
   * @return Formatted string with parentheses: "(name expr)".
   */
  auto Parenthesize(std::string_view name, const ExprPtr& expr) const
      -> std::string;

  /**
   * @brief Helper function to format an expression with two operands.
   * @param name The operator or node type name.
   * @param expr1 The first operand expression.
   * @param expr2 The second operand expression.
   * @return Formatted string with parentheses: "(name expr1 expr2)".
   */
  auto Parenthesize(std::string_view name, const ExprPtr& expr1,
                    const ExprPtr& expr2) const -> std::string;

  auto Parenthesize(std::string_view name, const ExprPtr& expr,
                    const StmtPtr& stmt) const -> std::string;

  auto Parenthesize(std::string_view name, const ExprPtr& expr,
                    const StmtPtr& stmt1, const StmtPtr& stmt2) const
      -> std::string;
};
}  // namespace cclox

#endif  // AST_PRINTER_H_
