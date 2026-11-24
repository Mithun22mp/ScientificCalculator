
#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "calculator/app/calculator_app.hpp"

namespace calculator {
namespace {

std::string run_application(const std::string& commands) {
    std::istringstream input(commands);
    std::ostringstream output;

    CalculatorApp application;
    application.run(input, output);

    return output.str();
}

TEST(CalculatorAppTest, CalculatesAndReusesPreviousAnswer) {
    const std::string output = run_application(
        "10 + 5\n"
        "ans + 10\n"
        "ans * 2\n"
        "exit\n"
    );

    EXPECT_NE(output.find("= 15"), std::string::npos);
    EXPECT_NE(output.find("= 25"), std::string::npos);
    EXPECT_NE(output.find("= 50"), std::string::npos);
}

TEST(CalculatorAppTest, DisplaysCalculationHistory) {
    const std::string output = run_application(
        "10 + 5\n"
        "ans + 10\n"
        "history\n"
        "exit\n"
    );

    EXPECT_NE(output.find("1. 10 + 5 = 15"), std::string::npos);
    EXPECT_NE(output.find("2. ans + 10 = 25"), std::string::npos);
}

TEST(CalculatorAppTest, ClearsCalculationHistory) {
    const std::string output = run_application(
        "10 + 5\n"
        "clear\n"
        "history\n"
        "exit\n"
    );

    EXPECT_NE(
        output.find("Calculation history cleared."),
        std::string::npos
    );

    EXPECT_NE(
        output.find("No calculations in history."),
        std::string::npos
    );
}

TEST(CalculatorAppTest, RejectsPreviousAnswerWithoutHistory) {
    const std::string output = run_application(
        "ans + 5\n"
        "exit\n"
    );

    EXPECT_NE(
        output.find("No previous answer available."),
        std::string::npos
    );
}

TEST(CalculatorAppTest, FailedCalculationPreservesPreviousAnswer) {
    const std::string output = run_application(
        "10 + 5\n"
        "10 / 0\n"
        "ans + 1\n"
        "exit\n"
    );

    EXPECT_NE(
        output.find("Error: Division by zero."),
        std::string::npos
    );

    EXPECT_NE(output.find("= 16"), std::string::npos);
}

TEST(CalculatorAppTest, QuitCommandClosesApplication) {
    const std::string output = run_application("quit\n");

    EXPECT_NE(
        output.find("Calculator closed."),
        std::string::npos
    );
}

TEST(CalculatorAppTest, HandlesEndOfInput) {
    const std::string output = run_application("");

    EXPECT_NE(
        output.find("Calculator closed."),
        std::string::npos
    );
}

} // namespace
} // namespace calculator
