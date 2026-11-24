#include <limits>

#include <gtest/gtest.h>

#include "calculator/core/engine.hpp"

using calculator::CalculatorEngine;
using calculator::ErrorCode;

class CalculatorEngineTest : public ::testing::Test {
protected:
    CalculatorEngine engine;
};

TEST_F(CalculatorEngineTest, Addition) {
    const auto result = engine.add(10.0, 20.0);

    EXPECT_TRUE(result.success());
    EXPECT_DOUBLE_EQ(result.value, 30.0);
}

TEST_F(CalculatorEngineTest, Subtraction) {
    const auto result = engine.subtract(20.0, 10.0);

    EXPECT_TRUE(result.success());
    EXPECT_DOUBLE_EQ(result.value, 10.0);
}

TEST_F(CalculatorEngineTest, Multiplication) {
    const auto result = engine.multiply(5.0, 4.0);

    EXPECT_TRUE(result.success());
    EXPECT_DOUBLE_EQ(result.value, 20.0);
}

TEST_F(CalculatorEngineTest, Division) {
    const auto result = engine.divide(20.0, 4.0);

    EXPECT_TRUE(result.success());
    EXPECT_DOUBLE_EQ(result.value, 5.0);
}

TEST_F(CalculatorEngineTest, DivisionByZero) {
    const auto result = engine.divide(10.0, 0.0);

    EXPECT_FALSE(result.success());
    EXPECT_EQ(result.error, ErrorCode::division_by_zero);
}

TEST_F(CalculatorEngineTest, DetectsArithmeticOverflow) {
    const double maximum = std::numeric_limits<double>::max();

    const auto addition = engine.add(maximum, maximum);
    const auto subtraction = engine.subtract(maximum, -maximum);
    const auto multiplication = engine.multiply(maximum, 2.0);
    const auto division =
        engine.divide(maximum, std::numeric_limits<double>::min());

    EXPECT_EQ(addition.error, ErrorCode::numeric_overflow);
    EXPECT_EQ(subtraction.error, ErrorCode::numeric_overflow);
    EXPECT_EQ(multiplication.error, ErrorCode::numeric_overflow);
    EXPECT_EQ(division.error, ErrorCode::numeric_overflow);
}

TEST_F(CalculatorEngineTest, RejectsNonFiniteInputs) {
    const double infinity = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();

    EXPECT_EQ(engine.add(infinity, 1.0).error, ErrorCode::numeric_overflow);

    EXPECT_EQ(engine.subtract(1.0, nan).error, ErrorCode::numeric_overflow);

    EXPECT_EQ(engine.multiply(infinity, 0.0).error,
              ErrorCode::numeric_overflow);

    EXPECT_EQ(engine.divide(1.0, infinity).error, ErrorCode::numeric_overflow);
}

TEST_F(CalculatorEngineTest, PreservesValidArithmeticResults) {
    EXPECT_DOUBLE_EQ(engine.add(10.0, 5.0).value, 15.0);
    EXPECT_DOUBLE_EQ(engine.subtract(10.0, 5.0).value, 5.0);
    EXPECT_DOUBLE_EQ(engine.multiply(10.0, 5.0).value, 50.0);
    EXPECT_DOUBLE_EQ(engine.divide(10.0, 5.0).value, 2.0);
}
