#include "lox.h"

#include <sysexits.h>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

#include "scanner.h"

namespace cclox {

bool cclox::Lox::had_error = false;

auto Lox::RunFile(const std::string& filepath) -> void {
  std::ifstream file{filepath};
  std::stringstream buffer;
  buffer << file.rdbuf();
  Run(buffer.str());

  // Indicate an error in the exit code.
  if (had_error) {
    std::exit(EX_DATAERR);
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

// =========================Private Methods=========================

auto Lox::Error(uint32_t line_number, const std::string& message) -> void {
  Report(line_number, "", message);
}

auto Lox::Run(std::string source) -> void {
  Scanner scanner{source};
  std::vector<Token> tokens = scanner.ScanTokens();

  // TODO(Dang): remove
  // for (const auto& token : tokens) {
  //   std::cout << token << '\n';
  // }
}

auto Lox::Report(uint32_t line_number, const std::string& where,
                 const std::string& message) -> void {
  std::cout << std::format("[line {}] Error{}: {}\n", line_number, where,
                           message);
}
}  // namespace cclox
