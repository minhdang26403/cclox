#ifndef TOKEN_H_
#define TOKEN_H_

#include <iostream>
#include <optional>
#include <string>
#include <utility>

#include "object.h"
#include "token_type.h"

namespace cclox {
/**
 * @brief Represents a token in the Lox programming language.
 */
class Token {
 public:
  Token(TokenType type, std::string lexeme)
      : type_(type), lexeme_(std::move(lexeme)) {}

  /**
   * @brief Constructs a Token object.
   * @param type The type of the token (e.g., keyword, identifier, symbol).
   * @param lexeme The textual representation of the token.
   * @param literal The optional literal value associated with the token, if
   * any.
   * @param line_number The line number in the source code where the token
   * appears.
   */
  Token(TokenType type, std::string lexeme, std::optional<Object> literal,
        uint32_t line_number)
      : type_(type),
        lexeme_(std::move(lexeme)),
        literal_(std::move(literal)),
        line_number_(line_number) {}

  /**
   * @brief Gets the type of this token.
   * @return The token type.
   */
  auto GetType() const noexcept -> TokenType;

  /**
   * @brief Gets the lexeme of this token.
   * @return The lexeme.
   */
  auto GetLexeme() const noexcept -> const std::string&;

  /**
   * @brief Gets the literal value of this token.
   * @return The literal value.
   */
  auto GetLiteral() const noexcept -> const Object&;

  /**
   * @brief Gets the line number of this token.
   * @return The line number.
   */
  auto GetLineNumber() const noexcept -> uint32_t;

  /**
   * @brief Returns a string representation of the token. This method converts
   * the token's type, lexeme, and literal value into a human-readable string
   * format.
   * @return A string representing the token.
   */
  auto ToString() const -> std::string;

  /**
   * @brief Enables printing of a Token object using the `<<` operator.
   * @param os The output stream to write to.
   * @param token The token object to be printed.
   * @return A reference to the output stream.
   */
  friend auto operator<<(std::ostream& os, const Token& token) -> std::ostream&;

 private:
  // The type of the token (e.g., identifier, keyword).
  TokenType type_;
  // The textual representation of the token.
  std::string lexeme_;
  // The literal value associated with the token, if any.
  std::optional<Object> literal_{std::nullopt};
  // The line number where the token was found.
  uint32_t line_number_{};
};

}  // namespace cclox

#endif  // TOKEN_H_
