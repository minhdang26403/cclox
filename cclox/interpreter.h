#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <format>
#include <stdexcept>

#include "expr.h"
#include "object.h"

namespace cclox {
/**
 * @brief Exception class for runtime errors during interpretation.
 */
class RuntimeError : public std::runtime_error {
 public:
  /**
   * @brief Constructs a RuntimeError with a token and error message.
   * @param token The token where the runtime error occurred.
   * @param message The error message describing the cause of the runtime error.
   */
  explicit RuntimeError(const Token& token, std::string_view message)
      : std::runtime_error(std::format("RuntimeError: {}", message)),
        token_(token) {}

  // The token where the runtime error occurred.
  Token token_;
};

/**
 * @brief Interpreter class that evaluates and executes expressions.
 */
class Interpreter {
 public:
  /**
   * @brief Evaluates an expression and prints its result.
   * @param expr The expression to interpret.
   */
  auto Interpret(const ExprPtr& expr) const -> void;

  /**
   * @brief Helper function to evaluate an expression.
   * @param expr The expression to evaluate.
   * @return The result of the evaluation.
   */
  auto Evaluate(const ExprPtr& expr) const -> Object;

  /**
   * @brief Evaluates a binary expression (e.g., a + b, a > b).
   * @param expr The binary expression to evaluate.
   * @return The result of evaluating the expression.
   */
  auto operator()(const BinaryExprPtr& expr) const -> Object;

  /**
   * @brief Evaluates a grouping expression (expressions in parentheses).
   * @param expr The grouping expression to evaluate.
   * @return The result of evaluating the contained expression.
   */
  auto operator()(const GroupingExprPtr& expr) const -> Object;

  /**
   * @brief Evaluates a literal value (numbers, strings, booleans, null).
   * @param expr The literal expression to evaluate.
   * @return The literal value wrapped in an Object.
   */
  auto operator()(const LiteralExprPtr& expr) const -> Object;

  /**
   * @brief Evaluates a unary expression (e.g., -a, !b).
   * @param expr The unary expression to evaluate.
   * @return The result of applying the unary operator.
   */
  auto operator()(const UnaryExprPtr& expr) const -> Object;

 private:
  /**
   * @brief Tests equality between two Objects.
   * @param left Left operand of the equality comparison.
   * @param right Right operand of the equality comparison.
   * @return true if objects are equal, false otherwise.
   */
  auto Equal(const Object& left, const Object& right) const -> bool;

  /**
   * @brief Tests if left operand is greater than right operand.
   * @param left Left operand.
   * @param op The operator token for error reporting.
   * @param right Right operand.
   * @return true if left > right, false otherwise.
   */
  auto Greater(const Object& left, const Token& op, const Object& right) const
      -> bool;

  /**
   * @brief Tests if left operand is less than right operand.
   * @param left Left operand.
   * @param op The operator token for error reporting.
   * @param right Right operand.
   * @return true if left < right, false otherwise.
   */
  auto Less(const Object& left, const Token& op, const Object& right) const
      -> bool;

  /**
   * @brief Performs subtraction between two Objects.
   * @param left Left operand.
   * @param op The operator token for error reporting.
   * @param right Right operand.
   * @return Result of subtraction.
   */
  auto Subtract(const Object& left, const Token& op, const Object& right) const
      -> Object;

  /**
   * @brief Performs addition between two Objects.
   * @param left Left operand.
   * @param op The operator token for error reporting.
   * @param right Right operand.
   * @return Result of addition.
   */
  auto Add(const Object& left, const Token& op, const Object& right) const
      -> Object;

  /**
   * @brief Performs division between two Objects.
   * @param left Left operand.
   * @param op The operator token for error reporting.
   * @param right Right operand.
   * @return Result of division.
   */
  auto Divide(const Object& left, const Token& op, const Object& right) const
      -> Object;

  /**
   * @brief Performs multiplication between two Objects.
   * @param left Left operand.
   * @param op The operator token for error reporting.
   * @param right Right operand.
   * @return Result of multiplication.
   */
  auto Multiply(const Object& left, const Token& op, const Object& right) const
      -> Object;

  /**
   * @brief Helper function to extract numeric values from Objects.
   * @param left Left operand.
   * @param op The operator token for error reporting.
   * @param right Right operand.
   * @return Pair of doubles representing the numeric values.
   */
  auto GetNumberOperands(const Object& left, const Token& op,
                         const Object& right) const
      -> std::pair<double, double>;
};
}  // namespace cclox

#endif  // INTERPRETER_H_
