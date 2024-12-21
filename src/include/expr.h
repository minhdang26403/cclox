#ifndef EXPR_H_
#define EXPR_H_

#include <memory>
#include <utility>
#include <vector>

#include "token.h"

namespace cclox {
// Forward declaration.
class AssignExpr;
class BinaryExpr;
class CallExpr;
class GroupingExpr;
class LiteralExpr;
class LogicalExpr;
class UnaryExpr;
class VariableExpr;

// Use pointers to remove cyclic dependencies between these objects and `Expr`
// object
using AssignExprPtr = std::shared_ptr<AssignExpr>;
using BinaryExprPtr = std::shared_ptr<BinaryExpr>;
using CallExprPtr = std::shared_ptr<CallExpr>;
using GroupingExprPtr = std::shared_ptr<GroupingExpr>;
using LiteralExprPtr = std::shared_ptr<LiteralExpr>;
using LogicalExprPtr = std::shared_ptr<LogicalExpr>;
using UnaryExprPtr = std::shared_ptr<UnaryExpr>;
using VariableExprPtr = std::shared_ptr<VariableExpr>;

using ExprPtr =
    std::variant<AssignExprPtr, BinaryExprPtr, CallExprPtr, GroupingExprPtr,
                 LiteralExprPtr, LogicalExprPtr, UnaryExprPtr, VariableExprPtr>;

class AssignExpr {
 public:
  AssignExpr(Token variable, ExprPtr value)
      : variable_(std::move(variable)), value_(std::move(value)) {}

  auto GetVariable() const noexcept -> const Token& { return variable_; }

  auto GetValue() const noexcept -> const ExprPtr& { return value_; }

 private:
  Token variable_;
  ExprPtr value_;
};

class BinaryExpr {
 public:
  /**
   * @brief Constructs a BinaryExpr expression.
   * @param left The left operand expression.
   * @param op The binary operator token.
   * @param right The right operand expression.
   */
  BinaryExpr(ExprPtr left, Token op, ExprPtr right)
      : left_(std::move(left)), op_(std::move(op)), right_(std::move(right)) {}

  /**
   * @brief Gets the operator of the binary expression.
   * @return a const reference to the operator object.
   */
  auto GetOperator() const noexcept -> const Token& { return op_; }

  /**
   * @brief Gets the left expression of the binary expression.
   * @return a const reference to the left expression object.
   */
  auto GetLeftExpression() const noexcept -> const ExprPtr& { return left_; }

  /**
   * @brief Gets the right expression of the binary expression.
   * @return a const reference to the right expression object.
   */
  auto GetRightExpression() const noexcept -> const ExprPtr& { return right_; }

 private:
  ExprPtr left_;
  Token op_;
  ExprPtr right_;
};

class CallExpr {
 public:
  CallExpr(ExprPtr callee, Token paren, std::vector<ExprPtr> arguments)
      : callee_(std::move(callee)),
        paren_(std::move(paren)),
        arguments_(std::move(arguments)) {}

  auto GetCallee() const noexcept -> const ExprPtr& { return callee_; }

  auto GetParen() const noexcept -> const Token& { return paren_; }

  auto GetArguments() const noexcept -> const std::vector<ExprPtr>& {
    return arguments_;
  }

 private:
  ExprPtr callee_;
  Token paren_;
  std::vector<ExprPtr> arguments_;
};

class GroupingExpr {
 public:
  /**
   * @brief Constructs a GroupingExpr expression.
   * @param expression The inner expression being grouped.
   */
  explicit GroupingExpr(ExprPtr expression)
      : expression_(std::move(expression)) {}

  /**
   * @brief Gets the expression being grouped.
   * @return a const reference to the inner expression object.
   */
  auto GetExpr() const noexcept -> const ExprPtr& { return expression_; }

 private:
  ExprPtr expression_;
};

class LiteralExpr {
 public:
  /**
   * @brief Constructs a LiteralExpr expression.
   * @param value The value of the literal, stored as an Object.
   */
  explicit LiteralExpr(Object value) : value_(std::move(value)) {}

  /**
   * @brief Gets the value of a literal.
   * @return a const reference to the value object.
   */
  auto GetValue() const noexcept -> const Object& { return value_; }

 private:
  Object value_;
};

class LogicalExpr {
 public:
  LogicalExpr(ExprPtr left, Token op, ExprPtr right)
      : left_(std::move(left)), op_(std::move(op)), right_(std::move(right)) {}

  auto GetOperator() const noexcept -> const Token& { return op_; }

  auto GetLeftExpression() const noexcept -> const ExprPtr& { return left_; }

  auto GetRightExpression() const noexcept -> const ExprPtr& { return right_; }

 private:
  ExprPtr left_;
  Token op_;
  ExprPtr right_;
};

class UnaryExpr {
 public:
  /**
   * @brief Constructs a UnaryExpr expression.
   * @param op The unary operator token.
   * @param right The operand expression.
   */
  UnaryExpr(Token op, ExprPtr right)
      : op_(std::move(op)), right_(std::move(right)) {}

  /**
   * @brief Gets the operator of the unary expression.
   * @return a const reference to the operator object.
   */
  auto GetOperator() const noexcept -> const Token& { return op_; }

  /**
   * @brief Gets the right expression of the unary expression.
   * @return a const reference to the right expression object.
   */
  auto GetRightExpression() const noexcept -> const ExprPtr& { return right_; }

 private:
  Token op_;
  ExprPtr right_;
};

class VariableExpr {
 public:
  explicit VariableExpr(Token variable) : variable_(std::move(variable)) {}

  auto GetVariable() const noexcept -> const Token& { return variable_; }

 private:
  Token variable_;
};

}  // namespace cclox

#endif
