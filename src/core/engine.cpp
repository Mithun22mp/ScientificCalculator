
#include "calculator/core/engine.hpp"

#include "calculator/parser/expression_parser.hpp"

namespace calculator {

bool CalculationResult::success() const noexcept {
    return error == ErrorCode::none;
}

CalculationResult CalculatorEngine::add(
    double a, double b
) const noexcept {
    return {a + b, ErrorCode::none};
}

CalculationResult CalculatorEngine::subtract(
    double a, double b
) const noexcept {
    return {a - b, ErrorCode::none};
}

CalculationResult CalculatorEngine::multiply(
    double a, double b
) const noexcept {
    return {a * b, ErrorCode::none};
}

CalculationResult CalculatorEngine::divide(
    double a, double b
) const noexcept {
    if (b == 0.0) {
        return {0.0, ErrorCode::division_by_zero};
    }

    return {a / b, ErrorCode::none};
}

CalculationResult CalculatorEngine::evaluate(
    std::string_view expression
) const {
    const ExpressionParser parser;
    return parser.evaluate(expression);
}

}
