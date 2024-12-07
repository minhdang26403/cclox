cmake_minimum_required(VERSION 3.28)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # For clang-tidy and clangd.
set(CMAKE_CXX_STANDARD 20) # Compile as C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Require C++20 support.

project(CraftingInterpreter VERSION 2024.1
  DESCRIPTION "A tree-walk interpreter"
  LANGUAGES C CXX)

# #####################################################################################################################
# COMPILER SETUP
# #####################################################################################################################

# Expected directory structure.
set(CCLOX_BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build_support")
set(CCLOX_CLANG_SEARCH_PATH "/usr/bin/" "/opt/homebrew/opt/llvm/bin/")

message(STATUS "You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# Output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(NOT DEFINED SANITIZER)
  set(SANITIZER thread)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Werror -Wconversion -Wno-unqualified-std-cast-call")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fsanitize=${SANITIZER} -fno-omit-frame-pointer -fno-optimize-sibling-calls")

# #####################################################################################################################
# DEPENDENCIES
# #####################################################################################################################

# clang-tidy
find_program(CLANG_TIDY_BIN
  NAMES clang-tidy clang-tidy-14
  HINTS ${CCLOX_CLANG_SEARCH_PATH}
)

if("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
  message(WARNING "cclox/main couldn't find clang-tidy.")
else()
  message(STATUS "cclox/main found clang-tidy at ${CLANG_TIDY_BIN}")
endif()

# cpplint
find_program(CPPLINT_BIN
  NAMES cpplint cpplint.py
  HINTS ${CCLOX_BUILD_SUPPORT_DIR}
)

message(${CCLOX_BUILD_SUPPORT_DIR})

if("${CPPLINT_BIN}" STREQUAL "CPPLINT_BIN-NOTFOUND")
  message(WARNING "cclox/main couldn't find cpplint.")
else()
  message(STATUS "cclox/main found cpplint at ${CPPLINT_BIN}")
endif()

# GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/f8d7d77.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

include_directories(${PROJECT_SOURCE_DIR}/src/include)

add_subdirectory(src)
add_subdirectory(test)

# #####################################################################################################################
# MAKE TARGETS
# #####################################################################################################################

# #########################################
# "make check-lint"
# #########################################
file(GLOB_RECURSE CCLOX_LINT_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/include/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
)

# Balancing act: cpplint.py takes a non-trivial time to launch,
# so process 12 files per invocation, while still ensuring parallelism
add_custom_target(check-lint echo '${CCLOX_LINT_FILES}' | xargs -n12 -P8
  ${CPPLINT_BIN}
  --verbose=2 --quiet
  --linelength=120
  --filter=-legal/copyright,-build/header_guard,-build/include_subdir,-runtime/references # https://github.com/cpplint/cpplint/issues/148
)

# ##########################################################
# "make check-clang-tidy" target
# ##########################################################
# runs clang-tidy and exits with a non-zero exit code if any errors are found.
# note that clang-tidy automatically looks for a .clang-tidy file in parent directories
add_custom_target(check-clang-tidy
  ${CCLOX_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
  -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
  -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
  ${CCLOX_LINT_FILES}
)

add_dependencies(check-clang-tidy cclox)