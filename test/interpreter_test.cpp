#include "interpreter.h"

#include <gtest/gtest.h>
#include <cstddef>
#include <variant>
#include <vector>

#include "parser.h"
#include "scanner.h"
#include "token.h"

using namespace cclox;

Object Interpret(const std::string& source) {
  Scanner scanner{source};
  std::vector<cclox::Token> tokens = scanner.ScanTokens();

  Parser parser{std::move(tokens)};
  ExprPtr expression = parser.Parse();
  Interpreter interpreter;

  return interpreter.Evaluate(expression);
}

TEST(InterpreterTest, BasicLiteralsTest) {
  auto true_obj = Interpret("true");
  EXPECT_TRUE(true_obj.IsBool());
  EXPECT_EQ(true_obj.Get<bool>(), true);
  EXPECT_EQ(true_obj.ToString(), "true");

  auto false_obj = Interpret("false");
  EXPECT_TRUE(false_obj.IsBool());
  EXPECT_EQ(false_obj.Get<bool>(), false);
  EXPECT_EQ(false_obj.ToString(), "false");

  auto nil = Interpret("nil");
  EXPECT_TRUE(nil.IsNil());
  EXPECT_EQ(nil.Get<std::nullptr_t>(), nullptr);
  EXPECT_EQ(nil.ToString(), "nil");

  auto integer = Interpret("123");
  EXPECT_TRUE(integer.IsInteger());
  EXPECT_TRUE(integer.AsInteger());
  // Integer should be convertible to double.
  EXPECT_TRUE(integer.AsDouble());
  EXPECT_EQ(integer.Get<int32_t>(), 123);
  EXPECT_EQ(integer.ToString(), "123");

  auto decimal = Interpret("123.456");
  EXPECT_TRUE(decimal.IsDouble());
  EXPECT_TRUE(decimal.AsDouble());
  // Double should be convertible to integer.
  EXPECT_TRUE(decimal.AsInteger());
  EXPECT_EQ(decimal.Get<double>(), 123.456);
  EXPECT_EQ(decimal.ToString(), "123.456");

  auto str = Interpret("\"hello world\"");
  EXPECT_TRUE(str.IsString());
  EXPECT_TRUE(str.AsString());
  EXPECT_EQ(str.Get<std::string>(), "hello world");
}

TEST(InterpreterTest, ArithmeticOperationsTest) {
  // Basic Integer Operations.
  auto sum1 = Interpret("1 + 2");
  EXPECT_TRUE(sum1.IsInteger());
  EXPECT_EQ(sum1.Get<int32_t>(), 3);

  auto sum2 = Interpret("5 + -3");
  EXPECT_TRUE(sum2.IsInteger());
  EXPECT_EQ(sum2.Get<int32_t>(), 2);

  auto diff1 = Interpret("5 - 3");
  EXPECT_TRUE(diff1.IsInteger());
  EXPECT_EQ(diff1.Get<int32_t>(), 2);

  auto negative = Interpret("-5");
  EXPECT_TRUE(negative.IsInteger());
  EXPECT_EQ(negative.Get<int32_t>(), -5);

  auto product1 = Interpret("4 * 3");
  EXPECT_TRUE(product1.IsInteger());
  EXPECT_EQ(product1.Get<int32_t>(), 12);

  auto quotient1 = Interpret("10 / 2");
  EXPECT_TRUE(quotient1.IsInteger());
  EXPECT_EQ(quotient1.Get<int32_t>(), 5);

  // Mixed Integer and Double Operations.
  auto sum3 = Interpret("1 + 2.5");
  EXPECT_TRUE(sum3.IsDouble());
  EXPECT_EQ(sum3.Get<double>(), 3.5);

  auto diff2 = Interpret("10.5 - 3");
  EXPECT_TRUE(diff2.IsDouble());
  EXPECT_EQ(diff2.Get<double>(), 7.5);

  auto product2 = Interpret("4.2 * 3");
  EXPECT_TRUE(product2.IsDouble());
  EXPECT_EQ(product2.Get<double>(), 4.2 * 3);

  auto quotient2 = Interpret("10.0 / 2");
  EXPECT_TRUE(quotient2.IsDouble());
  EXPECT_EQ(quotient2.Get<double>(), 10.0 / 2);

  auto quotient3 = Interpret("3 / 2.0");
  EXPECT_TRUE(quotient3.IsDouble());
  EXPECT_EQ(quotient3.Get<double>(), 3 / 2.0);

  // Overflow arithmetic converts the result to double type.
  auto sum4 = Interpret("2147483647 + 1");
  EXPECT_TRUE(sum4.IsDouble());
  EXPECT_EQ(sum4.Get<double>(), static_cast<double>(INT_MAX) + 1);

  // (INT_MIN - 1) should lead to overflow, so the interpreter converts the
  // result to double type internally.
  auto diff3 = Interpret("-2147483648 - 1");
  EXPECT_TRUE(diff3.IsDouble());
  EXPECT_EQ(diff3.Get<double>(), static_cast<double>(INT_MIN) - 1);

  // The parser only parses '-2147483648' and discards '-1'.
  auto diff4 = Interpret("-2147483648 -1");
  EXPECT_TRUE(diff4.IsInteger());
  EXPECT_EQ(diff4.Get<int32_t>(), INT_MIN);

  auto str1 = Interpret("\"hello \" + \"world\"");
  EXPECT_TRUE(str1.IsString());
  EXPECT_EQ(str1.Get<std::string>(), "hello world");

  auto str2 = Interpret("\"abc\" + \"123\"");
  EXPECT_TRUE(str2.IsString());
  EXPECT_EQ(str2.Get<std::string>(), "abc123");

  auto str3 = Interpret("\"test\" + \"\" +\"concatenation\"");
  EXPECT_TRUE(str3.IsString());
  EXPECT_EQ(str3.Get<std::string>(), "testconcatenation");
}

