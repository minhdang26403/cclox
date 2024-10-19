#ifndef TOKEN_H_
#define TOKEN_H_

#include <iostream>
#include <optional>
#include <string>

#include "object.h"
#include "token_type.h"

namespace cclox {
/**
 * @brief Represents a token in the Lox programming language.
 */
class Token {
 public:
  /**
   * @brief Constructs a Token object.
   * @param type the type of the token (e.g., keyword, identifier, symbol).
   * @param lexeme the textual representation of the token.
   * @param literal the optional literal value associated with the token, if
   * any.
   * @param line_number the line number in the source code where the token
   * appears.
   */
  Token(TokenType type, std::string lexeme, std::optional<Object> literal,
        uint32_t line_number)
      : type_(type),
        lexeme_(std::move(lexeme)),
        literal_(std::move(literal)),
        line_number_(line_number) {}

  /**
   * @brief Returns a string representation of the token. This method converts
   * the token's type, lexeme, and literal value into a human-readable string
   * format.
   * @return a string representing the token.
   */
  auto ToString() const -> std::string;

  /**
   * @brief Enables printing of a Token object using the `<<` operator.
   * @param os the output stream to write to.
   * @param token the token object to be printed.
   * @return a reference to the output stream.
   */
  friend auto operator<<(std::ostream& os, const Token& token) -> std::ostream&;

 private:
  // The type of the token (e.g., identifier, keyword).
  TokenType type_;
  // The textual representation of the token.
  std::string lexeme_;
  // The literal value associated with the token, if any.
  std::optional<Object> literal_;
  // The line number where the token was found.
  [[maybe_unused]] uint32_t line_number_;
};

}  // namespace cclox

#endif  // TOKEN_H_
