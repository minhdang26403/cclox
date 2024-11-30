#include "parser.h"

#include <initializer_list>
#include <variant>

#include "expr.h"
#include "lox.h"
#include "stmt.h"
#include "token.h"
#include "token_type.h"

/**
 * Syntax Grammar:
 *    program       → declaration* EOF ;
 *
 * Declarations:
 *    declaration   → classDecl
 *                  | funDecl
 *                  | varDecl
 *                  | statement;
 *
 *    classDecl     → "class" IDENTIFIER ( "<" IDENTIFIER )?
 *                     "{" function* "}" ;
 *    funDecl       → "fun" function ;
 *    varDecl       → "var" IDENTIFIER ( "=" expression )? ";" ;
 *
 * Statements:
 *    statement     → exprStmt
 *                  | forStmt
 *                  | ifStmt
 *                  | printStmt
 *                  | returnStmt
 *                  | whileStmt
 *                  | block ;
 *
 *    exprStmt      → expression ";" ;
 *    forStmt       → "for" "(" ( varDecl | exprStmt | ";" )
 *                               expression? ";"
 *                               expression? ")" statement ;
 *    ifStmt        → "if" "(" expression ")" statement
 *                     ( "else" statement )? ;
 *    printStmt     → "print" expression ";" ;
 *    returnStmt    → "return" expression? ";" ;
 *    whileStmt     → "while" "(" expression ")" statement ;
 *    block         → "{" declaration* "}" ;
 *
 * Expressions:
 *    expression    → assignment ;
 *
 *    assignment    → ( call "." )? IDENTIFIER "=" assignment
 *                  | logic_or ;
 *
 *    logic_or      → logic_and ( "or" logic_and )* ;
 *    logic_and     → equality ( "and" equality )* ;
 *    equality      → comparison ( ( "!=" | "==" ) comparison )* ;
 *    comparison    → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
 *    term          → factor ( ( "-" | "+" ) factor )* ;
 *    factor        → unary ( ( "/" | "*" ) unary )* ;
 *    unary         → ( "!" | "-" ) unary | call ;
 *    call          → primary ( "(" arguments? ")" | "." IDENTIFIER )* ;
 *    primary       → "true" | "false" | "nil" | "this"
 *                  | NUMBER | STRING | IDENTIFIER | "(" expression ")"
 *                  | "super" "." IDENTIFIER ;
 *
 * Utility rules:
 *    function      → IDENTIFIER "(" parameters? ")" block ;
 *    parameters    → IDENTIFIER ( "," IDENTIFIER )* ;
 *    arguments     → expression ( "," expression )* ;
 */