TEST(InterpreterTest, ComparisonOperationsTest) {
  auto greater = Interpret("2147483647 + 1 > 2147483647");
  EXPECT_TRUE(greater.IsBool());
  EXPECT_EQ(greater.IsTruthy(), true);

  auto greater_equal = Interpret("2147483647 >= 2147483647");
  EXPECT_TRUE(greater_equal.IsBool());
  EXPECT_EQ(greater_equal.IsTruthy(), true);

  auto less = Interpret("-2147483648 - 1 < -2147483648");
  EXPECT_TRUE(less.IsBool());
  EXPECT_EQ(less.IsTruthy(), true);

  auto less_equal = Interpret("-2147483648 <= -2147483648");
  EXPECT_TRUE(less_equal.IsBool());
  EXPECT_EQ(less_equal.IsTruthy(), true);

  auto equal = Interpret("10 == 10.0");
  EXPECT_TRUE(equal.IsBool());
  EXPECT_EQ(equal.IsTruthy(), 10 == 10.0);

  auto not_equal = Interpret("26.4 != 26");
  EXPECT_TRUE(not_equal.IsBool());
  EXPECT_EQ(not_equal.IsTruthy(), 26.4 != 26);

  auto negate = Interpret("!true");
  EXPECT_TRUE(negate.IsBool());
  EXPECT_EQ(negate.IsTruthy(), false);
}

TEST(InterpreterTest, ComplexExpressionsTest) {
  auto expr1 = Interpret("1 + 2 * 3");
  EXPECT_TRUE(expr1.IsInteger());
  EXPECT_EQ(expr1.Get<int32_t>(), 7);

  auto expr2 = Interpret("(1 + 2) * 3");
  EXPECT_TRUE(expr2.IsInteger());
  EXPECT_EQ(expr2.Get<int32_t>(), 9);

  auto expr3 = Interpret("-1 + 2");
  EXPECT_TRUE(expr3.IsInteger());
  EXPECT_EQ(expr3.Get<int32_t>(), 1);

  auto expr4 = Interpret("!(5 > 3)");
  EXPECT_TRUE(expr4.IsBool());
  EXPECT_EQ(expr4.IsTruthy(), false);

  auto expr5 = Interpret("1 + 2 + 3 + 4 + 5");
  EXPECT_TRUE(expr5.IsInteger());
  EXPECT_EQ(expr5.Get<int32_t>(), 15);

  auto expr6 = Interpret("3 * 4 + 5 / 2");
  EXPECT_TRUE(expr6.IsInteger());
  EXPECT_EQ(expr6.Get<int32_t>(), 14);

  auto expr7 = Interpret("1 + 2 > 3 * 4");
  EXPECT_TRUE(expr7.IsBool());
  EXPECT_EQ(expr7.IsTruthy(), false);
}