#include "ast_printer.h"
#include <format>
#include <variant>

namespace cclox::ASTPrinter {
// Group all helper functions in an anonymous namespace
namespace {
auto Parenthesize(std::string_view name, const ExprPtr& expr) -> std::string {
  return std::format("({} {})", name, ToStringExpr(expr));
}

auto Parenthesize(std::string_view name, const ExprPtr& expr1,
                  const ExprPtr& expr2) -> std::string {
  return std::format("({} {} {})", name, ToStringExpr(expr1),
                     ToStringExpr(expr2));
}

auto ToStringBinaryExpr(const BinaryPtr& expr) -> std::string {
  return Parenthesize(expr->GetOperator().GetLexeme(), expr->GetLeftExpr(),
                      expr->GetRightExpr());
}

auto ToStringGroupExpr(const GroupingPtr& expr) -> std::string {
  return Parenthesize("group", expr->GetExpr());
}

auto ToStringLiteralExpr(const LiteralPtr& expr) -> std::string {
  return expr->GetValue().ToString();
}

auto ToStringUnaryExpr(const UnaryPtr& expr) -> std::string {
  return Parenthesize(expr->GetOperator().GetLexeme(),
                      expr->GetRightExpression());
}
}  // namespace

auto ToStringExpr(const ExprPtr& expr) -> std::string {
  return std::visit(
      [](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, BinaryPtr>) {
          return ToStringBinaryExpr(arg);
        } else if constexpr (std::is_same_v<T, GroupingPtr>) {
          return ToStringGroupExpr(arg);
        } else if constexpr (std::is_same_v<T, LiteralPtr>) {
          return ToStringLiteralExpr(arg);
        } else if constexpr (std::is_same_v<T, UnaryPtr>) {
          return ToStringUnaryExpr(arg);
        }
      },
      expr);
}

}  // namespace cclox::ASTPrinter
