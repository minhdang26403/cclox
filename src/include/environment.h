#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "object.h"
#include "token.h"

namespace cclox {
class Environment : public std::enable_shared_from_this<Environment> {
 public:
  // Ensure that client code cannot directly call the constructor and can only
  // create instances of Environment as std::shared_ptr to support the usage of
  // shared_from_this.
  static auto Create() -> std::shared_ptr<Environment>;

  static auto Create(const std::shared_ptr<Environment>& enclosing)
      -> std::shared_ptr<Environment>;

  auto Get(const Token& variable) const -> Object;

  auto GetAt(uint64_t distance, const Token& variable) -> Object;

  auto Define(const std::string& name, const Object& value) -> void;

  auto Assign(const Token& variable, const Object& value) -> void;

  auto AssignAt(uint64_t distance, const Token& variable, const Object& value)
      -> void;

  auto GetEnclosingEnvironment() const noexcept
      -> const std::shared_ptr<Environment>&;

 private:
  Environment() = default;

  explicit Environment(const std::shared_ptr<Environment>& enclosing)
      : enclosing_(enclosing) {}

  auto Ancestor(uint64_t distance) -> std::shared_ptr<Environment>;

  std::unordered_map<std::string, Object> values_;
  std::shared_ptr<Environment> enclosing_;
};
}  // namespace cclox

#endif  // ENVIRONMENT_H_
