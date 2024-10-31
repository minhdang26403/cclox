# Define the library
add_library(lox
  lox.cpp
  object.cpp
  parser.cpp
  scanner.cpp
  token.cpp)

# Define the executable
add_executable(cclox shell.cpp)

# Link the executable with the lox library
target_link_libraries(cclox PRIVATE lox)
