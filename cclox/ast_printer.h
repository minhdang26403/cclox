#ifndef AST_PRINTER_H_
#define AST_PRINTER_H_

#include "expr.h"

namespace cclox::ASTPrinter {
/**
 * @brief Prints the abstract syntax tree.
 * @param expr the expression to print.
 * @return the string representation of the expression.
 */
auto ToStringExpr(const ExprPtr& expr) -> std::string;
}

#endif // AST_PRINTER_H_
