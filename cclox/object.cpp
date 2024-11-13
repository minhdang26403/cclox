#include "object.h"

#include <sstream>

namespace cclox {
auto Object::ToString() const -> std::string { 
    return std::visit([](auto&& arg) -> std::string {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, std::nullptr_t>) {
        return "nil";
      }
      
      std::ostringstream oss;
      oss << arg;
      return oss.str();
    }, value_);
  }
}
