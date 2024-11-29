#ifndef STMT_H_
#define STMT_H_

#include <memory>
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
  BlockStmt(std::vector<StmtPtr> statements)
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

class IfStmt {};

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

class WhileStmt {};

}  // namespace cclox

#endif  // STMT_H_