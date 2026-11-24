#include "calculator/core/engine.hpp"

#include <cmath>

#include "calculator/parser/expression_parser.hpp"

namespace calculator {
namespace {

bool has_finite_inputs(double a, double b) noexcept {
    return std::isfinite(a) && std::isfinite(b);
}

CalculationResult checked_result(double value) noexcept {
    if (!std::isfinite(value)) {
        return {0.0, ErrorCode::numeric_overflow};
    }

    return {value, ErrorCode::none};
}

} // namespace

bool CalculationResult::success() const noexcept {
    return error == ErrorCode::none;
}

CalculationResult CalculatorEngine::add(
    double a, double b
) const noexcept {
    if (!has_finite_inputs(a, b)) {
        return {0.0, ErrorCode::numeric_overflow};
    }

    return checked_result(a + b);
}

CalculationResult CalculatorEngine::subtract(
    double a, double b
) const noexcept {
    if (!has_finite_inputs(a, b)) {
        return {0.0, ErrorCode::numeric_overflow};
    }

    return checked_result(a - b);
}

CalculationResult CalculatorEngine::multiply(
    double a, double b
) const noexcept {
    if (!has_finite_inputs(a, b)) {
        return {0.0, ErrorCode::numeric_overflow};
    }

    return checked_result(a * b);
}

CalculationResult CalculatorEngine::divide(
    double a, double b
) const noexcept {
    if (!has_finite_inputs(a, b)) {
        return {0.0, ErrorCode::numeric_overflow};
    }

    if (b == 0.0) {
        return {0.0, ErrorCode::division_by_zero};
    }

    return checked_result(a / b);
}

CalculationResult CalculatorEngine::evaluate(
    std::string_view expression
) const {
    const ExpressionParser parser;
    return parser.evaluate(expression);
}

} // namespace calculator
