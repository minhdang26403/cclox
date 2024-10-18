#ifndef OBJECT_H_
#define OBJECT_H_

#include <string>
#include <variant>

namespace cclox {
class Object {
 public:
  using ObjectType = std::variant<int, double, std::string>;

  template<typename T>
  Object(const T& value) : value_(value) {}

  auto ToString() const -> std::string;

 private:
  ObjectType value_;
};
}  // namespace cclox

#endif  // OBJECT_H_
