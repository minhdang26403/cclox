#ifndef LOX_H_
#define LOX_H_

#include <string>

#include "token.h"

namespace cclox {
/**
 * @brief The main class for the Lox interpreter, responsible for running files,
 * handling interactive prompts (REPL), and reporting errors.
 */
class Lox {
 public:
  /**
   * @brief Runs the Lox interpreter from the specified source file.
   * @param path the path to the Lox script file to be executed.
   */
  auto RunFile(std::string_view path) -> void;

  /**
   * @brief Starts an interactive prompt (REPL) for the Lox interpreter.
   */
  auto RunPrompt() -> void;

  /**
   * @brief Reports an error with a message at a specific line number.
   * @param line_number the line number where the error occurred.
   * @param message the error message describing what went wrong.
   */
  static auto Error(uint32_t line_number, std::string_view message) -> void;

  /**
   * @brief Reports an error related to a token with a given message.
   * @param line_number the token where the error occurs
   * @param message the error message describing what went wrong.
   */
  static auto Error(const Token& token, std::string_view message) -> void;

 private:
  /**
   * @brief Executes the given Lox source code.
   * @param source the Lox source code to be executed.
   */
  auto Run(std::string source) -> void;

  /**
   * @brief Reports an error at a specific line with additional context.
   * @param line_number the line number where the error occurred.
   * @param where a string indicating where in the code the error occurred (if
   * applicable).
   * @param message the error message describing what went wrong.
   */
  static auto Report(uint32_t line_number, std::string_view where,
                     std::string_view message) -> void;

  // A flag indicating whether an error has occurred.
  static bool had_error;
};
}  // namespace cclox

#endif  // LOX_H_
