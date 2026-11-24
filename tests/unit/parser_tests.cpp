
#include <gtest/gtest.h>

#include <cmath>
#include <string_view>

#include "calculator/core/engine.hpp"

namespace {

class ExpressionParserTest : public ::testing::Test {
protected:
    calculator::CalculatorEngine engine;

    calculator::CalculationResult evaluate(std::string_view expression) const {
        return engine.evaluate(expression);
    }
};

TEST_F(ExpressionParserTest, EvaluatesBasicArithmetic) {
    EXPECT_DOUBLE_EQ(evaluate("10 + 20").value, 30.0);
    EXPECT_DOUBLE_EQ(evaluate("10 - 4").value, 6.0);
    EXPECT_DOUBLE_EQ(evaluate("6 * 7").value, 42.0);
    EXPECT_DOUBLE_EQ(evaluate("20 / 4").value, 5.0);
}

TEST_F(ExpressionParserTest, RespectsOperatorPrecedence) {
    EXPECT_DOUBLE_EQ(evaluate("2 + 3 * 4").value, 14.0);
    EXPECT_DOUBLE_EQ(evaluate("(2 + 3) * 4").value, 20.0);
    EXPECT_DOUBLE_EQ(evaluate("20 - 6 / 2").value, 17.0);
}

TEST_F(ExpressionParserTest, EvaluatesPowers) {
    EXPECT_DOUBLE_EQ(evaluate("2^3").value, 8.0);
    EXPECT_DOUBLE_EQ(evaluate("2^3^2").value, 512.0);
    EXPECT_DOUBLE_EQ(evaluate("2^-2").value, 0.25);
}

TEST_F(ExpressionParserTest, EvaluatesUnaryOperators) {
    EXPECT_DOUBLE_EQ(evaluate("-5").value, -5.0);
    EXPECT_DOUBLE_EQ(evaluate("+5").value, 5.0);
    EXPECT_DOUBLE_EQ(evaluate("-2^2").value, -4.0);
}

TEST_F(ExpressionParserTest, EvaluatesMathematicalConstants) {
    EXPECT_DOUBLE_EQ(evaluate("pi").value, std::acos(-1.0));

    EXPECT_DOUBLE_EQ(evaluate("e").value, std::exp(1.0));
}

TEST_F(ExpressionParserTest, EvaluatesTrigonometricFunctionsInDegrees) {
    EXPECT_NEAR(evaluate("sin(30)").value, 0.5, 1e-12);
    EXPECT_NEAR(evaluate("cos(60)").value, 0.5, 1e-12);
    EXPECT_NEAR(evaluate("tan(45)").value, 1.0, 1e-12);
}

TEST_F(ExpressionParserTest, EvaluatesInverseTrigonometricFunctions) {
    EXPECT_NEAR(evaluate("asin(0.5)").value, 30.0, 1e-12);
    EXPECT_NEAR(evaluate("acos(0.5)").value, 60.0, 1e-12);
    EXPECT_NEAR(evaluate("atan(1)").value, 45.0, 1e-12);
}

TEST_F(ExpressionParserTest, EvaluatesScientificFunctions) {
    EXPECT_DOUBLE_EQ(evaluate("sqrt(81)").value, 9.0);
    EXPECT_DOUBLE_EQ(evaluate("abs(-12)").value, 12.0);
    EXPECT_NEAR(evaluate("ln(e)").value, 1.0, 1e-12);
    EXPECT_DOUBLE_EQ(evaluate("log(100)").value, 2.0);
    EXPECT_NEAR(evaluate("exp(1)").value, std::exp(1.0), 1e-12);
}

TEST_F(ExpressionParserTest, EvaluatesNestedExpressions) {
    EXPECT_DOUBLE_EQ(evaluate("sqrt(16) + 2 * (3 + 4)").value, 18.0);

    EXPECT_NEAR(evaluate("sin(30) + cos(60)").value, 1.0, 1e-12);
}

TEST_F(ExpressionParserTest, ReportsDivisionByZero) {
    const auto result = evaluate("10 / 0");

    EXPECT_FALSE(result.success());
    EXPECT_EQ(result.error, calculator::ErrorCode::division_by_zero);
}

TEST_F(ExpressionParserTest, ReportsInvalidExpressions) {
    const auto empty = evaluate("");
    const auto incomplete = evaluate("2 +");
    const auto invalid = evaluate("2 + * 3");

    EXPECT_FALSE(empty.success());
    EXPECT_FALSE(incomplete.success());
    EXPECT_FALSE(invalid.success());
}

TEST_F(ExpressionParserTest, ReportsMismatchedParentheses) {
    const auto missing_closing = evaluate("(2 + 3");
    const auto extra_closing = evaluate("2 + 3)");

    EXPECT_FALSE(missing_closing.success());
    EXPECT_FALSE(extra_closing.success());
}

TEST_F(ExpressionParserTest, ReportsUnknownIdentifiers) {
    const auto result = evaluate("unknown(5)");

    EXPECT_FALSE(result.success());
    EXPECT_EQ(result.error, calculator::ErrorCode::unknown_identifier);
}

TEST_F(ExpressionParserTest, ReportsMathematicalDomainErrors) {
    const auto square_root = evaluate("sqrt(-1)");
    const auto logarithm = evaluate("log(0)");
    const auto inverse_sine = evaluate("asin(2)");

    EXPECT_FALSE(square_root.success());
    EXPECT_FALSE(logarithm.success());
    EXPECT_FALSE(inverse_sine.success());

    EXPECT_EQ(square_root.error, calculator::ErrorCode::domain_error);
}

TEST_F(ExpressionParserTest, ReportsNumericOverflow) {
    const auto result = evaluate("1e308 * 1e308");

    EXPECT_FALSE(result.success());
    EXPECT_EQ(result.error, calculator::ErrorCode::numeric_overflow);
}

TEST_F(ExpressionParserTest, SupportsWhitespace) {
    const auto result = evaluate("  ( 10 + 5 ) * 2  ");

    ASSERT_TRUE(result.success());
    EXPECT_DOUBLE_EQ(result.value, 30.0);
}

} // namespace
TEST_F(ExpressionParserTest, SupportsImplicitMultiplication) {
    EXPECT_NEAR(evaluate("2pi").value, 2.0 * std::acos(-1.0), 1e-12);

    EXPECT_DOUBLE_EQ(evaluate("2(3+4)").value, 14.0);

    EXPECT_DOUBLE_EQ(evaluate("(2+3)(4+5)").value, 45.0);

    EXPECT_NEAR(evaluate("2sin(30)").value, 1.0, 1e-12);

    EXPECT_DOUBLE_EQ(evaluate("3sqrt(16)").value, 12.0);

    EXPECT_NEAR(evaluate("2pi(3)").value, 6.0 * std::acos(-1.0), 1e-12);

    EXPECT_DOUBLE_EQ(evaluate("(2+3)2").value, 10.0);

    EXPECT_DOUBLE_EQ(evaluate("sin(30)2").value, 1.0);
}

