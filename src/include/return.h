#ifndef RETURN_H_
#define RETURN_H_

#include <stdexcept>
#include <utility>

#include "object.h"

namespace cclox {
class Return : public std::runtime_error {
 public:
  explicit Return(std::optional<Object> value)
      : runtime_error(""), value_(std::move(value)) {}

  auto GetReturnValue() const noexcept -> const std::optional<Object>& {
    return value_;
  }

 private:
  std::optional<Object> value_;
};
}  // namespace cclox

#endif  // RETURN_H_
