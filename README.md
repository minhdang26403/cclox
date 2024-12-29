# Tree-Walk Interpreter in C++

This repository contains an implementation of a tree-walk interpreter inspired by part II of *Crafting Interpreters* by Robert Nystrom. The language and interpreter are implemented using modern C++20 features, with an emphasis on clarity, modularity, and extensibility.

## Building the Project

To build the project, ensure you have `CMake` installed, along with a C++20-compatible compiler. Follow these steps:

1. Clone the repository and go to the project directory.

2. Create a build directory:
    ```bash
    mkdir build
    cd build
    ```

3. Run CMake:
    ```bash
    cmake ..
    ```

4. Build the project (cclox interpreter and unit tests):
    ```bash
    cmake --build .
    ```

5. Build the cclox interpreter only:
    ```bash
    make cclox
    ```

## Running the Project
After building the project, you can run the interpreter in two ways:
1. Start an interactive REPL (Read-Eval-Print Loop) session:
    ```bash
    bin/cclox
    ```

2. Execute a script file:
    ```bash
    bin/cclox [script]
    ```
Replace `[script]` with the path to your `.lox` script file to execute it using the interpreter.

## Running Tests
Tests are implemented using [GoogleTest](https://github.com/google/googletest). You can find all test cases in the `test/` directory. After building the project, you can run the tests using the generated test executable in the `build/bin` directory. For example:
```bash
cd build/bin
./interpreter_test
```

The `interpreter_test` executable runs all tests in the `test/` directory. To create your own tests, create a new directory for your test suite and add sample Lox programs and the expected output files from these programs.

## Status
The following features are currently implemented in the language:
- [x] Scanning
- [x] Representing Code
- [x] Parsing Expressions
- [x] Evaluating Expressions
- [x] Statements and State
- [x] Control Flow
- [x] Functions
- [x] Resolving and Binding
- [x] Classes
- [x] Inheritance

## Contributing
Feel free to open issues or submit pull requests if you have suggestions or find bugs. Contributions are always welcome!
