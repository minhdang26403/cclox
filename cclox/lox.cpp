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
#include "token_type.h"

namespace cclox {
bool Lox::had_error = false;
bool Lox::had_runtime_error = false;

auto Lox::RunFile(std::string_view path) -> void {
  std::ifstream file{path};
  std::stringstream buffer;
  buffer << file.rdbuf();
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
  std::string line;
  while (true) {
    std::cout << "> ";
    if (!std::getline(std::cin, line)) {
      break;
    }
    Run(std::move(line));
    // Reset this flag in the interactive loop. If the user makes a mistake,
    // it shouldn't kill their entire session
    had_error = false;
  }
}

auto Lox::Error(uint32_t line_number, std::string_view message) -> void {
  Report(line_number, "", message);
}

auto Lox::Error(const Token& token, std::string_view message) -> void {
  if (token.GetType() == TokenType::EoF) {
    Report(token.GetLineNumber(), " at end", message);
  } else {
    Report(token.GetLineNumber(), std::format(" at '{}'", token.GetLexeme()),
           message);
  }
}

auto Lox::ReportRuntimeError(const RuntimeError& error) -> void {
  std::cout << std::format("{}\n[line {}]\n", error.what(),
                           error.token_.GetLineNumber());
  had_runtime_error = true;
}

// =========================Private Methods=========================

auto Lox::Run(std::string source) -> void {
  Scanner scanner{std::move(source)};
  std::vector<Token> tokens = scanner.ScanTokens();
  // Stop if there was a lexing error
  if (had_error) {
    return;
  }

  Parser parser{std::move(tokens)};
  ExprPtr expression = parser.Parse();
  // Stop if there was a parsing error.
  if (had_error) {
    return;
  }

  interpreter_.Interpret(expression);
}

auto Lox::Report(uint32_t line_number, std::string_view where,
                 std::string_view message) -> void {
  std::cout << std::format("[line {}] Error{}: {}\n", line_number, where,
                           message);
  had_error = true;
}
}  // namespace cclox
