#include "interpreter.h"

#include <gtest/gtest.h>
#include <cstddef>
#include <variant>
#include <vector>

#include "parser.h"
#include "scanner.h"
#include "stmt.h"
#include "token.h"

using cclox::Scanner, cclox::Parser, cclox::Interpreter;
using cclox::StmtPtr, cclox::ExprStmtPtr;

TEST(ExpressionTest, BasicLiteralsTest) {
  std::string source =
      "true;"
      "false;"
      "nil;"
      "123;"
      "123.456;"
      "\"hello world\";";

  Scanner scanner{std::move(source)};
  std::vector<cclox::Token> tokens = scanner.ScanTokens();

  Parser parser{std::move(tokens)};
  std::vector<StmtPtr> statements = parser.Parse();
  Interpreter interpreter;

  // true;
  auto true_obj = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[0])->GetExpression());
  EXPECT_TRUE(true_obj.IsBool());
  EXPECT_EQ(true_obj.Get<bool>(), true);
  EXPECT_EQ(true_obj.ToString(), "true");

  // false;
  auto false_obj = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[1])->GetExpression());
  EXPECT_TRUE(false_obj.IsBool());
  EXPECT_EQ(false_obj.Get<bool>(), false);
  EXPECT_EQ(false_obj.ToString(), "false");

  // nil;
  auto nil = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[2])->GetExpression());
  EXPECT_TRUE(nil.IsNil());
  EXPECT_EQ(nil.Get<std::nullptr_t>(), nullptr);
  EXPECT_EQ(nil.ToString(), "nil");

  // 123;
  auto integer = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[3])->GetExpression());
  EXPECT_TRUE(integer.IsInteger());
  EXPECT_TRUE(integer.AsInteger());
  // Integer should be convertible to double.
  EXPECT_TRUE(integer.AsDouble());
  EXPECT_EQ(integer.Get<int32_t>(), 123);
  EXPECT_EQ(integer.ToString(), "123");

  // 123.456;
  auto decimal = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[4])->GetExpression());
  EXPECT_TRUE(decimal.IsDouble());
  EXPECT_TRUE(decimal.AsDouble());
  // Double should be convertible to integer.
  EXPECT_TRUE(decimal.AsInteger());
  EXPECT_EQ(decimal.Get<double>(), 123.456);
  EXPECT_EQ(decimal.ToString(), "123.456");

  // "hello world";
  auto str = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[5])->GetExpression());
  EXPECT_TRUE(str.IsString());
  EXPECT_TRUE(str.AsString());
  EXPECT_EQ(str.Get<std::string>(), "hello world");
}

TEST(ExpressionTest, ArithmeticOperationsTest) {
  std::string source =
      "1 + 2;"
      "5 + -3;"
      "5 - 3;"
      "-5;"
      "4 * 3;"
      "10 / 2;"
      "1 + 2.5;"
      "10.5 - 3;"
      "4.2 * 3;"
      "10.0 / 2;"
      "3 / 2.0;"
      "2147483647 + 1;"
      "-2147483648 - 1;"
      "-2147483648 -1;"
      "\"hello \" + \"world\";"
      "\"abc\" + \"123\";"
      "\"test\" + \"\" +\"concatenation\";";

  Scanner scanner{std::move(source)};
  std::vector<cclox::Token> tokens = scanner.ScanTokens();

  Parser parser{std::move(tokens)};
  std::vector<StmtPtr> statements = parser.Parse();
  Interpreter interpreter;

  // Basic Integer Operations.
  // 1 + 2;
  auto sum1 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[0])->GetExpression());
  EXPECT_TRUE(sum1.IsInteger());
  EXPECT_EQ(sum1.Get<int32_t>(), 3);

  // 5 + -3;
  auto sum2 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[1])->GetExpression());
  EXPECT_TRUE(sum2.IsInteger());
  EXPECT_EQ(sum2.Get<int32_t>(), 2);

  // 5 - 3;
  auto diff1 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[2])->GetExpression());
  EXPECT_TRUE(diff1.IsInteger());
  EXPECT_EQ(diff1.Get<int32_t>(), 2);

  // -5;
  auto negative = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[3])->GetExpression());
  EXPECT_TRUE(negative.IsInteger());
  EXPECT_EQ(negative.Get<int32_t>(), -5);

  // 4 * 3;
  auto product1 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[4])->GetExpression());
  EXPECT_TRUE(product1.IsInteger());
  EXPECT_EQ(product1.Get<int32_t>(), 12);

  // 10 / 2;
  auto quotient1 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[5])->GetExpression());
  EXPECT_TRUE(quotient1.IsInteger());
  EXPECT_EQ(quotient1.Get<int32_t>(), 5);

  // Mixed Integer and Double Operations.
  // 1 + 2.5;
  auto sum3 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[6])->GetExpression());
  EXPECT_TRUE(sum3.IsDouble());
  EXPECT_EQ(sum3.Get<double>(), 3.5);

  // 10.5 - 3;
  auto diff2 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[7])->GetExpression());
  EXPECT_TRUE(diff2.IsDouble());
  EXPECT_EQ(diff2.Get<double>(), 7.5);

  // 4.2 * 3;
  auto product2 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[8])->GetExpression());
  EXPECT_TRUE(product2.IsDouble());
  EXPECT_EQ(product2.Get<double>(), 4.2 * 3);

  // 10.0 / 2;
  auto quotient2 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[9])->GetExpression());
  EXPECT_TRUE(quotient2.IsDouble());
  EXPECT_EQ(quotient2.Get<double>(), 10.0 / 2);

  // 3 / 2.0;
  auto quotient3 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[10])->GetExpression());
  EXPECT_TRUE(quotient3.IsDouble());
  EXPECT_EQ(quotient3.Get<double>(), 3 / 2.0);

  // 2147483647 + 1;
  // Overflow arithmetic converts the result to double type.
  auto sum4 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[11])->GetExpression());
  EXPECT_TRUE(sum4.IsDouble());
  EXPECT_EQ(sum4.Get<double>(), static_cast<double>(INT_MAX) + 1);

  // -2147483648 - 1;
  // (INT_MIN - 1) should lead to overflow, so the interpreter converts the
  // result to double type internally.
  auto diff3 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[12])->GetExpression());
  EXPECT_TRUE(diff3.IsDouble());
  EXPECT_EQ(diff3.Get<double>(), static_cast<double>(INT_MIN) - 1);

  // -2147483648 -1;s
  // The parser only parses '-2147483648' and discards '-1'.
  EXPECT_EQ(std::get<0>(statements[13]), nullptr);

  // "hello " + "world";
  auto str1 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[14])->GetExpression());
  EXPECT_TRUE(str1.IsString());
  EXPECT_EQ(str1.Get<std::string>(), "hello world");

  // "abc" + "123";
  auto str2 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[15])->GetExpression());
  EXPECT_TRUE(str2.IsString());
  EXPECT_EQ(str2.Get<std::string>(), "abc123");

  // "test" + "" + "concatenation";
  auto str3 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[16])->GetExpression());
  EXPECT_TRUE(str3.IsString());
  EXPECT_EQ(str3.Get<std::string>(), "testconcatenation");
}

