#include <gtest/gtest.h>
#include <sysexits.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

#include "lox.h"

namespace fs = std::filesystem;

std::string ReadFile(const std::string& filepath) {
  std::ifstream file{filepath};
  if (!file.is_open()) {
    std::cerr << "Error: Unable to open file: " << filepath << '\n';
    std::exit(EX_NOINPUT);
  }
  std::ostringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

// Parameterized test class for handling directories of test cases
class InterpreterTest : public ::testing::TestWithParam<std::string> {
 protected:
  void RunTestFromFile(const std::string& input_file_path,
                       const std::string& expected_output_path) {
    std::string expected_output = ReadFile(expected_output_path);

    // The custom output stream, which will be used to compare with the expected
    // output.
    std::ostringstream output;
    cclox::Lox lox{output};

    lox.RunFile(input_file_path);
    EXPECT_EQ(output.str(), expected_output);
  }
};

// Collect all `.lox` files and their corresponding `.txt` files in the
// directory
std::vector<std::string> CollectTestFiles(
    const std::vector<std::string>& directories) {
  std::vector<std::string> test_files;
  for (const auto& directory : directories) {
    for (const auto& entry : fs::directory_iterator{directory}) {
      if (entry.path().extension() == ".lox") {
        test_files.push_back(entry.path().string());
      }
    }
  }
  return test_files;
}

// Instantiate the tests dynamically using `INSTANTIATE_TEST_SUITE_P`
INSTANTIATE_TEST_SUITE_P(InterpreterSuite, InterpreterTest,
                         ::testing::ValuesIn(CollectTestFiles({
                             "../../test/assignment",
                             "../../test/block",
                             "../../test/bool",
                             "../../test/call",
                             "../../test/class",
                             "../../test/for",
                             "../../test/if",
                             "../../test/logical_operator",
                             "../../test/number",
                             "../../test/operator",
                             "../../test/string",
                         })));

// Test each `.lox` file by comparing it to the expected `.txt` file
TEST_P(InterpreterTest, RunsProgramCorrectly) {
  std::string lox_file = GetParam();
  std::string txt_file = lox_file.substr(0, lox_file.size() - 4) + ".txt";

  ASSERT_TRUE(fs::exists(txt_file))
      << "Expected output file missing: " << txt_file;

  RunTestFromFile(lox_file, txt_file);
}