namespace cclox {
auto Parser::Parse() -> std::vector<StmtPtr> {
  std::vector<StmtPtr> statements;

  while (!IsAtEnd()) {
    statements.emplace_back(ParseDeclaration());
  }

  return statements;
}

auto Parser::ParseDeclaration() -> StmtPtr {
  try {
    if (Match(TokenType::VAR)) {
      return ParseVarDeclaration();
    }

    return ParseStatement();
  } catch (const ParseError& error) {
    Synchronize();
    return StmtPtr{};
  }
}

auto Parser::ParseVarDeclaration() -> StmtPtr {
  using enum TokenType;
  Token name = Consume(IDENTIFIER, "Expect variable name.");

  std::optional<ExprPtr> initializer;
  if (Match(EQUAL)) {
    initializer = ParseExpression();
  }

  Consume(SEMICOLON, "Expect ';' after variable declaration.");
  return std::make_unique<VarStmt>(std::move(name), std::move(initializer));
}

auto Parser::ParseStatement() -> StmtPtr {
  if (Match(TokenType::PRINT)) {
    return ParsePrintStatement();
  }
  if (Match(TokenType::LEFT_BRACE)) {
    return std::make_unique<BlockStmt>(ParseBlockStatement());
  }

  return ParseExpressionStatement();
}

auto Parser::ParsePrintStatement() -> StmtPtr {
  ExprPtr value = ParseExpression();
  Consume(TokenType::SEMICOLON, "Expect ';' after value.");

  return std::make_unique<PrintStmt>(std::move(value));
}

auto Parser::ParseExpressionStatement() -> StmtPtr {
  ExprPtr expr = ParseExpression();
  Consume(TokenType::SEMICOLON, "Expect ';' after expression.");

  return std::make_unique<ExprStmt>(std::move(expr));
}

auto Parser::ParseBlockStatement() -> std::vector<StmtPtr> {
  std::vector<StmtPtr> statements;

  while (!Check(TokenType::RIGHT_BRACE) && !IsAtEnd()) {
    statements.emplace_back(ParseDeclaration());
  }

  Consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
  return statements;
}

auto Parser::ParseExpression() -> ExprPtr {
  return ParseAssignment();
}

auto Parser::ParseAssignment() -> ExprPtr {
  ExprPtr expr = ParseEquality();

  if (Match(TokenType::EQUAL)) {
    Token equals = Previous();
    ExprPtr value = ParseAssignment();
    const auto* variable_expr_ptr = std::get_if<VariableExprPtr>(&expr);

    if (variable_expr_ptr) {
      const Token& variable = (*variable_expr_ptr)->GetVariable();
      return std::make_unique<AssignExpr>(variable, std::move(value));
    }

    throw Error(equals, "Invalid assignment target.");
  }

  return expr;
}

auto Parser::ParseEquality() -> ExprPtr {
  ExprPtr expr = ParseComparison();
  using enum TokenType;

  while (Match(BANG_EQUAL, EQUAL_EQUAL)) {
    Token op = Previous();
    ExprPtr right = ParseComparison();
    expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op),
                                        std::move(right));
  }

  return expr;
}

auto Parser::ParseComparison() -> ExprPtr {
  ExprPtr expr = ParseTerm();
  using enum TokenType;

  while (Match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
    Token op = Previous();
    ExprPtr right = ParseTerm();
    expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op),
                                        std::move(right));
  }

  return expr;
}

auto Parser::ParseTerm() -> ExprPtr {
  ExprPtr expr = ParseFactor();
  using enum TokenType;

  while (Match(MINUS, PLUS)) {
    Token op = Previous();
    ExprPtr right = ParseFactor();
    expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op),
                                        std::move(right));
  }

  return expr;
}

auto Parser::ParseFactor() -> ExprPtr {
  ExprPtr expr = ParseUnary();
  using enum TokenType;

  while (Match(SLASH, STAR)) {
    Token op = Previous();
    ExprPtr right = ParseUnary();
    expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op),
                                        std::move(right));
  }

  return expr;
}

auto Parser::ParseUnary() -> ExprPtr {
  using enum TokenType;
  if (Match(BANG, MINUS)) {
    Token op = Previous();
    ExprPtr right = ParseUnary();
    return std::make_unique<UnaryExpr>(std::move(op), std::move(right));
  }

  return ParsePrimary();
}

auto Parser::ParsePrimary() -> ExprPtr {
  using enum TokenType;
  if (Match(FALSE)) {
    return std::make_unique<LiteralExpr>(Object{false});
  }

  if (Match(TRUE)) {
    return std::make_unique<LiteralExpr>(Object{true});
  }

  if (Match(NIL)) {
    return std::make_unique<LiteralExpr>(Object{nullptr});
  }

  if (Match(NUMBER, STRING)) {
    return std::make_unique<LiteralExpr>(Previous().GetLiteral());
  }

  if (Match(IDENTIFIER)) {
    return std::make_unique<VariableExpr>(Previous());
  }

  if (Match(LEFT_PAREN)) {
    ExprPtr expr = ParseExpression();
    Consume(RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_unique<GroupingExpr>(std::move(expr));
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

auto Parser::Error(const Token& token, std::string_view message) const
    -> ParseError {
  Lox::Error(output_, token, message);
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
      default: {
      };
    }

    Advance();
  }
}

}  // namespace cclox
