set(TESTS
  interpreter_test
  expression_test
)

# Loop through each test
foreach(TEST_NAME ${TESTS})
  # Add ".cpp" suffix to the test name
  set(TEST_NAME_CPP "${TEST_NAME}.cpp")

  # Create executable from the test source file
  add_executable(${TEST_NAME} ${TEST_NAME_CPP})

  # Link the test with gtest_main
  target_link_libraries(${TEST_NAME} GTest::gtest_main lox)

  # Discover tests for each executable
  gtest_discover_tests(${TEST_NAME})
endforeach()