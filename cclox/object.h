#ifndef OBJECT_H_
#define OBJECT_H_

#include <cstddef>
#include <optional>
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
   * @brief Defines the type of object, which can be boolean, null pointer,
   * integer, double, or string.
   */
  using ValueType =
      std::variant<bool, std::nullptr_t, int32_t, double, std::string>;

  /**
   * @brief Default constructor. Initializes the Object with the default value
   * of the first type in the variant.
   */
  Object() = default;

  /**
   * @brief Copy constructor. Initializes the Object with the value of another
   * Object.
   * @param other The Object to copy from.
   */
  Object(const Object& other) : value_(other.value_) {}

  /**
   * @brief Templated constructor for initializing an Object with a compatible
   * value.
   * @param value The value to be stored in the Object.
   */
  template<typename T>
    requires std::is_convertible_v<T, ValueType>
  Object(T&& value) : value_(std::forward<T>(value)) {}

  /**
   * @brief Checks if the Object holds a boolean value.
   * @return `true` if the Object holds a boolean, `false` otherwise.
   */
  auto IsBool() const noexcept -> bool {
    return std::holds_alternative<bool>(value_);
  }

  /**
   * @brief Checks if the Object holds a null pointer value.
   * @return `true` if the Object holds a null pointer, `false` otherwise.
   */
  auto IsNil() const noexcept -> bool {
    return std::holds_alternative<std::nullptr_t>(value_);
  }

  /**
   * @brief Checks if the Object holds an integer value.
   * @return `true` if the Object holds an integer, `false` otherwise.
   */
  auto IsInteger() const noexcept -> bool {
    return std::holds_alternative<int32_t>(value_);
  }

  /**
   * @brief Checks if the Object holds a double value.
   * @return `true` if the Object holds a double, `false` otherwise.
   */
  auto IsDouble() const noexcept -> bool {
    return std::holds_alternative<double>(value_);
  }

  /**
   * @brief Checks if the Object holds a string value.
   * @return `true` if the Object holds a string, `false` otherwise.
   */
  auto IsString() const noexcept -> bool {
    return std::holds_alternative<std::string>(value_);
  }

  /**
   * @brief Retrieves the stored value as a constant reference.
   * @return A constant reference to the stored value.
   */
  const ValueType& Value() const noexcept { return value_; }

  /**
   * @brief Attempts to retrieve the stored value as an integer.
   * @return An `std::optional<int32_t>` containing the value if it is an
   * integer or can be converted to an integer, otherwise `std::nullopt`.
   */
  auto AsInteger() const noexcept -> std::optional<int32_t>;

  /**
   * @brief Attempts to retrieve the stored value as a double.
   * @return An `std::optional<double>` containing the value if it is a double
   * or can be converted to a double, otherwise `std::nullopt`.
   */
  auto AsDouble() const noexcept -> std::optional<double>;

  /**
   * @brief Attempts to retrieve the stored value as a string.
   * @return An `std::optional<std::string>` containing the value if it is a
   * string, otherwise `std::nullopt`.
   */
  auto AsString() const noexcept -> std::optional<std::string>;

  /**
   * @brief Retrieves the stored value as a specific type.
   * @return The stored value as type `T`.
   * @throws `std::bad_variant_access` if the stored value cannot be accessed as
   * `T`.
   */
  template<typename T>
  auto Get() const -> T {
    return std::get<T>(value_);
  }

  /**
   * @brief Evaluates whether the stored value is truthy.
   *
   * - `bool`: Returns the boolean value.
   * - `nullptr_t`: Returns `false`.
   * - `int32_t`: Returns `false` if 0, otherwise `true`.
   * - `double`: Returns `false` if 0.0, otherwise `true`.
   * - `std::string`: Returns `false` if the string is empty, otherwise `true`.
   *
   * @return `true` if the stored value is truthy, `false` otherwise.
   */
  auto IsTruthy() const noexcept -> bool;

  /**
   * @brief Returns the string representation of the stored value.
   * @return a string representation of the stored value.
   */
  auto ToString() const -> std::string;

 private:
  // The value of the Object
  ValueType value_;
};
}  // namespace cclox

#endif  // OBJECT_H_
