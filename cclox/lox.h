#ifndef LOX_H_
#define LOX_H_

#include <string>

namespace cclox {
class Lox {
 public:
  auto RunFile(const std::string& filepath) -> void;

  auto RunPrompt() -> void;

  static auto Error(uint32_t line_number, const std::string& message) -> void;

 private:
  auto Run(std::string source) -> void;

  static auto Report(uint32_t line_number, const std::string& where,
                     const std::string& message) -> void;

  static bool had_error;
};
}  // namespace cclox

#endif  // LOX_H_