TEST(ExpressionTest, ComparisonOperationsTest) {
  std::string source =
      "2147483647 + 1 > 2147483647;"
      "2147483647 >= 2147483647;"
      "-2147483648 - 1 < -2147483648;"
      "-2147483648 <= -2147483648;"
      "10 == 10.0;"
      "26.4 != 26;"
      "!true;";

  Scanner scanner{std::move(source)};
  std::vector<cclox::Token> tokens = scanner.ScanTokens();

  Parser parser{std::move(tokens)};
  std::vector<StmtPtr> statements = parser.Parse();
  Interpreter interpreter;

  // 2147483647 + 1 > 2147483647;
  auto greater = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[0])->GetExpression());
  EXPECT_TRUE(greater.IsBool());
  EXPECT_EQ(greater.IsTruthy(), true);

  // 2147483647 >= 2147483647;
  auto greater_equal = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[1])->GetExpression());
  EXPECT_TRUE(greater_equal.IsBool());
  EXPECT_EQ(greater_equal.IsTruthy(), true);

  // -2147483648 - 1 < -2147483648;
  auto less = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[2])->GetExpression());
  EXPECT_TRUE(less.IsBool());
  EXPECT_EQ(less.IsTruthy(), true);

  // -2147483648 <= -2147483648;
  auto less_equal = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[3])->GetExpression());
  EXPECT_TRUE(less_equal.IsBool());
  EXPECT_EQ(less_equal.IsTruthy(), true);

  // 10 == 10.0;
  auto equal = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[4])->GetExpression());
  EXPECT_TRUE(equal.IsBool());
  EXPECT_EQ(equal.IsTruthy(), 10 == 10.0);

  // 26.4 != 26;
  auto not_equal = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[5])->GetExpression());
  EXPECT_TRUE(not_equal.IsBool());
  EXPECT_EQ(not_equal.IsTruthy(), 26.4 != 26);

  // !true;
  auto negate = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[6])->GetExpression());
  EXPECT_TRUE(negate.IsBool());
  EXPECT_EQ(negate.IsTruthy(), false);
}

TEST(ExpressionTest, ComplexExpressionsTest) {
  std::string source =
      "1 + 2 * 3;"
      "(1 + 2) * 3;"
      "-1 + 2;"
      "!(5 > 3);"
      "1 + 2 + 3 + 4 + 5;"
      "3 * 4 + 5 / 2;"
      "1 + 2 > 3 * 4;";

  Scanner scanner{std::move(source)};
  std::vector<cclox::Token> tokens = scanner.ScanTokens();

  Parser parser{std::move(tokens)};
  std::vector<StmtPtr> statements = parser.Parse();
  Interpreter interpreter;

  // 1 + 2 * 3;
  auto expr1 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[0])->GetExpression());
  EXPECT_TRUE(expr1.IsInteger());
  EXPECT_EQ(expr1.Get<int32_t>(), 7);

  // (1 + 2) * 3;
  auto expr2 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[1])->GetExpression());
  EXPECT_TRUE(expr2.IsInteger());
  EXPECT_EQ(expr2.Get<int32_t>(), 9);

  // -1 + 2;
  auto expr3 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[2])->GetExpression());
  EXPECT_TRUE(expr3.IsInteger());
  EXPECT_EQ(expr3.Get<int32_t>(), 1);

  // !(5 > 3);
  auto expr4 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[3])->GetExpression());
  EXPECT_TRUE(expr4.IsBool());
  EXPECT_EQ(expr4.IsTruthy(), false);

  // 1 + 2 + 3 + 4 + 5;
  auto expr5 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[4])->GetExpression());
  EXPECT_TRUE(expr5.IsInteger());
  EXPECT_EQ(expr5.Get<int32_t>(), 15);

  // 3 * 4 + 5 / 2;
  auto expr6 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[5])->GetExpression());
  EXPECT_TRUE(expr6.IsInteger());
  EXPECT_EQ(expr6.Get<int32_t>(), 14);

  // 1 + 2 > 3 * 4;
  auto expr7 = interpreter.EvaluateExpression(
      std::get<ExprStmtPtr>(statements[6])->GetExpression());
  EXPECT_TRUE(expr7.IsBool());
  EXPECT_EQ(expr7.IsTruthy(), false);
}
