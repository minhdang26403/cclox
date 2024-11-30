#ifndef LOX_H_
#define LOX_H_

#include <string>

#include "interpreter.h"
#include "token.h"

namespace cclox {
/**
 * @brief The main class for the Lox interpreter, responsible for running files,
 * handling interactive prompts (REPL), and reporting errors.
 */
class Lox {
 public:
  Lox() { ResetLoxInterpreterState(); }

  Lox(std::ostream& output) : output_(output), interpreter_(output_) {
    ResetLoxInterpreterState();
  }

  /**
   * @brief Runs the Lox interpreter from the specified source file.
   * @param path The path to the Lox script file to be executed.
   */
  auto RunFile(std::string_view path) -> void;

  /**
   * @brief Starts an interactive prompt (REPL) for the Lox interpreter.
   */
  auto RunPrompt() -> void;

  /**
   * @brief Reports an error with a message at a specific line number.
   * @param output The output stream.
   * @param line_number The line number where the error occurred.
   * @param message The error message describing what went wrong.
   */
  static auto Error(std::ostream& output, uint32_t line_number,
                    std::string_view message) -> void;

  /**
   * @brief Reports an error related to a token with a given message.
   * @param output The output stream.
   * @param line_number The token where the error occurs.
   * @param message The error message describing what went wrong.
   */
  static auto Error(std::ostream& output, const Token& token,
                    std::string_view message) -> void;

  /**
   * Reports a runtime error to standard output and sets the error flag.
   * Prints the error message followed by the line number where the error
   * occurred.
   * @param output The output stream.
   * @param error The runtime error containing the error message and token
   * information.
   */
  static auto ReportRuntimeError(std::ostream& output,
                                 const RuntimeError& error) -> void;

 private:
  /**
   * @brief Executes the given Lox source code.
   * @param source The Lox source code to be executed.
   */
  auto Run(std::string source) -> void;

  auto ResetLoxInterpreterState() noexcept -> void;

  /**
   * @brief Reports an error at a specific line with additional context.
   * @param output The output stream.
   * @param line_number The line number where the error occurred.
   * @param where A string indicating where in the code the error occurred (if
   * applicable).
   * @param message The error message describing what went wrong.
   */
  static auto Report(std::ostream& output, uint32_t line_number,
                     std::string_view where, std::string_view message) -> void;

  std::ostream& output_{std::cout};
  Interpreter interpreter_;
  // A flag indicating whether an error has occurred.
  static bool had_error;
  static bool had_runtime_error;
};
}  // namespace cclox

#endif  // LOX_H_
