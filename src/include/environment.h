#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <unordered_map>

#include "object.h"
#include "token.h"

namespace cclox {
class Environment {
 public:
  Environment() = default;

  /**
   * @brief Construct a new Environment object given a pointer to the outer
   * environment.
   * @param enclosing The enclosing environment.
   */
  Environment(std::shared_ptr<Environment> enclosing)
      : enclosing_(std::move(enclosing)) {}

  auto Get(const Token& variable) const -> Object;

  auto Define(const std::string& name, const Object& value) -> void;

  auto Assign(const Token& variable, const Object& value) -> void;

 private:
  std::unordered_map<std::string, Object> values_;
  std::shared_ptr<Environment> enclosing_;
};
}  // namespace cclox

#endif  // ENVIRONMENT_H_