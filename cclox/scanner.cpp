#include "scanner.h"
#include "token.h"
#include "token_type.h"

namespace cclox {

// clang-format off

// A map of reserved keywords in the Lox language.
const std::unordered_map<std::string, TokenType> Scanner::keywords = {
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

auto Scanner::IsAtEnd() const -> bool {
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
      AddToken(TokenType::MINUS);
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
      String();
      break;
    default:
      if (IsDigit(c)) {
        Number();
      } else if (IsAlpha(c)) {
        Identifier();
      } else {
        Lox::Error(line_number_, "Unexpected character.");
      }
      break;
  }
}

auto Scanner::Identifier() -> void {
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

auto Scanner::Number() -> void {
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

  Object value{std::stod(source_.substr(start_, current_ - start_))};
  AddToken(TokenType::NUMBER, value);
}

auto Scanner::String() -> void {
  while (Peek() != '"' && !IsAtEnd()) {
    if (Peek() != '\n') {
      line_number_++;
    }
    Advance();
  }

  if (IsAtEnd()) {
    Lox::Error(line_number_, "Unterminated string.");
    return;
  }

  // The closing ".
  Advance();

  // Trim the surrounding quotes.
  Object value{source_.substr(start_ + 1, current_ - start_ - 2)};
  AddToken(TokenType::STRING, value);
}

auto Scanner::Match(char expected) -> bool {
  if (IsAtEnd() || source_[current_] != expected) {
    return false;
  }

  current_++;
  return true;
}

auto Scanner::Peek() const -> char {
  if (IsAtEnd()) {
    return '\0';
  }

  return source_[current_];
}

auto Scanner::PeekNext() const -> char {
  if (current_ + 1 >= source_.size()) {
    return '\0';
  }

  return source_[current_ + 1];
}

auto Scanner::IsAlpha(char c) const -> bool {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

auto Scanner::IsDigit(char c) const -> bool {
  return c >= '0' && c <= '9';
}

auto Scanner::IsAlphaNumeric(char c) -> bool {
  return IsAlpha(c) || IsDigit(c);
}

auto Scanner::Advance() -> char {
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
