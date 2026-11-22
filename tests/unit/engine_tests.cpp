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
