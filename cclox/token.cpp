#include "token.h"

#include <format>

namespace cclox {
auto operator<<(std::ostream& os, const Token& token) -> std::ostream& {
  // TODO(Dang): Revise this
  std::string value = token.literal_ ? token.literal_->ToString() : "";
  os << std::format("{} {} {}", TokenToString(token.type_), token.lexeme_,
                    value);
  return os;
}

}  // namespace cclox
