#include "parser.h"

#include <initializer_list>

#include "expr.h"
#include "lox.h"
#include "token.h"
#include "token_type.h"

/**
 * Grammar:
 *    expression  -> equality;
 *    equality    -> comparison ( ( "!=" | "==" ) comparison )* ;
 *    comparison  -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
 *    term        -> factor ( ( "-" | "+" ) factor )* ;
 *    factor      -> unary ( ( "/" | "*" ) unary )* ;
 *    unary       -> ( "!" | "-" ) unary
 *                | primary ;
 *    primary     -> NUMBER | STRING | "true" | "false" | "nil"
 *                | "(" expression ")" ;
 */

namespace cclox {

auto Parser::Parse() -> ExprPtr {
  try {
    return ParseExpression();
  } catch (const ParseError& error) {
    return {};
  }
}

auto Parser::ParseExpression() -> ExprPtr {
  return ParseEquality();
}

auto Parser::ParseEquality() -> ExprPtr {
  ExprPtr expr = ParseComparison();
  using enum TokenType;

  while (Match(BANG_EQUAL, EQUAL_EQUAL)) {
    Token op = Previous();
    ExprPtr right = ParseComparison();
    expr = ExprPtr{std::make_unique<Binary>(std::move(expr), std::move(op),
                                            std::move(right))};
  }

  return expr;
}

auto Parser::ParseComparison() -> ExprPtr {
  ExprPtr expr = ParseTerm();
  using enum TokenType;

  while (Match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
    Token op = Previous();
    ExprPtr right = ParseTerm();
    expr = ExprPtr{std::make_unique<Binary>(std::move(expr), std::move(op),
                                            std::move(right))};
  }

  return expr;
}

auto Parser::ParseTerm() -> ExprPtr {
  ExprPtr expr = ParseFactor();
  using enum TokenType;

  while (Match(MINUS, PLUS)) {
    Token op = Previous();
    ExprPtr right = ParseFactor();
    expr = ExprPtr{std::make_unique<Binary>(std::move(expr), std::move(op),
                                            std::move(right))};
  }

  return expr;
}

auto Parser::ParseFactor() -> ExprPtr {
  ExprPtr expr = ParseUnary();
  using enum TokenType;

  while (Match(SLASH, STAR)) {
    Token op = Previous();
    ExprPtr right = ParseUnary();
    expr = ExprPtr{std::make_unique<Binary>(std::move(expr), std::move(op),
                                            std::move(right))};
  }

  return expr;
}

auto Parser::ParseUnary() -> ExprPtr {
  using enum TokenType;
  if (Match(BANG, MINUS)) {
    Token op = Previous();
    ExprPtr right = ParseUnary();
    return ExprPtr{std::make_unique<Unary>(std::move(op), std::move(right))};
  }

  return ParsePrimary();
}

auto Parser::ParsePrimary() -> ExprPtr {
  using enum TokenType;
  if (Match(FALSE)) {
    return ExprPtr{std::make_unique<Literal>(false)};
  }

  if (Match(TRUE)) {
    return ExprPtr{std::make_unique<Literal>(true)};
  }

  if (Match(NIL)) {
    return ExprPtr{std::make_unique<Literal>(nullptr)};
  }

  if (Match(NUMBER, STRING)) {
    return ExprPtr{std::make_unique<Literal>(Previous().GetLiteral())};
  }

  if (Match(LEFT_PAREN)) {
    ExprPtr expr = ParseExpression();
    Consume(RIGHT_PAREN, "Expect ')' after expression.");
    return ExprPtr{std::make_unique<Grouping>(std::move(expr))};
  }

  throw Error(Peek(), "Expect expression.");
}

template<typename T, typename... Ts, typename>
auto Parser::Match(T type, Ts... types) noexcept -> bool {
  auto CheckAndAdvance = [this](TokenType type) {
    if (Check(type)) {
      Advance();
      return true;
    }
    return false;
  };
  return (CheckAndAdvance(type) || ... || CheckAndAdvance(types));
}

auto Parser::Consume(TokenType type, std::string_view message) -> Token {
  if (Check(type)) {
    return Advance();
  }

  throw Error(Peek(), message);
}

auto Parser::Check(TokenType type) const noexcept -> bool {
  if (IsAtEnd()) {
    return false;
  }
  return Peek().GetType() == type;
}

auto Parser::Advance() noexcept -> Token {
  if (!IsAtEnd()) {
    current_++;
  }
  return Previous();
}

auto Parser::IsAtEnd() const noexcept -> bool {
  return Peek().GetType() == TokenType::EoF;
}

auto Parser::Peek() const noexcept -> Token {
  return tokens_[current_];
}

auto Parser::Previous() const noexcept -> Token {
  return tokens_[current_ - 1];
}

auto Parser::Error(const Token& token, std::string_view message) const noexcept
    -> ParseError {
  Lox::Error(token, message);
  return ParseError{message};
}

auto Parser::Synchronize() noexcept -> void {
  Advance();
  using enum TokenType;

  while (!IsAtEnd()) {
    if (Previous().GetType() == SEMICOLON) {
      return;
    }

    switch (Peek().GetType()) {
      case CLASS:
      case FUN:
      case VAR:
      case FOR:
      case IF:
      case WHILE:
      case PRINT:
      case RETURN:
        return;
      default:;
    }

    Advance();
  }
}

}  // namespace cclox
