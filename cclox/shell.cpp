/*
  dir2/foo2.h.
  A blank line
  C system files.
  C++ system files.
  A blank line
  Other libraries' .h files.
  Your project's .h files.
*/

#include <sysexits.h>
#include <iostream>

#include "lox.h"

auto main(int argc, char* argv[]) -> int {
  if (argc > 2) {
    std::cout << "Usage: cclox [script]\n";
    std::exit(EX_USAGE);
  }

  cclox::Lox lox;
  if (argc == 2) {
    lox.RunFile(argv[1]);
  } else {
    lox.RunPrompt();
  }

  return 0;
}
