#include "parser.h"

#include <initializer_list>
#include <memory>
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
    if (Match(TokenType::FUN)) {
      return ParseFunction("function");
    }
    if (Match(TokenType::VAR)) {
      return ParseVarDeclaration();
    }

    return ParseStatement();
  } catch (const ParseError& error) {
    Synchronize();
    return StmtPtr{};
  }
}

auto Parser::ParseFunction(std::string_view kind) -> StmtPtr {
  using enum TokenType;
  // Parse function name.
  Token name = Consume(IDENTIFIER, std::format("Expect {} name.", kind));
  Consume(LEFT_PAREN, std::format("Expect '(' after {} name.", kind));

  // Parse function parameters.
  std::vector<Token> parameters;
  if (!Check(RIGHT_PAREN)) {
    do {
      if (parameters.size() >= 255) {
        Lox::Error(output_, Peek(), "Can't have more than 255 parameters.");
      }
      parameters.emplace_back(Consume(IDENTIFIER, "Expect parameter name."));
    } while (Match(COMMA));
  }
  Consume(RIGHT_PAREN, "Expect ')' after parameters.");

  // Parse function body.
  Consume(LEFT_BRACE, std::format("Expect '{{' before {} body.", kind));
  std::vector<StmtPtr> body = ParseBlockStatement();

  return std::make_unique<FunctionStmt>(std::move(name), std::move(parameters),
                                        std::move(body));
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
  using enum TokenType;
  if (Match(FOR)) {
    return ParseForStatement();
  }
  if (Match(IF)) {
    return ParseIfStatement();
  }
  if (Match(PRINT)) {
    return ParsePrintStatement();
  }
  if (Match(RETURN)) {
    return ParseReturnStatement();
  }
  if (Match(WHILE)) {
    return ParseWhileStatement();
  }
  if (Match(LEFT_BRACE)) {
    return std::make_unique<BlockStmt>(ParseBlockStatement());
  }

  return ParseExpressionStatement();
}

auto Parser::ParseForStatement() -> StmtPtr {
  Consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

  std::optional<StmtPtr> initializer_stmt;
  if (Match(TokenType::SEMICOLON)) {
    // Do nothing.
  } else if (Match(TokenType::VAR)) {
    initializer_stmt = ParseVarDeclaration();
  } else {
    initializer_stmt = ParseExpressionStatement();
  }

  std::optional<ExprPtr> condition_expr;
  if (!Check(TokenType::SEMICOLON)) {
    condition_expr = ParseExpression();
  }
  Consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

  std::optional<ExprPtr> increment_expr;
  if (!Check(TokenType::RIGHT_PAREN)) {
    increment_expr = ParseExpression();
  }
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

  StmtPtr body = ParseStatement();

  using std::make_unique, std::move;

  if (increment_expr) {
    std::vector<StmtPtr> statements;
    statements.reserve(2);
    statements.emplace_back(move(body));
    statements.emplace_back(
        make_unique<ExprStmt>(move(increment_expr.value())));
    body = make_unique<BlockStmt>(move(statements));
  }

  if (!condition_expr) {
    condition_expr = make_unique<LiteralExpr>(Object{true});
  }
  body = make_unique<WhileStmt>(move(condition_expr.value()), move(body));

  if (initializer_stmt) {
    std::vector<StmtPtr> statements;
    statements.reserve(2);
    statements.emplace_back(move(initializer_stmt.value()));
    statements.emplace_back(move(body));
    body = make_unique<BlockStmt>(move(statements));
  }

  return body;
}

auto Parser::ParseIfStatement() -> StmtPtr {
  Consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
  ExprPtr condition = ParseExpression();
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

  StmtPtr then_branch = ParseStatement();
  std::optional<StmtPtr> else_branch;
  if (Match(TokenType::ELSE)) {
    else_branch = ParseStatement();
  }

  return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch),
                                  std::move(else_branch));
}

auto Parser::ParsePrintStatement() -> StmtPtr {
  ExprPtr value = ParseExpression();
  Consume(TokenType::SEMICOLON, "Expect ';' after value.");

  return std::make_unique<PrintStmt>(std::move(value));
}

auto Parser::ParseReturnStatement() -> StmtPtr {
  Token keyword = Previous();

  std::optional<ExprPtr> value;
  if (!Check(TokenType::SEMICOLON)) {
    value = ParseExpression();
  }
  Consume(TokenType::SEMICOLON, "Expect ';' after return value.");

  return std::make_unique<ReturnStmt>(std::move(keyword), std::move(value));
}

auto Parser::ParseWhileStatement() -> StmtPtr {
  Consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
  ExprPtr condition = ParseExpression();
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
  StmtPtr body = ParseStatement();

  return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
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
  ExprPtr expr = ParseOr();

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

auto Parser::ParseOr() -> ExprPtr {
  ExprPtr expr = ParseAnd();

  while (Match(TokenType::OR)) {
    Token op = Previous();
    ExprPtr right = ParseAnd();
    expr = std::make_unique<LogicalExpr>(std::move(expr), std::move(op),
                                         std::move(right));
  }

  return expr;
}

auto Parser::ParseAnd() -> ExprPtr {
  ExprPtr expr = ParseEquality();

  while (Match(TokenType::AND)) {
    Token op = Previous();
    ExprPtr right = ParseEquality();
    expr = std::make_unique<LogicalExpr>(std::move(expr), std::move(op),
                                         std::move(right));
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

  return ParseCall();
}

auto Parser::ParseCall() -> ExprPtr {
  ExprPtr expr = ParsePrimary();

  while (true) {
    if (Match(TokenType::LEFT_PAREN)) {
      expr = FinishCall(std::move(expr));
    } else {
      break;
    }
  }

  return expr;
}

auto Parser::ParsePrimary() -> ExprPtr {
  using enum TokenType;
  using std::make_unique;
  if (Match(FALSE)) {
    return make_unique<LiteralExpr>(Object{false});
  }

  if (Match(TRUE)) {
    return make_unique<LiteralExpr>(Object{true});
  }

  if (Match(NIL)) {
    return make_unique<LiteralExpr>(Object{nullptr});
  }

  if (Match(NUMBER, STRING)) {
    return make_unique<LiteralExpr>(Previous().GetLiteral());
  }

  if (Match(IDENTIFIER)) {
    return make_unique<VariableExpr>(Previous());
  }

  if (Match(LEFT_PAREN)) {
    ExprPtr expr = ParseExpression();
    Consume(RIGHT_PAREN, "Expect ')' after expression.");
    return make_unique<GroupingExpr>(std::move(expr));
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

auto Parser::FinishCall(ExprPtr callee) -> ExprPtr {
  std::vector<ExprPtr> arguments;

  if (!Check(TokenType::RIGHT_PAREN)) {
    do {
      if (arguments.size() >= 255) {
        Lox::Error(output_, Peek(), "Can't have more than 255 arguments.");
      }
      arguments.emplace_back(ParseExpression());
    } while (Match(TokenType::COMMA));
  }

  Token paren = Consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

  return std::make_unique<CallExpr>(std::move(callee), std::move(paren),
                                    std::move(arguments));
}

}  // namespace cclox
