#ifndef EXPR_H_
#define EXPR_H_

#include <memory>

#include "token.h"

namespace cclox {
// Forward declaration.
class Binary;
class Grouping;
class Literal;
class Unary;

// Must use pointers to avoid cyclic dependencies between these objects and Expr
// object
using BinaryPtr = std::unique_ptr<Binary>;
using GroupingPtr = std::unique_ptr<Grouping>;
using LiteralPtr = std::unique_ptr<Literal>;
using UnaryPtr = std::unique_ptr<Unary>;

using ExprPtr = std::variant<BinaryPtr, GroupingPtr, LiteralPtr, UnaryPtr>;

class Binary {
 public:
  /**
   * @brief Constructs a Binary expression.
   * @param left The left operand expression.
   * @param op The binary operator token.
   * @param right The right operand expression.
   */
  Binary(ExprPtr left, Token op, ExprPtr right)
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
  auto GetLeftExpr() const noexcept -> const ExprPtr& { return left_; }

  /**
   * @brief Gets the right expression of the binary expression.
   * @return a const reference to the right expression object.
   */
  auto GetRightExpr() const noexcept -> const ExprPtr& { return right_; }

 private:
  ExprPtr left_;
  Token op_;
  ExprPtr right_;
};

class Grouping {
 public:
  /**
   * @brief Constructs a Grouping expression.
   * @param expression The inner expression being grouped.
   */
  Grouping(ExprPtr expression) : expression_(std::move(expression)) {}

  /**
   * @brief Gets the expression being grouped.
   * @return a const reference to the inner expression object.
   */
  auto GetExpr() const noexcept -> const ExprPtr& { return expression_; }

 private:
  ExprPtr expression_;
};

class Literal {
 public:
  /**
   * @brief Constructs a Literal expression.
   * @param value The value of the literal, stored as an Object.
   */
  Literal(Object value) : value_(std::move(value)) {}

  /**
   * @brief Gets the value of a literal.
   * @return a const reference to the value object.
   */
  auto GetValue() const noexcept -> const Object& { return value_; }

 private:
  Object value_;
};

class Unary {
 public:
  /**
   * @brief Constructs a Unary expression.
   * @param op The unary operator token.
   * @param right The operand expression.
   */
  Unary(Token op, ExprPtr right)
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

// TODO(Dang): may not need this
template<typename T>
struct ExprVisitor {
  auto operator()(const BinaryPtr& expr) -> T;
  auto operator()(const GroupingPtr& expr) -> T;
  auto operator()(const LiteralPtr& expr) -> T;
  auto operator()(const UnaryPtr& expr) -> T;
};

}  // namespace cclox

#endif