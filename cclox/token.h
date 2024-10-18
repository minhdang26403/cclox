#ifndef TOKEN_H_
#define TOKEN_H_

#include <iostream>
#include <optional>
#include <string>

#include "object.h"
#include "token_type.h"

namespace cclox {
class Token {
 public:
  Token(TokenType type, const std::string& lexeme,
        std::optional<Object> literal, uint32_t line_number)
      : type_(type),
        lexeme_(lexeme),
        literal_(std::move(literal)),
        line_number_(line_number) {}

  friend auto operator<<(std::ostream& os, const Token& token) -> std::ostream&;

 private:
  TokenType type_;
  std::string lexeme_;
  std::optional<Object> literal_;
  [[maybe_unused]] uint32_t line_number_;
};

}  // namespace cclox

#endif  // TOKEN_H_
