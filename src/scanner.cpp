#include "scanner.h"
#include <stdexcept>
#include "object.h"
#include "token.h"
#include "token_type.h"

/**
 * Lexical Grammar
 *    NUMBER          → DIGIT+ ( "." DIGIT+ )? ;
 *    STRING          → "\"" <any char except "\"">* "\"" ;
 *    IDENTIFIER      → ALPHA ( ALPHA | DIGIT )* ;
 *    ALPHA           → "a" ... "z" | "A" ... "Z" | "_" ;
 *    DIGIT           → "0" ... "9" ;
 */

namespace cclox {

// clang-format off

// A map of reserved keywords in the Lox language.
const Scanner::TokenTypeMap Scanner::keywords = {
    {"and", TokenType::AND},
    {"class",  TokenType::CLASS},
    {"else",   TokenType::ELSE},
    {"false",  TokenType::FALSE},
    {"for",    TokenType::FOR},
    {"fun",    TokenType::FUN},
    {"if",     TokenType::IF},
    {"nil",    TokenType::NIL},
    {"or",     TokenType::OR},
    {"print",  TokenType::PRINT},
    {"return", TokenType::RETURN},
    {"super",  TokenType::SUPER},
    {"this",   TokenType::THIS},
    {"true",   TokenType::TRUE},
    {"var",    TokenType::VAR},
    {"while",  TokenType::WHILE},
};

// clang-format on

auto Scanner::ScanTokens() -> std::vector<Token> {
  while (!IsAtEnd()) {
    // We are at the beginning of the next lexeme.
    start_ = current_;
    ScanToken();
  }

  tokens_.emplace_back(TokenType::EoF, "", std::nullopt, line_number_);
  return tokens_;
}

auto Scanner::IsAtEnd() const noexcept -> bool {
  return current_ >= source_.size();
}

auto Scanner::ScanToken() -> void {
  char c = Advance();

  switch (c) {
    case '(':
      AddToken(TokenType::LEFT_PAREN);
      break;
    case ')':
      AddToken(TokenType::RIGHT_PAREN);
      break;
    case '{':
      AddToken(TokenType::LEFT_BRACE);
      break;
    case '}':
      AddToken(TokenType::RIGHT_BRACE);
      break;
    case ',':
      AddToken(TokenType::COMMA);
      break;
    case '.':
      AddToken(TokenType::DOT);
      break;
    case '-':
      // We can't always split the minus sign and the following numeric value
      // since int32_t can't store 2147483648 (INT_MAX + 1), but it can store
      // -2147483648 (INT_MIN). Hence, we group the minus sign and the numeric
      // to handle this case.
      if (IsDigit(Peek())) {
        ScanNumber();
      } else {
        AddToken(TokenType::MINUS);
      }
      break;
    case '+':
      AddToken(TokenType::PLUS);
      break;
    case ';':
      AddToken(TokenType::SEMICOLON);
      break;
    case '*':
      AddToken(TokenType::STAR);
      break;
    case '!':
      AddToken(Match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
      break;
    case '=':
      AddToken(Match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
      break;
    case '<':
      AddToken(Match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
      break;
    case '>':
      AddToken(Match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
      break;
    case '/':
      if (Match('/')) {
        // A comment goes until the end of the line.
        while (Peek() != '\n' && !IsAtEnd()) {
          Advance();
        }
      } else {
        AddToken(TokenType::SLASH);
      }
      break;
    case ' ':
    case '\r':
    case '\t':
      // Ignore whitespace.
      break;
    case '\n':
      line_number_++;
      break;
    case '"':
      ScanString();
      break;
    default:
      if (IsDigit(c)) {
        ScanNumber();
      } else if (IsAlpha(c)) {
        ScanIdentifier();
      } else {
        Lox::Error(output_, line_number_, "Unexpected character.");
      }
      break;
  }
}

auto Scanner::ScanIdentifier() -> void {
  while (IsAlphaNumeric(Peek())) {
    Advance();
  }

  std::string text = source_.substr(start_, current_ - start_);
  TokenType type = TokenType::IDENTIFIER;
  if (keywords.contains(text)) {
    type = keywords.at(text);
  }

  AddToken(type);
}

auto Scanner::ScanNumber() -> void {
  while (IsDigit(Peek())) {
    Advance();
  }

  // Look for a fractional part.
  if (Peek() == '.' && IsDigit(PeekNext())) {
    // Consume the "."
    Advance();

    while (IsDigit(Peek())) {
      Advance();
    }
  }

  std::string num_str = source_.substr(start_, current_ - start_);
  std::variant<int32_t, double> num;

  if (num_str.find('.') != std::string::npos ||
      num_str.find('e') != std::string::npos) {
    num = std::stod(num_str);
  } else {
    try {
      num = std::stoi(num_str);
    } catch (const std::out_of_range&) {
      num = std::stod(num_str);
    }
  }

  if (auto pval = std::get_if<int32_t>(&num)) {
    Object value{*pval};
    AddToken(TokenType::NUMBER, value);
  } else if (auto pval = std::get_if<double>(&num)) {
    Object value{*pval};
    AddToken(TokenType::NUMBER, value);
  }
}

auto Scanner::ScanString() -> void {
  while (Peek() != '"' && !IsAtEnd()) {
    if (Peek() == '\n') {
      line_number_++;
    }
    Advance();
  }

  if (IsAtEnd()) {
    Lox::Error(output_, line_number_, "Unterminated string.");
    return;
  }

  // The closing ".
  Advance();

  // Trim the surrounding quotes.
  Object value{source_.substr(start_ + 1, current_ - start_ - 2)};
  AddToken(TokenType::STRING, value);
}

auto Scanner::Match(char expected) noexcept -> bool {
  if (IsAtEnd() || source_[current_] != expected) {
    return false;
  }

  current_++;
  return true;
}

auto Scanner::Peek() const noexcept -> char {
  if (IsAtEnd()) {
    return '\0';
  }

  return source_[current_];
}

auto Scanner::PeekNext() const noexcept -> char {
  if (current_ + 1 >= source_.size()) {
    return '\0';
  }

  return source_[current_ + 1];
}

auto Scanner::IsAlpha(char c) const noexcept -> bool {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

auto Scanner::IsDigit(char c) const noexcept -> bool {
  return c >= '0' && c <= '9';
}

auto Scanner::IsAlphaNumeric(char c) const noexcept -> bool {
  return IsAlpha(c) || IsDigit(c);
}

auto Scanner::Advance() noexcept -> char {
  return source_[current_++];
}

auto Scanner::AddToken(TokenType type) -> void {
  AddToken(type, std::nullopt);
}

auto Scanner::AddToken(TokenType type, std::optional<Object> literal) -> void {
  std::string text = source_.substr(start_, current_ - start_);
  tokens_.emplace_back(type, std::move(text), std::move(literal), line_number_);
}
}  // namespace cclox