TEST_F(ExpressionParserTest, SupportsScientificNotation) {
    EXPECT_DOUBLE_EQ(evaluate("1.5e2").value, 150.0);
    EXPECT_DOUBLE_EQ(evaluate("2.5e-3").value, 0.0025);
    EXPECT_DOUBLE_EQ(evaluate("1e+3").value, 1000.0);
}

TEST_F(ExpressionParserTest, HandlesComplexOperatorCombinations) {
    EXPECT_DOUBLE_EQ(evaluate("2 + 3 * 4^2").value, 50.0);
    EXPECT_DOUBLE_EQ(evaluate("2^3^2").value, 512.0);
    EXPECT_DOUBLE_EQ(evaluate("-(2 + 3) * 4").value, -20.0);
    EXPECT_DOUBLE_EQ(evaluate("2^-2^2").value, 0.0625);
}

TEST_F(ExpressionParserTest, RejectsMalformedExpressions) {
    const std::string_view expressions[] =
        {".", "1..2", "2**3", "2^^3", "()", "sin()", "2(3", "1e", "2 3"};

    for (const auto expression : expressions) {
        const auto result = evaluate(expression);

        EXPECT_FALSE(result.success())
            << "Expression should be rejected: " << expression;
    }
}

TEST_F(ExpressionParserTest, HandlesNegativeZeroDivision) {
    const auto result = evaluate("1 / -0");

    EXPECT_FALSE(result.success());
    EXPECT_EQ(result.error, calculator::ErrorCode::division_by_zero);
}

TEST_F(ExpressionParserTest, ReportsPowerDomainErrors) {
    const auto result = evaluate("(-2)^0.5");

    EXPECT_FALSE(result.success());
    EXPECT_EQ(result.error, calculator::ErrorCode::domain_error);
}

TEST_F(ExpressionParserTest, DetectsArithmeticOverflow) {
    const auto addition = evaluate("1e308 + 1e308");
    const auto multiplication = evaluate("1e308 * 1e308");

    EXPECT_FALSE(addition.success());
    EXPECT_FALSE(multiplication.success());

    EXPECT_EQ(addition.error, calculator::ErrorCode::numeric_overflow);

    EXPECT_EQ(multiplication.error, calculator::ErrorCode::numeric_overflow);
}

TEST_F(ExpressionParserTest, ReportsPowerOverflow) {
    const auto result = evaluate("1e308^2");

    EXPECT_FALSE(result.success());
    EXPECT_EQ(result.error, calculator::ErrorCode::numeric_overflow);
}

TEST_F(ExpressionParserTest, ReportsNumericLiteralOverflow) {
    const auto result = evaluate("1e9999");

    EXPECT_FALSE(result.success());
    EXPECT_EQ(result.error, calculator::ErrorCode::numeric_overflow);
}

TEST_F(ExpressionParserTest, RejectsIncompleteSignedScientificNotation) {
    const auto positive_exponent = evaluate("1e+");
    const auto negative_exponent = evaluate("1e-");

    EXPECT_FALSE(positive_exponent.success());
    EXPECT_FALSE(negative_exponent.success());

    EXPECT_EQ(positive_exponent.error, calculator::ErrorCode::invalid_number);

    EXPECT_EQ(negative_exponent.error, calculator::ErrorCode::invalid_number);
}

TEST_F(ExpressionParserTest, HandlesErrorsInsideDivisionOperands) {
    const auto result = evaluate("1/(2+)");

    EXPECT_FALSE(result.success());
    EXPECT_EQ(result.error, calculator::ErrorCode::invalid_expression);
}

TEST_F(ExpressionParserTest, ReportsMissingFunctionClosingParenthesis) {
    const auto result = evaluate("sqrt(4");

    EXPECT_FALSE(result.success());
    EXPECT_EQ(result.error, calculator::ErrorCode::mismatched_parentheses);
}
