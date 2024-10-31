#include "token.h"

#include <format>

namespace cclox {
auto Token::GetType() const noexcept -> TokenType {
  return type_;
}

auto Token::GetLexeme() const noexcept -> std::string {
  return lexeme_;
}

auto Token::GetLiteral() const -> Object {
  return literal_.value();
}

auto Token::GetLineNumber() const noexcept -> uint32_t {
  return line_number_;
}

auto Token::ToString() const -> std::string {
  std::string value = literal_ ? literal_->ToString() : "";
  return std::format("{} {} {}", TokenTypeToString(type_), lexeme_, value);
}

auto operator<<(std::ostream& os, const Token& token) -> std::ostream& {
  return os << token.ToString();
}

}  // namespace cclox
