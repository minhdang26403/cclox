#ifndef STMT_H_
#define STMT_H_

#include <memory>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

#include "expr.h"

namespace cclox {
class BlockStmt;
class ClassStmt;
class ExprStmt;
class FunctionStmt;
class IfStmt;
class PrintStmt;
class ReturnStmt;
class VarStmt;
class WhileStmt;

using BlockStmtPtr = std::unique_ptr<BlockStmt>;
using ClassStmtPtr = std::unique_ptr<ClassStmt>;
using ExprStmtPtr = std::unique_ptr<ExprStmt>;
using FunctionStmtPtr = std::unique_ptr<FunctionStmt>;
using IfStmtPtr = std::unique_ptr<IfStmt>;
using PrintStmtPtr = std::unique_ptr<PrintStmt>;
using ReturnStmtPtr = std::unique_ptr<ReturnStmt>;
using VarStmtPtr = std::unique_ptr<VarStmt>;
using WhileStmtPtr = std::unique_ptr<WhileStmt>;

using StmtPtr = std::variant<BlockStmtPtr, ClassStmtPtr, ExprStmtPtr,
                             FunctionStmtPtr, IfStmtPtr, PrintStmtPtr,
                             ReturnStmtPtr, VarStmtPtr, WhileStmtPtr>;

class BlockStmt {
 public:
  explicit BlockStmt(std::vector<StmtPtr> statements)
      : statements_(std::move(statements)) {}

  auto GetStatements() const noexcept -> const std::vector<StmtPtr>& {
    return statements_;
  }

 private:
  std::vector<StmtPtr> statements_;
};

class ClassStmt {
 public:
  ClassStmt(Token name, std::vector<StmtPtr> methods)
      : name_(std::move(name)), methods_(std::move(methods)) {}

  auto GetClassName() const noexcept -> const Token& { return name_; }

  auto GetClassMethods() const noexcept -> const std::vector<StmtPtr>& {
    return methods_;
  }

 private:
  Token name_;
  std::vector<StmtPtr> methods_;
};

class ExprStmt {
 public:
  explicit ExprStmt(ExprPtr expression) : expression_(std::move(expression)) {}

  auto GetExpression() const noexcept -> const ExprPtr& { return expression_; }

 private:
  ExprPtr expression_;
};

class FunctionStmt {
 public:
  FunctionStmt(Token name, std::vector<Token> params, std::vector<StmtPtr> body)
      : name_(std::move(name)),
        params_(std::move(params)),
        body_(std::move(body)) {}

  auto GetFunctionName() const noexcept -> const Token& { return name_; }

  auto GetParams() const noexcept -> const std::vector<Token>& {
    return params_;
  }

  auto GetBody() const noexcept -> const std::vector<StmtPtr>& { return body_; }

 private:
  Token name_;
  std::vector<Token> params_;
  std::vector<StmtPtr> body_;
};

class IfStmt {
 public:
  IfStmt(ExprPtr condition, StmtPtr then_branch,
         std::optional<StmtPtr> else_branch)
      : condition_(std::move(condition)),
        then_branch_(std::move(then_branch)),
        else_branch_(std::move(else_branch)) {}

  auto GetCondition() const noexcept -> const ExprPtr& { return condition_; }

  auto GetThenBranch() const noexcept -> const StmtPtr& { return then_branch_; }

  auto GetElseBranch() const noexcept -> const std::optional<StmtPtr>& {
    return else_branch_;
  }

 private:
  ExprPtr condition_;
  StmtPtr then_branch_;
  std::optional<StmtPtr> else_branch_;
};

class PrintStmt {
 public:
  explicit PrintStmt(ExprPtr expression) : expression_(std::move(expression)) {}

  auto GetExpression() const noexcept -> const ExprPtr& { return expression_; }

 private:
  ExprPtr expression_;
};

class ReturnStmt {
 public:
  ReturnStmt(Token keyword, std::optional<ExprPtr> value)
      : keyword_(std::move(keyword)), value_(std::move(value)) {}

  auto GetKeyword() const noexcept -> const Token& { return keyword_; }

  auto GetValue() const noexcept -> const std::optional<ExprPtr>& {
    return value_;
  }

 private:
  Token keyword_;
  std::optional<ExprPtr> value_;
};

class VarStmt {
 public:
  VarStmt(Token variable, std::optional<ExprPtr> initializer)
      : variable_(std::move(variable)), initializer_(std::move(initializer)) {}

  auto GetVariable() const noexcept -> const Token& { return variable_; }

  auto GetInitializer() const noexcept -> const std::optional<ExprPtr>& {
    return initializer_;
  }

 private:
  Token variable_;
  std::optional<ExprPtr> initializer_;
};

class WhileStmt {
 public:
  WhileStmt(ExprPtr condition, StmtPtr body)
      : condition_(std::move(condition)), body_(std::move(body)) {}

  auto GetCondition() const noexcept -> const ExprPtr& { return condition_; }

  auto GetBody() const noexcept -> const StmtPtr& { return body_; }

 private:
  ExprPtr condition_;
  StmtPtr body_;
};

}  // namespace cclox

#endif  // STMT_H_
