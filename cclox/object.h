#ifndef OBJECT_H_
#define OBJECT_H_

#include <cstddef>
#include <string>
#include <type_traits>
#include <variant>

namespace cclox {
/**
 * @brief Represents an abstract object that can holds different types of
 * values.
 */
class Object {
 public:
  /**
   * @brief Defines the type of object, which can be boolean, null pointer, integer,
   * double, or string.
   */
  using ObjectType =
      std::variant<bool, std::nullptr_t, int, double, std::string>;

  /**
   * @brief Constructs an Object with a value of type T. The type T must not be
   * the reference type.
   * @param value the value to be stored in the Object.
   */
  template<typename T, typename = std::enable_if_t<!std::is_reference_v<T>>>
  Object(T&& value) : value_(std::forward<T>(value)) {}

  /**
   * @brief Returns the string representation of the stored value.
   * @return a string representation of the stored value.
   */
  auto ToString() const -> std::string;

 private:
  // The value of the Object
  ObjectType value_;
};
}  // namespace cclox

#endif  // OBJECT_H_
