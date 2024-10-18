#ifndef SCANNER_H_
#define SCANNER_H_

#include <optional>
#include <string>
#include <vector>
#include <unordered_map>

#include "lox.h"
#include "token.h"
#include "token_type.h"

namespace cclox {
class Scanner {
 public:
  Scanner(const std::string& source) : source_(source) {}

  auto ScanTokens() -> std::vector<Token>;

 private:
  auto IsAtEnd() const -> bool;

  auto ScanToken() -> void;

  auto Identifier() -> void;

  auto Number() -> void;

  auto String() -> void;

  auto Match(char expected) -> bool;

  auto Peek() const -> char;

  auto PeekNext() const -> char;

  auto IsAlpha(char c) const -> bool;

  auto IsDigit(char c) const -> bool;

  auto IsAlphaNumeric(char c) -> bool;

  auto Advance() -> char;

  auto AddToken(TokenType type) -> void;

  auto AddToken(TokenType type, std::optional<Object> literal) -> void;

  static const std::unordered_map<std::string, TokenType> keywords;

  std::string source_;
  std::vector<Token> tokens_;
  uint32_t start_{};
  uint32_t current_{};
  uint32_t line_number_{1};
};
}  // namespace cclox

#endif  // SCANNER_H_
