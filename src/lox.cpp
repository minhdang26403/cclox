#include "lox.h"

#include <sysexits.h>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

#include "ast_printer.h"
#include "interpreter.h"
#include "parser.h"
#include "scanner.h"
#include "stmt.h"
#include "token_type.h"

namespace cclox {
bool Lox::had_error = false;
bool Lox::had_runtime_error = false;

auto Lox::RunFile(std::string_view path) -> void {
  std::ifstream file{path.data()};

  // Check if the file was opened successfully.
  if (!file.is_open()) {
    std::cerr << "Error: Unable to open file: " << path << std::endl;
    std::exit(EX_NOINPUT);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  // Check if reading the file was successful.
  if (file.fail() && !file.eof()) {
    std::cerr << "Error: Failed to read from file: " << path << std::endl;
    std::exit(EX_IOERR);
  }

  Run(buffer.str());

  // Indicate an error in the exit code.
  if (had_error) {
    std::exit(EX_DATAERR);
  }
  if (had_runtime_error) {
    std::exit(EX_SOFTWARE);
  }
}

auto Lox::RunPrompt() -> void {
  if (&output_ != &std::cout) {
    std::cerr << "Error: The Lox REPL must be run with the standard output "
                 "stream (std::cout)\n";
    std::exit(EX_USAGE);
  }
  std::string line;
  while (true) {
    std::cout << "> ";
    if (!std::getline(std::cin, line)) {
      break;
    }
    Run(std::move(line));
    // The string `line` is left in a valid but unspecified state after move, so
    // `clear()` to reset the string state to avoid clang-tidy warning.
    line.clear();
    // Reset this flag in the interactive loop. If the user makes a mistake,
    // it shouldn't kill their entire session
    had_error = false;
  }
}

auto Lox::Error(std::ostream& output, uint32_t line_number,
                std::string_view message) -> void {
  Report(output, line_number, "", message);
}

auto Lox::Error(std::ostream& output, const Token& token,
                std::string_view message) -> void {
  if (token.GetType() == TokenType::EoF) {
    Report(output, token.GetLineNumber(), " at end", message);
  } else {
    Report(output, token.GetLineNumber(),
           std::format(" at '{}'", token.GetLexeme()), message);
  }
}

auto Lox::ReportRuntimeError(std::ostream& output, const RuntimeError& error)
    -> void {
  output << std::format("{}\n[line {}]\n", error.what(),
                        error.token_.GetLineNumber());
  had_runtime_error = true;
}

// =========================Private Methods=========================

auto Lox::Run(std::string source) -> void {
  Scanner scanner{std::move(source), output_};
  std::vector<Token> tokens = scanner.ScanTokens();
  // Stop if there was a lexing error.
  if (had_error) {
    return;
  }

  Parser parser{std::move(tokens), output_};
  std::vector<StmtPtr> statements = parser.Parse();
  // Stop if there was a parsing error.
  if (had_error) {
    return;
  }

  interpreter_.Interpret(statements);
}

auto Lox::ResetLoxInterpreterState() noexcept -> void {
  // Since `had_error` and `had_runtime_error` are static variables, their
  // lifetime is the program's lifetime. Thus, we have to reset these variables
  // every time we create a new Lox instance.
  had_error = false;
  had_runtime_error = false;
}

auto Lox::Report(std::ostream& output, uint32_t line_number,
                 std::string_view where, std::string_view message) -> void {
  output << std::format("[line {}] Error{}: {}\n", line_number, where, message);
  had_error = true;
}
}  // namespace cclox
