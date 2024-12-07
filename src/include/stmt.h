#ifndef STMT_H_
#define STMT_H_

#include <memory>
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

class ClassStmt {};

class ExprStmt {
 public:
  explicit ExprStmt(ExprPtr expression) : expression_(std::move(expression)) {}

  auto GetExpression() const noexcept -> const ExprPtr& { return expression_; }

 private:
  ExprPtr expression_;
};

class FunctionStmt {};

class IfStmt {
 public:
  IfStmt(ExprPtr condition, StmtPtr then_branch,
         std::optional<StmtPtr> else_branch)
      : condition_(std::move(condition)),
        then_branch_(std::move(then_branch)),
        else_branch_(std::move(else_branch)) {}

  auto GetCondition() const noexcept -> const ExprPtr& { return condition_; }

  auto GetThenBranch() const noexcept -> const StmtPtr& { return then_branch_; }

  auto HasElseBranch() const noexcept -> bool {
    return else_branch_.has_value();
  }

  auto GetElseBranch() const -> const StmtPtr& { return else_branch_.value(); }

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

class ReturnStmt {};

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
