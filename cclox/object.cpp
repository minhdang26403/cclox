#include "object.h"

#include <sstream>

namespace cclox {
auto Object::ToString() const -> std::string { 
    return std::visit([](auto&& arg) -> std::string {
      std::ostringstream oss;
      oss << arg;
      return oss.str();
    }, value_);
  }
}
