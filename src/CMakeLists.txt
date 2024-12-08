# Define the library
add_library(lox
  ast_printer.cpp
  environment.cpp
  lox.cpp
  lox_function.cpp
  interpreter.cpp
  object.cpp
  parser.cpp
  scanner.cpp
  token.cpp)

# Define the executable
add_executable(cclox shell.cpp)

# Link the executable with the lox library
target_link_libraries(cclox PRIVATE lox)
