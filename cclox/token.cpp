#include "token.h"

#include <format>

namespace cclox {
auto Token::ToString() const -> std::string {
  std::string value = literal_ ? literal_->ToString() : "";
  return std::format("{} {} {}", TokenTypeToString(type_), lexeme_, value);
}

auto operator<<(std::ostream& os, const Token& token) -> std::ostream& {
  return os << token.ToString();
}

}  // namespace cclox
