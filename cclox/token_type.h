#ifndef TOKEN_TYPE_H_
#define TOKEN_TYPE_H_

#include <array>

namespace cclox {
// clang-format off
enum class TokenType {
  // Single-character tokens.
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

  // One or two character tokens.
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,

  // Literals.
  IDENTIFIER, STRING, NUMBER,

  // Keywords.
  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

  // stdio.h already defines EOF, so can't use it anymore.
  EoF,

  // Keep track of the number of enum items.
  COUNT
};

// Create a constexpr array for string representation.
constexpr std::array<const char*, static_cast<std::size_t>(TokenType::COUNT)>
  token_strings = {
    "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",
    "COMMA", "DOT", "MINUS", "PLUS", "SEMICOLON", "SLASH", "STAR",
    "BANG", "BANG_EQUAL",
    "EQUAL", "EQUAL_EQUAL",
    "GREATER", "GREATER_EQUAL",
    "LESS", "LESS_EQUAL",
    "IDENTIFIER", "STRING", "NUMBER",
    "AND", "CLASS", "ELSE", "FALSE", "FUN", "FOR", "IF", "NIL", "OR",
    "PRINT", "RETURN", "SUPER", "THIS", "TRUE", "VAR", "WHILE",
    "EoF"
};

// clang-format on

// Function to convert enum to string.
constexpr auto TokenTypeToString(TokenType token) -> const char* {
  return token_strings[static_cast<std::size_t>(token)];
}

}  // namespace cclox

#endif  // TOKEN_TYPE_H_
