
#include <gtest/gtest.h>

#include "calculator/core/history.hpp"

namespace calculator {
namespace {

class CalculationHistoryTest : public ::testing::Test {
protected:
    CalculationHistory history;
};

TEST_F(CalculationHistoryTest, StartsEmpty) {
    EXPECT_TRUE(history.entries().empty());
    EXPECT_FALSE(history.previous_answer().has_value());
}

TEST_F(CalculationHistoryTest, StoresSuccessfulCalculation) {
    const CalculationResult result{40.0, ErrorCode::none};

    EXPECT_TRUE(history.add("25 + 15", result));

    ASSERT_EQ(history.entries().size(), 1U);
    EXPECT_EQ(history.entries()[0].expression, "25 + 15");
    EXPECT_DOUBLE_EQ(history.entries()[0].result, 40.0);
}

TEST_F(CalculationHistoryTest, RejectsFailedCalculation) {
    const CalculationResult result{
        0.0,
        ErrorCode::division_by_zero
    };

    EXPECT_FALSE(history.add("10 / 0", result));
    EXPECT_TRUE(history.entries().empty());
}

TEST_F(CalculationHistoryTest, PreservesCalculationOrder) {
    EXPECT_TRUE(history.add(
        "10 + 5",
        CalculationResult{15.0, ErrorCode::none}
    ));

    EXPECT_TRUE(history.add(
        "20 * 2",
        CalculationResult{40.0, ErrorCode::none}
    ));

    ASSERT_EQ(history.entries().size(), 2U);

    EXPECT_EQ(history.entries()[0].expression, "10 + 5");
    EXPECT_EQ(history.entries()[1].expression, "20 * 2");
}

TEST_F(CalculationHistoryTest, ReturnsLatestAnswer) {
    EXPECT_TRUE(history.add(
        "10 + 5",
        CalculationResult{15.0, ErrorCode::none}
    ));

    EXPECT_TRUE(history.add(
        "20 * 2",
        CalculationResult{40.0, ErrorCode::none}
    ));

    ASSERT_TRUE(history.previous_answer().has_value());
    EXPECT_DOUBLE_EQ(*history.previous_answer(), 40.0);
}

TEST_F(CalculationHistoryTest, ClearRemovesHistoryAndAnswer) {
    EXPECT_TRUE(history.add(
        "5 + 5",
        CalculationResult{10.0, ErrorCode::none}
    ));

    history.clear();

    EXPECT_TRUE(history.entries().empty());
    EXPECT_FALSE(history.previous_answer().has_value());
}

TEST_F(CalculationHistoryTest, RejectsEmptyExpression) {
    const CalculationResult result{10.0, ErrorCode::none};

    EXPECT_FALSE(history.add("", result));
    EXPECT_TRUE(history.entries().empty());
}

TEST_F(CalculationHistoryTest, RejectsNaNResult) {
    const CalculationResult result{
        std::numeric_limits<double>::quiet_NaN(),
        ErrorCode::none
    };

    EXPECT_FALSE(history.add("invalid", result));
    EXPECT_TRUE(history.entries().empty());
}

TEST_F(CalculationHistoryTest, RejectsInfiniteResult) {
    const CalculationResult result{
        std::numeric_limits<double>::infinity(),
        ErrorCode::none
    };

    EXPECT_FALSE(history.add("overflow", result));
    EXPECT_TRUE(history.entries().empty());
}

TEST_F(CalculationHistoryTest, CanBeReusedAfterClear) {
    EXPECT_TRUE(history.add(
        "5 + 5",
        CalculationResult{10.0, ErrorCode::none}
    ));

    history.clear();

    EXPECT_TRUE(history.add(
        "20 + 30",
        CalculationResult{50.0, ErrorCode::none}
    ));

    ASSERT_EQ(history.entries().size(), 1U);
    EXPECT_EQ(history.entries()[0].expression, "20 + 30");
    EXPECT_DOUBLE_EQ(*history.previous_answer(), 50.0);
}

}
}
