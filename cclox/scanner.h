#ifndef SCANNER_H_
#define SCANNER_H_

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "lox.h"
#include "token.h"
#include "token_type.h"

namespace cclox {
/**
 * @brief The Scanner reads the raw source code, identifies lexemes, and
 * produces a list of tokens.
 */
class Scanner {
 public:
  /**
   * @brief Constructs a Scanner object with the provided source code.
   * @param source the source code to scan.
   */
  explicit Scanner(std::string source) : source_(std::move(source)) {}

  /**
   * @brief Scans the source code and returns a list of tokens.
   * @return a vector containing all the tokens scanned from the source code.
   */
  auto ScanTokens() -> std::vector<Token>;

 private:
  /**
   * @brief Checks if the scanner has reached the end of the source code.
   * @return true if the scanner is at the end of the source, false otherwise.
   */
  auto IsAtEnd() const noexcept -> bool;

  /**
   * @brief Reads characters from the source, determines what type of token they
   * form, and adds the token to the token list.
   */
  auto ScanToken() -> void;

  /**
   * @brief Reads characters that form an identifier and checks if the read
   * identifier matches any reserved keywords.
   */
  auto ScanIdentifier() -> void;

  /**
   * @brief Reads characters that represent a numeric literal, including integer
   * and fractional parts.
   */
  auto ScanNumber() -> void;

  /**
   * @brief Processes a string literal from the source code.
   */
  auto ScanString() -> void;

  /**
   * @brief Compares the current character with the expected character. Only
   * consumes the current character if they match.
   * @param expected the character to compare against the current character.
   * @return true if the current character matches the expected character, false
   * otherwise.
   */
  auto Match(char expected) noexcept -> bool;

  /**
   * @brief Peeks at the current character in the source code without advancing.
   * @return the current character in the source code, or '\0' if at the end.
   */
  auto Peek() const noexcept -> char;

  /**
   * @brief Peeks at the next character in the source code without advancing.
   * @return the next character in the source code, or '\0' if at the end.
   */
  auto PeekNext() const noexcept -> char;

  /**
   * @brief Checks if the given character is an uppercase or lowercase letter
   * (A-Z, a-z).
   * @param c the character to check.
   * @return true if the character is alphabetic, false otherwise.
   */
  auto IsAlpha(char c) const noexcept -> bool;

  /**
   * @brief Checks if the given character is a numeric digit (0-9).
   * @param c the character to check.
   * @return true if the character is a digit, false otherwise.
   */
  auto IsDigit(char c) const noexcept -> bool;

  /**
   * @brief Checks if the given character is either an alphabetic character
   * (A-Z, a-z) or a digit (0-9).
   * @param c the character to check.
   * @return true if the character is alphanumeric, false otherwise.
   */
  auto IsAlphaNumeric(char c) const noexcept -> bool;

  /**
   * @brief Returns the current character and then advances
   * the scanner to the next character in the source code.
   * @return the current character before advancing.
   */
  auto Advance() noexcept -> char;

  /**
   * @brief Creates a token of the given type and adds it to the list of tokens.
   * @param type the type of token to add.
   */
  auto AddToken(TokenType type) -> void;

  /**
   * @brief Creates a token of the given type with an associated literal value,
   * and adds it to the list of tokens.
   * @param type the type of token to add.
   * @param literal the literal value associated with the token.
   */
  auto AddToken(TokenType type, std::optional<Object> literal) -> void;

  static const std::unordered_map<std::string, TokenType> keywords;

  // The source code being scanned.
  std::string source_;
  // The list of tokens generated from the source code.
  std::vector<Token> tokens_;
  // The starting index of the current lexeme.
  uint32_t start_{0};
  // The current position in the source code.
  uint32_t current_{0};
  // The current line number in the source code.
  uint32_t line_number_{1};
};
}  // namespace cclox

#endif  // SCANNER_H_
